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
    const Order buy_order = createBuyOrder(50.00, 100);
    std::vector<Trade> trades = book_->place_order(buy_order);
    EXPECT_FALSE(book_->cancel_order(2)) << "Order book cannot cancel an orderId that does not exist";
    EXPECT_TRUE(book_->best_bid().has_value()) << "No buy_order should be in bids after cancel";
}

TEST_F(OrderCancelTest, CancelPartiallyFilledOrder)
{
    const Order sell_order = createSellOrder(50.0, 100, "TestClient2");
    std::vector<Trade> trades_sell = book_->place_order(sell_order);
    const Order buy_order = createBuyOrder(50.00, 60);
    std::vector<Trade> trades = book_->place_order(buy_order);
    EXPECT_TRUE(book_->cancel_order(1)) << "Partially filled sell order can be cancelled";
    EXPECT_FALSE(book_->best_ask().has_value()) << "No sell order should be in bids after cancel";
}

TEST_F(OrderCancelTest, CancelAfterFullMatch)
{
    const Order sell_order = createSellOrder(50.0, 100, "TestClient2");
    std::vector<Trade> trades_sell = book_->place_order(sell_order);
    const Order buy_order = createBuyOrder(50.00, 100);
    std::vector<Trade> trades = book_->place_order(buy_order);
    EXPECT_FALSE(book_->cancel_order(1)) << "Unable to cancel fully filled order";
}
