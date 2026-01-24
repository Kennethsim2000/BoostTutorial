// ============================================================================
// test_helpers.cpp
// Helper functions for all tests
// ============================================================================

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "test_helpers.hpp"

Order CreateBuyOrder(double price, uint64_t qty, const std::string &client)
{
    return Order(-1, client, Side::Buy, price, qty, qty, std::chrono::system_clock::now());
}

Order CreateSellOrder(double price, uint64_t qty, const std::string &client)
{
    return Order(-1, client, Side::Sell, price, qty, qty, std::chrono::system_clock::now());
}

// Helper: Verify trade matches expected values
void VerifyTrade(const Trade &trade, OrderId expected_buy, OrderId expected_sell,
                 double expected_price, uint64_t expected_qty)
{
    // TODO: Use EXPECT_EQ for each field
    // EXPECT_EQ(trade.buy_order, expected_buy);
    // EXPECT_EQ(trade.sell_order, expected_sell);
    // EXPECT_EQ(trade.price, expected_price);
    // EXPECT_EQ(trade.qty, expected_qty);
    EXPECT_EQ(trade.buy_order, expected_buy);
}

// Helper: Count lines in CSV file
int CountCSVLines(const std::string &filename)
{
    // TODO: Open file, count lines, return count
    std::ifstream file(filename);
    int count = 0;
    std::string line;
    while (std::getline(file, line))
    {
        count++;
    }
    return count;
}

// Helper: Parse CSV line into Trade struct
Trade ParseCSVLine(const std::string &line)
{
    // TODO: Split by comma, parse fields, return Trade
    // This is more complex - you'll need to parse:
    // timestamp,buy_order,sell_order,price,qty
    Trade t;
    // Implementation left for practice
    return t;
}

// Helper: Read entire CSV file into vector of trades
std::vector<Trade> ReadAllTrades(const std::string &filename)
{
    // TODO: Open file, skip header, parse each line
    std::vector<Trade> trades;
    // Implementation left for practice
    return trades;
}