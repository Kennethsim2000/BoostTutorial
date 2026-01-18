// ============================================================================
// test_order_book_basic.cpp
// Basic unit tests for OrderBook functionality
// ============================================================================

#include <gtest/gtest.h>
#include "order_book.hpp"
#include "csv_logger.hpp"
#include "types.hpp"
#include <filesystem>

// ----------------------------------------------------------------------------
// Test Fixture for OrderBook
// ----------------------------------------------------------------------------
class OrderBookTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        logger_ = new CSVLogger{"test_trades_basic.csv"};
        book_ = new OrderBook{*logger_};
    }

    void TearDown() override
    {
        delete book_;
        delete logger_;
        std::filesystem::remove("test_trades_basic.csv");
    }
    CSVLogger *logger_;
    OrderBook *book_;
};

// ----------------------------------------------------------------------------
// Basic Order Placement Tests
// ----------------------------------------------------------------------------

TEST_F(OrderBookTest, PlaceSimpleBuyOrder)
{
    Order buy_order = Order(1, "testclient", Side::Buy, 50.00, 100, 100, std::chrono::system_clock::now());
    std::vector<Trade> trades = book_->place_order(buy_order);
    EXPECT_TRUE(trades.empty());
    std::optional<double> best_bid = book_->best_bid();
    EXPECT_TRUE(best_bid.has_value());
    EXPECT_EQ(50.0, best_bid.value());
    std::optional<double> best_ask = book_->best_ask();
    EXPECT_EQ(std::nullopt, best_ask);
}

TEST_F(OrderBookTest, PlaceSimpleSellOrder)
{
    Order sell_order = Order(1, "testclient", Side::Sell, 51.00, 100, 100, std::chrono::system_clock::now());
    std::vector<Trade> trades = book_->place_order(sell_order);
    EXPECT_TRUE(trades.empty());
    std::optional<double> best_bid = book_->best_bid();
    std::optional<double> best_ask = book_->best_ask();
    EXPECT_TRUE(best_ask.has_value());
    EXPECT_EQ(51.0, best_ask.value());
    EXPECT_EQ(std::nullopt, best_bid);
}

// ----------------------------------------------------------------------------
// Order Matching Tests
// ----------------------------------------------------------------------------

TEST_F(OrderBookTest, FullMatchBuyOrder)
{
    Order sell_order = Order(1, "testclient", Side::Sell, 50.00, 100, 100, std::chrono::system_clock::now());
    std::vector<Trade> trades_sell = book_->place_order(sell_order);
    Order buy_order = Order(2, "testclient2", Side::Buy, 50.00, 100, 100, std::chrono::system_clock::now());
    std::vector<Trade> trades_buy = book_->place_order(buy_order);
    EXPECT_EQ(trades_buy.size(), 1);
    Trade trade = trades_buy.at(0);
    EXPECT_EQ(trade.qty, 100);
    EXPECT_EQ(trade.price, 50);
    EXPECT_EQ(book_->best_bid(), std::nullopt);
    EXPECT_EQ(book_->best_ask(), std::nullopt);
}

TEST_F(OrderBookTest, PartialMatchBuyOrder)
{
    Order sell_order = Order(1, "testclient", Side::Sell, 50.00, 50, 100, std::chrono::system_clock::now());
    Order buy_order = Order(2, "testclient2", Side::Buy, 50.00, 100, 100, std::chrono::system_clock::now());
    std::vector<Trade> sell_trades = book_->place_order(sell_order);
    std::vector<Trade> buy_trades = book_->place_order(buy_order);
    EXPECT_EQ(buy_trades.size(), 1);
    EXPECT_EQ(book_->best_bid(), 50);
    EXPECT_EQ(book_->best_ask(), std::nullopt);

    // TODO: Verify remaining 50 buy quantity stays in book
}

TEST_F(OrderBookTest, MultiplePartialMatches)
{
    // Test: One order matches against multiple orders at same price
    // TODO: Place 3 sell orders at $50 (e.g., 30, 40, 50 shares)
    // TODO: Place 1 buy order for 100 shares @ $50
    // TODO: Verify 3 trades were created
    // TODO: Verify total traded quantity = 100
    // TODO: Verify remaining 20 shares stay as sell order
}

TEST_F(OrderBookTest, PriceTimePriority)
{
    // Test: Orders at same price execute in FIFO order
    // TODO: Place sell order 1: Sell 50 @ $50, client "Alice"
    // TODO: Place sell order 2: Sell 50 @ $50, client "Bob"
    // TODO: Place buy order: Buy 60 @ $50
    // TODO: Verify Alice's order filled completely (50 shares)
    // TODO: Verify Bob's order filled partially (10 shares)
    // TODO: Verify Bob has 40 shares remaining in book
}

TEST_F(OrderBookTest, PriceLevelMatching)
{
    // Test: Orders match across multiple price levels
    // TODO: Place sell orders at different prices: $50, $51, $52
    // TODO: Place buy order @ $52 (willing to pay up to $52)
    // TODO: Verify it matches with $50 first, then $51, etc.
    // TODO: Verify trades execute at the maker's price (sell prices)
}