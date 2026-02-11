// ============================================================================
// test_performance.cpp
// Performance tests - testing system under load
// ============================================================================

#include <gtest/gtest.h>
#include "order_book.hpp"
#include "csv_logger.hpp"
#include "types.hpp"
#include <chrono>
#include <filesystem>
#include "test_helpers.hpp"

class PerformanceTest : public ::testing::Test
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

    std::string test_filename_ = "test_trades_perf.csv";
    std::unique_ptr<CSVLogger> logger_;
    std::unique_ptr<OrderBook> book_;
};

TEST_F(PerformanceTest, HighVolumeOrders)
{
    constexpr int NUM_ORDERS = 10000;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_ORDERS; i++)
    {
        Side side = (i % 2 == 0) ? Side::Buy : Side::Sell;
        const Order order = side == Side::Buy ? createBuyOrder(50.00, 100) : createSellOrder(50.0, 100, "TestClient2");
        std::vector<Trade> trades = book_->place_order(order);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double seconds = duration.count() / 1000000.0;
    double orders_per_sec = NUM_ORDERS / seconds;
    EXPECT_GT(orders_per_sec, 10000);
}

TEST_F(PerformanceTest, ManySmallTrades)
{
    // Test: Many small trades (worst case for matching)
    // TODO: Place 1000 sell orders of 1 share each at $50
    // TODO: Place 1 buy order for 1000 shares at $50
    // TODO: Measure time to match all
    // TODO: Verify acceptable performance
    constexpr int NUM_ORDERS = 10000;
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < NUM_ORDERS; i++)
    {
        const Order order = createSellOrder(50.0, 1);
        std::vector<Trade> trades = book_->place_order(order);
    }
    const Order order = createBuyOrder(50.0, 1000, "client2");
    std::vector<Trade> trades = book_->place_order(order);
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double seconds = duration.count() / 1000000.0;
    double orders_per_sec = NUM_ORDERS / seconds;
    EXPECT_GT(orders_per_sec, 10000);
}

TEST_F(PerformanceTest, SnapshotPerformance)
{
    // Test: Snapshot generation with full book
    // TODO: Build order book with 100 bid levels and 100 ask levels
    // TODO: Measure time to generate snapshot_top(50) 1000 times
    // TODO: Calculate average time per snapshot
    // TODO: Verify acceptable performance
    constexpr int NUM_LEVELS = 100;
    double start_sell_price = 0;
    double start_buy_price = 200;
    constexpr double SNAPSHOT_TIMES = 1000;
    for (int i = 0; i < NUM_LEVELS; i++)
    {
        const Order order = createSellOrder(start_sell_price, 1);
        start_sell_price++;
        std::vector<Trade> trades = book_->place_order(order);
    }
    for (int i = 0; i < NUM_LEVELS; i++)
    {
        const Order order = createBuyOrder(start_buy_price, 1);
        start_buy_price++;
        std::vector<Trade> trades = book_->place_order(order);
    }
    auto start = std::chrono::steady_clock::now();
    for (int i = 0; i < SNAPSHOT_TIMES; i++)
    {
        book_->snapshot_top(50);
    }
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    double seconds = duration.count() / 1000000.0;
    double snapshots_per_sc = SNAPSHOT_TIMES / seconds;
    EXPECT_GT(snapshots_per_sc, 10000);
}
