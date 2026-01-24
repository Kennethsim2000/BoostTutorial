// ============================================================================
// test_helpers.cpp
// Helper functions for all tests
// ============================================================================

#include <gtest/gtest.h>
#include <fstream>
#include <sstream>
#include "test_helpers.hpp"

Order createBuyOrder(double price, uint64_t qty, const std::string &client)
{
    return Order(-1, client, Side::Buy, price, qty, qty, std::chrono::system_clock::now());
}

Order createSellOrder(double price, uint64_t qty, const std::string &client)
{
    return Order(-1, client, Side::Sell, price, qty, qty, std::chrono::system_clock::now());
}

void verifyTrade(const Trade &trade, OrderId expected_buy, OrderId expected_sell,
                 double expected_price, uint64_t expected_qty)
{
    EXPECT_EQ(trade.buy_order, expected_buy);
    EXPECT_EQ(trade.sell_order, expected_sell);
    EXPECT_EQ(trade.price, expected_price);
    EXPECT_EQ(trade.qty, expected_qty);
}

int countCSVLines(const std::string &filename)
{
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
Trade parseCSVLine(const std::string &line)
{
    std::istringstream sstream(line);
    std::string attribute;
    std::vector<std::string> vec;
    while (std::getline(sstream, attribute, ','))
    {
        vec.push_back(attribute);
    }
    if (vec.size() < 5)
    {
        throw new std::invalid_argument("Invalid CSV line: expected 5 fields");
    }
    long long ms = std::stoll(vec.at(0));
    std::chrono::milliseconds dur(ms);
    std::chrono::system_clock::time_point time = std::chrono::system_clock::time_point(dur);
    OrderId buy_order = stoull(vec.at(1));
    OrderId sell_order = stoull(vec.at(2));
    double price = stod(vec.at(3));
    uint64_t qty = stoull(vec.at(4));
    Trade t(buy_order, sell_order, price, qty, time);
    return t;
}

// Helper: Read entire CSV file into vector of trades
std::vector<Trade> readAllTrades(const std::string &filename)
{
    std::vector<Trade> trades;
    std::ifstream file(filename);
    std::string line;
    while (std::getline(file, line))
    {
        Trade t = parseCSVLine(line);
        trades.push_back(t);
    }
    return trades;
}