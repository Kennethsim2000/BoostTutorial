#pragma once
#include <cstdint>
#include <string>
#include <chrono>
#include <list>

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

    Order(OrderId id_, ClientId client_, Side side_, double price_, uint64_t qty_, uint64_t original_qty_, std::chrono::system_clock::time_point ts_)
        : id(id_), client(client_), side(side_), price(price_), qty(qty_), original_qty(original_qty_), ts(ts_) {}
};

struct Trade
{
    OrderId buy_order;
    OrderId sell_order;
    double price;
    uint64_t qty;
    std::chrono::system_clock::time_point ts;

    bool operator==(const Trade &other) const // Only ONE parameter
    {
        return buy_order == other.buy_order && sell_order == other.sell_order && price == other.price && qty == other.qty;
    }

    Trade(OrderId buy_order_, OrderId sell_order_, double price_, uint64_t qty_, std::chrono::system_clock::time_point ts_) : buy_order(buy_order_), sell_order(sell_order_), price(price_), qty(qty_), ts(ts_)
    {
    }
};

struct OrderRef
{
    Side side;
    double price;
    std::list<Order>::iterator it;
};