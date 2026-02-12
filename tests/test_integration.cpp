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
    // Test: Complete flow from order placement to CSV logging
    // TODO: Place matching buy and sell orders
    // TODO: Verify trade is created in return value
    // TODO: Read CSV file
    // TODO: Verify trade is logged with correct details
    const Order sell_order = createSellOrder(50.0, 100);
    std::vector<Trade> trades_sell = book_->place_order(sell_order);

    const Order buy_order = createBuyOrder(50.0, 100, "testclient2");
    std::vector<Trade> trades_buy = book_->place_order(buy_order);
    EXPECT_EQ(trades_buy.size(), 1) << "Should generate exactly one trade";
    int rows = countCSVLines(test_filename_);
    EXPECT_EQ(rows, 2) << "Should log one trade in CSV";
    std::vector<Trade> recordedTrades = readAllTrades(test_filename_);
    EXPECT_EQ(recordedTrades[0], trades_buy[0]) << "Trade recorded should be equal to trade returned to client";
}

TEST_F(IntegrationTest, MarketOrderSimulation)
{
    // Test: Simulate market orders (willing to match any price)
    // TODO: Place sell orders at $50, $51, $52 (100 each)
    // TODO: Place "market buy" with very high limit price @ $100 for 250 shares
    // TODO: Verify it sweeps through all three levels
    // TODO: Verify 3 trades created at correct prices
    // TODO: Verify all trades logged to CSV
    const Order sell_order1 = createSellOrder(50.0, 100);
    const Order sell_order2 = createSellOrder(51.0, 100);
    const Order sell_order3 = createSellOrder(52.0, 100);
    std::vector<Trade> trades_sell = book_->place_order(sell_order1);
    std::vector<Trade> trades_sell2 = book_->place_order(sell_order2);
    std::vector<Trade> trades_sell3 = book_->place_order(sell_order3);

    const Order buy_order = createBuyOrder(100.0, 250, "testclient2");
    std::vector<Trade> trades_buy = book_->place_order(buy_order);
    EXPECT_EQ(trades_buy.size(), 3) << "Should generate three trades";
    int rows = countCSVLines(test_filename_);
    EXPECT_EQ(rows, 4) << "Should log three trade in CSV";
}

TEST_F(IntegrationTest, PartialFillsAndLogging)
{
    // Test: Verify partial fills are logged correctly
    // TODO: Place sell order for 100 @ $50
    // TODO: Place buy order for 60 @ $50
    // TODO: Verify 1 trade logged for 60 shares
    // TODO: Place another buy for 30 @ $50
    // TODO: Verify another trade logged for 30 shares
    // TODO: Verify CSV has 2 separate trade entries
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
}
