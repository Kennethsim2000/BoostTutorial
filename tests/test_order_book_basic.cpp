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
        if (std::filesystem::exists(test_filename_))
        {
            std::filesystem::remove(test_filename_);
        }
        logger_ = std::make_unique<CSVLogger>(test_filename_); // logger_ owns the CSVLOGger object
        book_ = std::make_unique<OrderBook>(*logger_);         // takes a reference to the CSVLOgger- It does not take ownership
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
    std::string test_filename_ = "test_trades_basic.csv";
    std::unique_ptr<CSVLogger> logger_;
    std::unique_ptr<OrderBook> book_;
};

// ----------------------------------------------------------------------------
// Basic Order Placement Tests
// ----------------------------------------------------------------------------

TEST_F(OrderBookTest, PlaceSimpleBuyOrder)
{
    const Order buy_order = createBuyOrder(50.00, 100);
    std::vector<Trade> trades = book_->place_order(buy_order);

    EXPECT_TRUE(trades.empty()) << "No matching sell order, should not generate trades";

    const std::optional<double> best_bid = book_->best_bid();
    EXPECT_TRUE(best_bid.has_value()) << "Buy order should create a bid";
    EXPECT_DOUBLE_EQ(50.0, best_bid.value());

    EXPECT_FALSE(book_->best_ask().has_value()) << "No sell orders placed";
}

TEST_F(OrderBookTest, PlaceSimpleSellOrder)
{
    Order sell_order = createSellOrder(51.00, 100);
    std::vector<Trade> trades = book_->place_order(sell_order);

    EXPECT_TRUE(trades.empty()) << "No matching buy order, should not generate trades";

    std::optional<double> best_ask = book_->best_ask();
    EXPECT_TRUE(best_ask.has_value()) << "Sell order should create an ask";
    EXPECT_DOUBLE_EQ(51.0, best_ask.value());

    EXPECT_FALSE(book_->best_bid().has_value()) << "No buy orders placed";
}

// ----------------------------------------------------------------------------
// Order Matching Tests
// ----------------------------------------------------------------------------

TEST_F(OrderBookTest, FullMatchBuyOrder)
{
    const Order sell_order = createSellOrder(50.0, 100);
    std::vector<Trade> trades_sell = book_->place_order(sell_order);
    EXPECT_TRUE(trades_sell.empty()) << "FIrst order shouldn't match";

    const Order buy_order = createBuyOrder(50.0, 100, "testclient2");
    std::vector<Trade> trades_buy = book_->place_order(buy_order);
    EXPECT_EQ(trades_buy.size(), 1) << "Should generate exactly one trade";

    const Trade &trade = trades_buy.at(0);
    verifyTrade(trade, 2, 1, 50, 100);

    EXPECT_FALSE(book_->best_bid().has_value()) << "All buy orders matched";
    EXPECT_FALSE(book_->best_ask().has_value()) << "All sell orders matched";
}

TEST_F(OrderBookTest, PartialMatchBuyOrder)
{
    Order sell_order = createSellOrder(50.0, 50);
    Order buy_order = createBuyOrder(50.0, 100, "testclient2");

    std::vector<Trade> sell_trades = book_->place_order(sell_order);
    EXPECT_TRUE(sell_trades.empty());

    std::vector<Trade> buy_trades = book_->place_order(buy_order);
    EXPECT_EQ(buy_trades.size(), 1) << "Should partially match";

    const auto best_bid = book_->best_bid();
    ASSERT_TRUE(best_bid.has_value()) << "Remaining buy order should stay on book";
    EXPECT_DOUBLE_EQ(50.0, *best_bid);

    EXPECT_FALSE(book_->best_ask().has_value()) << "Sell order fully consumed";
    EXPECT_EQ(book_->get_qty(50.00, Side::Buy), 50);
}

TEST_F(OrderBookTest, MultiplePartialMatches)
{
    const std::vector<Order> sell_orders = {
        createSellOrder(50.0, 30),
        createSellOrder(50.0, 40),
        createSellOrder(50.0, 50)};

    for (const auto &order : sell_orders)
    {
        std::vector<Trade> trades = book_->place_order(order);
        EXPECT_TRUE(trades.empty()) << "Sell orders shouldn't match each other";
    }

    const Order buy_order = createBuyOrder(50.0, 100, "testclient2");
    std::vector<Trade> buy_trade = book_->place_order(buy_order);

    EXPECT_EQ(buy_trade.size(), 3) << "Should match against 3 sell orders";
    EXPECT_EQ(30, buy_trade[0].qty);
    EXPECT_EQ(40, buy_trade[1].qty);
    EXPECT_EQ(30, buy_trade[2].qty);

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
    const std::vector<Order> sell_orders = {
        createSellOrder(50.0, 50),
        createSellOrder(51.0, 50),
        createSellOrder(52.0, 50)};
    for (const auto &order : sell_orders)
    {
        book_->place_order(order);
    }

    Order buy_order = createBuyOrder(52.0, 150, "testclient2");
    std::vector<Trade> buy_trade = book_->place_order(buy_order);

    EXPECT_EQ(buy_trade.size(), 3) << "Should match at 3 price levels";
    EXPECT_EQ(buy_trade.at(0).price, 50.00) << "Should match best ask first";
    EXPECT_EQ(buy_trade.at(1).price, 51.00);
    EXPECT_EQ(buy_trade.at(2).price, 52.00);

    for (const auto &trade : buy_trades)
    {
        EXPECT_EQ(50, trade.qty);
    }
}