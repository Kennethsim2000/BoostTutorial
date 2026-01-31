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
        if (std::filesystem::exists(test_filename_))
        {
            std::filesystem::remove(test_filename_);
        }
    }

    void TearDown() override
    {
        if (std::filesystem::exists(test_filename_))
        {
            std::filesystem::remove(test_filename_);
        }
    }

    std::string test_filename_;
};

TEST_F(CSVLoggerTest, CreateNewLogFile)
{
    CSVLogger logger(test_filename_);
    ASSERT_TRUE(std::filesystem::exists(test_filename_));
    std::ifstream file(test_filename_);
    ASSERT_TRUE(file.is_open()) << "Failed to open test file";
    std::string firstLine;
    std::getline(file, firstLine);
    const std::string expectedHeader = "timestamp_ms, buy_order, sell_order, price, qty";
    EXPECT_EQ(expectedHeader, firstLine);
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
    EXPECT_EQ(3, rows) << "Expected header + 2 trades";
}

TEST_F(CSVLoggerTest, LogTradeFormat)
{
    CSVLogger logger(test_filename_);
    Trade t1(1, 2, 50.0, 100, std::chrono::system_clock::now());
    logger.log_trade(t1);

    std::vector<Trade> trades = readAllTrades(test_filename_);
    EXPECT_EQ(1, trades.size()) << "Should have exactly one trade";

    const Trade &tradeRecorded = trades.at(0);
    EXPECT_EQ(t1, tradeRecorded);
}

TEST_F(CSVLoggerTest, ConcurrentLogging)
{
    CSVLogger logger(test_filename_);
    std::vector<std::thread> threads;

    constexpr int NUM_THREADS = 10;
    constexpr int TRADES_PER_THREAD = 100;

    for (int i = 0; i < NUM_THREADS; i++)
    {
        threads.emplace_back([&logger]()
                             {
                                 Trade t1(1, 2, 50.0, 100, std::chrono::system_clock::now());
                                 for(int j = 0; j < TRADES_PER_THREAD; j++) {
                                    logger.log_trade(t1);
                                 } });
    }
    for (auto &t : threads)
    {
        t.join();
    }
    int rows = countCSVLines(test_filename_);
    EXPECT_EQ(NUM_THREADS * TRADES_PER_THREAD + 1, rows) << "Expected header + " << NUM_THREADS * TRADES_PER_THREAD << " trades";
}

TEST_F(CSVLoggerTest, LogMultipleTrades)
{
    CSVLogger logger(test_filename_);
    const std::vector<Trade> trades = {
        Trade(1, 2, 50.0, 100, std::chrono::system_clock::now()),
        Trade(2, 3, 51.0, 150, std::chrono::system_clock::now()),
        Trade(3, 4, 52.0, 200, std::chrono::system_clock::now()),
        Trade(4, 5, 53.0, 250, std::chrono::system_clock::now()),
        Trade(6, 7, 54.0, 300, std::chrono::system_clock::now())};
    for (const auto &trade : trades)
    {
        logger.log_trade(trade);
    }

    int rows = countCSVLines(test_filename_);
    EXPECT_EQ(trades.size() + 1, rows) << "Expected header + " << trades.size() << " trades";

    std::vector<Trade> recordedTrades = readAllTrades(test_filename_);
    ASSERT_EQ(trades.size(), recordedTrades.size());

    for (size_t i = 0; i < trades.size(); i++)
    {
        EXPECT_EQ(trades[i], recordedTrades[i]) << "Trade mismatch at index " << i;
    }
}
