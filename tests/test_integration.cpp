// ============================================================================
// test_integration.cpp
// Integration tests - testing components working together
// ============================================================================

#include <gtest/gtest.h>
#include "order_book.hpp"
#include "csv_logger.hpp"
#include "types.hpp"
#include <filesystem>
#include <fstream>
#include <sstream>
#include "test_helpers.hpp"

class IntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        if (std::filesystem::exists(test_filename_))
        {
            std::filesystem::remove(test_filename_);
        }
        logger_ = std::make_unique<CSVLogger>(test_filename_);
        book_ = std::make_unique<OrderBook>(*logger_);
    }

    void TearDown() override
    {
        book_.reset();
        logger_.reset();

        if (std::filesystem::exists(test_filename_))
        {
            std::filesystem::remove(test_filename_);
        }
    }

    std::string test_filename_ = "test_trades_integration.csv";
    std::unique_ptr<CSVLogger> logger_;
    std::unique_ptr<OrderBook> book_;
};

TEST_F(IntegrationTest, OrderToTradeToLog)
{
    const Order sell_order = createSellOrder(50.0, 100);
    std::vector<Trade> trades_sell = book_->place_order(sell_order);

    const Order buy_order = createBuyOrder(50.0, 100, "testclient2");
    std::vector<Trade> trades_buy = book_->place_order(buy_order);
    EXPECT_EQ(trades_buy.size(), 1) << "Should generate exactly one trade";
    std::vector<Trade> recordedTrades = readAllTrades(test_filename_);
    EXPECT_EQ(recordedTrades.size(), 1) << "Should log one trade in CSV";
    EXPECT_EQ(recordedTrades[0], trades_buy[0]) << "Trade recorded should be equal to trade returned to client";
}

TEST_F(IntegrationTest, MarketOrderSimulation)
{
    const Order sell_order1 = createSellOrder(50.0, 100);
    const Order sell_order2 = createSellOrder(51.0, 100);
    const Order sell_order3 = createSellOrder(52.0, 100);
    book_->place_order(sell_order1);
    book_->place_order(sell_order2);
    book_->place_order(sell_order3);

    const Order buy_order = createBuyOrder(100.0, 250, "testclient2");
    std::vector<Trade> trades_buy = book_->place_order(buy_order);
    EXPECT_EQ(trades_buy.size(), 3) << "Should generate three trades";
    std::vector<Trade> recordedTrades = readAllTrades(test_filename_);
    EXPECT_EQ(recordedTrades.size(), 3) << "Should log three trade in CSV";
    EXPECT_EQ(trades_buy[0].price, 50.0);
    EXPECT_EQ(trades_buy[0].qty, 100);

    EXPECT_EQ(trades_buy[1].price, 51.0);
    EXPECT_EQ(trades_buy[1].qty, 100);

    EXPECT_EQ(trades_buy[2].price, 52.0);
    EXPECT_EQ(trades_buy[2].qty, 50);
}

TEST_F(IntegrationTest, PartialFillsAndLogging)
{
    const Order sell_order = createSellOrder(50.0, 100);
    book_->place_order(sell_order);
    const Order buy_order = createBuyOrder(50.0, 60, "testclient2");
    book_->place_order(buy_order);
    std::vector<Trade> recordedTrades = readAllTrades(test_filename_);
    EXPECT_EQ(recordedTrades.size(), 1) << "One trade logged";
    EXPECT_EQ(recordedTrades[0].qty, 60) << "Trade logged for 60 shares";
    const Order buy_order2 = createBuyOrder(50.0, 30, "testclient2");
    book_->place_order(buy_order2);
    std::vector<Trade> recordedTrades2 = readAllTrades(test_filename_);
    EXPECT_EQ(recordedTrades2.size(), 2) << "Two trades logged";
    EXPECT_EQ(recordedTrades2[1].qty, 30) << "Second trade logged for 30 shares";
    EXPECT_EQ(book_->get_qty(50.0, Side::Sell), 10);
}
