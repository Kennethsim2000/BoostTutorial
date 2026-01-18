// ============================================================================
// test_order_book_basic.cpp
// Basic unit tests for OrderBook functionality
// ============================================================================

#include <gtest/gtest.h>
#include "order_book.hpp"
#include "csv_logger.hpp"
#include "types.hpp"
#include <filesystem>
#include <numeric>

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
    Order sell_order = Order(1, "testclient", Side::Sell, 50.00, 50, 50, std::chrono::system_clock::now());
    Order buy_order = Order(2, "testclient2", Side::Buy, 50.00, 100, 100, std::chrono::system_clock::now());
    std::vector<Trade> sell_trades = book_->place_order(sell_order);
    std::vector<Trade> buy_trades = book_->place_order(buy_order);
    EXPECT_EQ(buy_trades.size(), 1);
    EXPECT_EQ(book_->best_bid(), 50);
    EXPECT_EQ(book_->best_ask(), std::nullopt);
    EXPECT_EQ(book_->get_qty(50.00, Side::Buy), 50);
}

TEST_F(OrderBookTest, MultiplePartialMatches)
{
    Order sell_order1 = Order(1, "testclient", Side::Sell, 50.00, 30, 30, std::chrono::system_clock::now());
    Order sell_order2 = Order(2, "testclient", Side::Sell, 50.00, 40, 40, std::chrono::system_clock::now());
    Order sell_order3 = Order(3, "testclient", Side::Sell, 50.00, 50, 50, std::chrono::system_clock::now());
    std::vector<Trade> sell_trade1 = book_->place_order(sell_order1);
    std::vector<Trade> sell_trade2 = book_->place_order(sell_order2);
    std::vector<Trade> sell_trade3 = book_->place_order(sell_order3);
    Order buy_order = Order(4, "testclient2", Side::Buy, 50.00, 100, 100, std::chrono::system_clock::now());
    std::vector<Trade> buy_trade = book_->place_order(buy_order);
    EXPECT_EQ(buy_trade.size(), 3);
    EXPECT_EQ(book_->get_qty(50.00, Side::Sell), 20);

    uint64_t total_qty_traded = std::accumulate(buy_trade.begin(), buy_trade.end(), uint64_t{0},
                                                [](auto sum, const Trade &t)
                                                {
                                                    return sum + t.qty;
                                                });
    EXPECT_EQ(total_qty_traded, 100);
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