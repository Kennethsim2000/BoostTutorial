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
#include "test_helpers.hpp"

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
    Order buy_order = createBuyOrder(50.00, 100);
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
    Order sell_order = createSellOrder(51.00, 100);
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
    Order sell_order = createSellOrder(50.0, 100);
    std::vector<Trade> trades_sell = book_->place_order(sell_order);
    Order buy_order = createBuyOrder(50.0, 100, "testclient2");
    std::vector<Trade> trades_buy = book_->place_order(buy_order);
    EXPECT_EQ(trades_buy.size(), 1);
    Trade trade = trades_buy.at(0);
    verifyTrade(trade, 2, 1, 50, 100);
    EXPECT_EQ(book_->best_bid(), std::nullopt);
    EXPECT_EQ(book_->best_ask(), std::nullopt);
}

TEST_F(OrderBookTest, PartialMatchBuyOrder)
{
    Order sell_order = createSellOrder(50.0, 50);
    Order buy_order = createBuyOrder(50.0, 100, "testclient2");
    std::vector<Trade> sell_trades = book_->place_order(sell_order);
    std::vector<Trade> buy_trades = book_->place_order(buy_order);
    EXPECT_EQ(buy_trades.size(), 1);
    EXPECT_EQ(book_->best_bid(), 50);
    EXPECT_EQ(book_->best_ask(), std::nullopt);
    EXPECT_EQ(book_->get_qty(50.00, Side::Buy), 50);
}

TEST_F(OrderBookTest, MultiplePartialMatches)
{
    Order sell_order1 = createSellOrder(50.0, 30);
    Order sell_order2 = createSellOrder(50.0, 40);
    Order sell_order3 = createSellOrder(50.0, 50);
    std::vector<Trade> sell_trade1 = book_->place_order(sell_order1);
    std::vector<Trade> sell_trade2 = book_->place_order(sell_order2);
    std::vector<Trade> sell_trade3 = book_->place_order(sell_order3);
    Order buy_order = createBuyOrder(50.0, 100, "testclient2");
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

TEST_F(OrderBookTest, PriceLevelMatching)
{
    Order sell_order1 = createSellOrder(50.0, 50);
    Order sell_order2 = createSellOrder(51.0, 50);
    Order sell_order3 = createSellOrder(52.0, 50);
    std::vector<Trade> sell_trade1 = book_->place_order(sell_order1);
    std::vector<Trade> sell_trade2 = book_->place_order(sell_order2);
    std::vector<Trade> sell_trade3 = book_->place_order(sell_order3);
    Order buy_order = createBuyOrder(52.0, 150, "testclient2");
    std::vector<Trade> buy_trade = book_->place_order(buy_order);
    EXPECT_EQ(buy_trade.size(), 3);
    EXPECT_EQ(buy_trade.at(0).price, 50.00);
    EXPECT_EQ(buy_trade.at(1).price, 51.00);
    EXPECT_EQ(buy_trade.at(2).price, 52.00);
}