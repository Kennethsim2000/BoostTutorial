// ============================================================================
// test_order_book_snapshot.cpp
// Tests for market depth snapshot functionality
// ============================================================================

#include <gtest/gtest.h>
#include "order_book.hpp"
#include "csv_logger.hpp"
#include "types.hpp"
#include <filesystem>
#include <sstream>

class SnapshotTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // TODO: Initialize logger and order book
        // Hint: Use filename "test_trades_snapshot.csv"
    }

    void TearDown() override
    {
        // TODO: Cleanup
    }

    // TODO: Declare logger_ and book_
};

TEST_F(SnapshotTest, SnapshotEmptyBook)
{
    // Test: Snapshot of empty order book
    // TODO: Call snapshot_top(5)
    // TODO: Verify result contains empty bids and asks arrays
    // Hint: Check for `"bids": []` and `"asks": []`
}

TEST_F(SnapshotTest, SnapshotSingleLevel)
{
    // Test: Snapshot with orders at single price level
    // TODO: Place multiple buy orders at $50 (e.g., 30, 40, 30 shares)
    // TODO: Call snapshot_top(1)
    // TODO: Verify snapshot shows aggregated quantity (100) at $50
}

TEST_F(SnapshotTest, SnapshotMultipleLevels)
{
    // Test: Snapshot showing multiple price levels
    // TODO: Place buy orders at $50, $49, $48
    // TODO: Place sell orders at $51, $52, $53
    // TODO: Call snapshot_top(3)
    // TODO: Verify bids are sorted high to low: $50, $49, $48
    // TODO: Verify asks are sorted low to high: $51, $52, $53
}

TEST_F(SnapshotTest, SnapshotDepthLimiting)
{
    // Test: Snapshot respects depth parameter
    // TODO: Place 5 levels of buy orders
    // TODO: Call snapshot_top(3)
    // TODO: Verify only top 3 levels are returned
}

TEST_F(SnapshotTest, SnapshotAggregation)
{
    // Test: Multiple orders at same price are aggregated
    // TODO: Place 4 buy orders at $50: 10, 20, 30, 40 shares
    // TODO: Place 3 sell orders at $51: 15, 25, 35 shares
    // TODO: Call snapshot_top(1)
    // TODO: Verify bid shows [50, 100]
    // TODO: Verify ask shows [51, 75]
}