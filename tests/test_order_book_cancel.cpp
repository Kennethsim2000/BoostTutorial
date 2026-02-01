// ============================================================================
// test_order_book_cancel.cpp
// Tests for order cancellation functionality
// ============================================================================

#include <gtest/gtest.h>
#include "order_book.hpp"
#include "csv_logger.hpp"
#include "types.hpp"
#include <filesystem>
#include "test_helpers.hpp"

class OrderCancelTest : public ::testing::Test
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

    std::string test_filename_ = "test_trades_cancel.csv";
    std::unique_ptr<CSVLogger> logger_;
    std::unique_ptr<OrderBook> book_;
};

TEST_F(OrderCancelTest, CancelExistingOrder)
{
    const Order buy_order = createBuyOrder(50.00, 100);
    std::vector<Trade> trades = book_->place_order(buy_order);
    EXPECT_TRUE(book_->cancel_order(1));
    EXPECT_FALSE(book_->best_bid().has_value()) << "No buy_order should be in bids after cancel";
}

TEST_F(OrderCancelTest, CancelNonExistentOrder)
{
    // Test: Attempt to cancel an order that doesn't exist
    // TODO: Call cancel_order(99999) with invalid ID
    // TODO: Verify cancel returns false
}

TEST_F(OrderCancelTest, CancelPartiallyFilledOrder)
{
    // Test: Cancel an order that was partially filled
    // TODO: Place sell order for 100 @ $50
    // TODO: Place buy order for 60 @ $50 (partially fills sell order)
    // TODO: Cancel the remaining sell order (40 shares)
    // TODO: Verify cancellation succeeds
    // TODO: Verify best_ask() becomes nullopt
}

TEST_F(OrderCancelTest, CancelAfterFullMatch)
{
    // Test: Try to cancel an order that was fully matched
    // TODO: Place sell order, capture ID
    // TODO: Place matching buy order (fully executes sell)
    // TODO: Try to cancel the sell order ID
    // TODO: Verify cancel returns false (order no longer exists)
}
