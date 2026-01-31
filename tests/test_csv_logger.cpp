// ============================================================================
// test_csv_logger.cpp
// Tests for CSV logging functionality
// ============================================================================

#include <gtest/gtest.h>
#include "csv_logger.hpp"
#include "test_helpers.hpp"
#include "types.hpp"
#include <filesystem>
#include <fstream>
#include <thread>
#include <vector>
#include <string_view>

class CSVLoggerTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        test_filename_ = "test_logger.csv";
        std::filesystem::remove(test_filename_);
    }

    void TearDown() override
    {
        std::filesystem::remove(test_filename_);
    }

    std::string test_filename_;
};

TEST_F(CSVLoggerTest, CreateNewLogFile)
{
    CSVLogger logger(test_filename_);
    std::filesystem::exists(test_filename_);
    std::ifstream file(test_filename_);
    if (file.is_open())
    {
        file.seekg(0, std::ios::beg);
        auto size = file.tellg();
        std::string firstLine;
        std::getline(file, firstLine);
        std::string expectedHeader = "timestamp_ms, buy_order, sell_order, price, qty";
        EXPECT_EQ(expectedHeader, firstLine);
    }
}

TEST_F(CSVLoggerTest, AppendToExistingFile)
{
    {
        CSVLogger logger(test_filename_);
        Trade t1(1, 2, 50.0, 100, std::chrono::system_clock::now());
        logger.log_trade(t1);
    }
    Trade t2(1, 2, 50.0, 100, std::chrono::system_clock::now());
    CSVLogger logger(test_filename_);
    logger.log_trade(t2);
    int rows = countCSVLines(test_filename_);
    EXPECT_EQ(3, rows);
}

TEST_F(CSVLoggerTest, LogTradeFormat)
{
    CSVLogger logger(test_filename_);
    Trade t1(1, 2, 50.0, 100, std::chrono::system_clock::now());
    logger.log_trade(t1);
    std::vector<Trade> trades = readAllTrades(test_filename_);
    EXPECT_EQ(1, trades.size());
    Trade tradeRecorded = trades.at(0);
    EXPECT_TRUE(t1 == tradeRecorded);
}

TEST_F(CSVLoggerTest, ConcurrentLogging)
{
    CSVLogger logger(test_filename_);
    std::vector<std::thread> threads;
    auto lambda_func = [&]()
    {
        Trade t1(1, 2, 50.0, 100, std::chrono::system_clock::now());
        for (int i = 0; i < 100; i++)
        {
            logger.log_trade(t1);
        }
    };
    for (int i = 0; i < 10; i++)
    {
        threads.emplace_back(lambda_func);
    }
    for (auto &t : threads)
    {
        t.join();
    }
    int rows = countCSVLines(test_filename_);
    EXPECT_EQ(1001, rows);
}

TEST_F(CSVLoggerTest, LogMultipleTrades)
{
    CSVLogger logger(test_filename_);
    Trade t1(1, 2, 50.0, 100, std::chrono::system_clock::now());
    Trade t2(2, 3, 50.0, 100, std::chrono::system_clock::now());
    Trade t3(3, 4, 50.0, 100, std::chrono::system_clock::now());
    Trade t4(4, 5, 50.0, 100, std::chrono::system_clock::now());
    Trade t5(6, 7, 50.0, 100, std::chrono::system_clock::now());
    std::vector<Trade> trades = {t1, t2, t3, t4, t5};
    for (auto trade : trades)
    {
        logger.log_trade(trade);
    }
    int rows = countCSVLines(test_filename_);
    EXPECT_EQ(6, rows);
    std::vector<Trade> recordedTrades = readAllTrades(test_filename_);
    for (int i = 0; i < trades.size(); i++)
    {
        EXPECT_TRUE(trades[i] == recordedTrades[i]);
    }
}
