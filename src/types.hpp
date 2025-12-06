#pragma once
#include <cstdint>
#include <string>
#include <chrono>

using OrderId = uint64_t;
using ClientId = std::string;

enum class Side
{
    Buy,
    Sell
};

struct Order
{
    OrderId id;
    ClientId client;
    Side side;
    double price;
    uint64_t qty;
    uint64_t original_qty;
    std::chrono::system_clock::time_point ts;
};

struct Trade
{
    OrderId buy_order;
    OrderId sell_order;
    double price;
    uint64_t qty;
    std::chrono::system_clock::time_point ts;
};

struct OrderRef
{
    Side side;
    double price;
    std::deque<Order>::iterator it;
};