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
        // TODO: Initialize a CSVLogger with a test file
        // TODO: Initialize an OrderBook instance
        // Hint: Use a temporary filename like "test_trades_basic.csv"
    }

    void TearDown() override
    {
        // TODO: Clean up test CSV files
        // Hint: Use std::filesystem::remove() to delete test files
    }

    // TODO: Declare CSVLogger* logger_;
    // TODO: Declare OrderBook* book_;
};

// ----------------------------------------------------------------------------
// Basic Order Placement Tests
// ----------------------------------------------------------------------------

TEST_F(OrderBookTest, PlaceSimpleBuyOrder)
{
    // Test: Placing a buy order with no matching sell orders
    // TODO: Create a buy order (e.g., Buy 100 shares @ $50.00)
    // TODO: Call book_->place_order(order)
    // TODO: Verify the returned trades vector is empty
    // TODO: Verify best_bid() returns the correct price
    // TODO: Verify best_ask() is nullopt (no sell orders)
}

TEST_F(OrderBookTest, PlaceSimpleSellOrder)
{
    // Test: Placing a sell order with no matching buy orders
    // TODO: Create a sell order (e.g., Sell 100 shares @ $51.00)
    // TODO: Call book_->place_order(order)
    // TODO: Verify the returned trades vector is empty
    // TODO: Verify best_ask() returns the correct price
    // TODO: Verify best_bid() is nullopt
}

// ----------------------------------------------------------------------------
// Order Matching Tests
// ----------------------------------------------------------------------------

TEST_F(OrderBookTest, FullMatchBuyOrder)
{
    // Test: Buy order completely matches with existing sell order
    // TODO: Place a sell order first (e.g., Sell 100 @ $50)
    // TODO: Place a buy order that matches (e.g., Buy 100 @ $50)
    // TODO: Verify exactly 1 trade was created
    // TODO: Verify trade quantity = 100
    // TODO: Verify trade price = $50
    // TODO: Verify both best_bid() and best_ask() are nullopt (book is empty)
}

TEST_F(OrderBookTest, PartialMatchBuyOrder)
{
    // Test: Buy order partially fills against smaller sell order
    // TODO: Place sell order (e.g., Sell 50 @ $50)
    // TODO: Place buy order (e.g., Buy 100 @ $50)
    // TODO: Verify 1 trade for quantity 50
    // TODO: Verify remaining 50 buy quantity stays in book
    // TODO: Verify best_bid() = $50, best_ask() = nullopt
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