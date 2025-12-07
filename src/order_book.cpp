
#include "order_book.hpp"
#include <sstream>
#include <chrono>
#include "types.hpp"

OrderBook::OrderBook(CSVLogger &logger)
    : logger_(logger)
{
}

void OrderBook::record_trade(const Trade &t)
{
    logger_.log_trade(t);
}

// ------------------------------------------------------------
// place_order
// ------------------------------------------------------------
std::vector<Trade> OrderBook::place_order(Order ord)
{
    std::lock_guard<std::mutex> lock(mu_);
    OrderId order_id = next_order_id_.fetch_add(1);
    ord.id = order_id;
    ord.ts = std::chrono::system_clock::now();
    std::vector<Trade> fulfilled_trades;
    if (ord.side == Side::Buy)
    {
        fulfilled_trades = match_buy(ord);
    }
    else
    {
        fulfilled_trades = match_sell(ord);
    }
    if (ord.qty > 0)
    {
        std::map<double, std::list<Order>> &
            book = ord.side == Side::Buy ? bids_ : asks_;
        std::list<Order> &queue = book[ord.price];
        queue.push_back(ord);
        auto it = std::prev(queue.end());
        order_index_[order_id] = OrderRef{ord.side, ord.price, it};
    }

    return fulfilled_trades;
}

// ------------------------------------------------------------
// match_buy (private)
// ------------------------------------------------------------
std::vector<Trade> OrderBook::match_buy(Order &incoming)
{
    std::vector<Trade> trades;
    while (incoming.qty > 0)
    {
        auto best_ask_price = best_ask();
        if (!best_ask_price.has_value())
        { // no liquidity
            break;
        }
        if (incoming.price < best_ask_price.value())
        { // price cannot match
            break;
        }

        // there is a match, fill all orders in book
        auto &queue = asks_[best_ask_price.value()];
        while (!queue.empty() && incoming.qty > 0)
        { // match the current order with the best price orders
            Order &ask_order = queue.front();
            uint64_t fulfilled_qty = std::min(incoming.qty, ask_order.qty);
            ask_order.qty -= fulfilled_qty;
            incoming.qty -= fulfilled_qty;
            Trade trade{incoming.id, ask_order.id, best_ask_price.value(), fulfilled_qty, std::chrono::system_clock::now()};
            trades.push_back(trade);
            record_trade(trade);
            if (ask_order.qty == 0)
            { // filled already
                queue.pop_front();
                order_index_.erase(ask_order.id);
            }
        }
        if (queue.empty())
        {
            asks_.erase(best_ask_price.value());
        }
    }

    return trades;
}

// ------------------------------------------------------------
// match_sell (private)
// ------------------------------------------------------------
std::vector<Trade> OrderBook::match_sell(Order &incoming)
{
    // TODO:
    // Same logic as match_buy but using:
    //   - best bid = bids_.rbegin()
    //   - price condition reversed

    std::vector<Trade> trades;
    while (incoming.qty > 0)
    {
        auto best_bid_price = best_bid();
        if (!best_bid_price.has_value())
        { // no liquidity
            break;
        }
        if (incoming.price > best_bid_price.value())
        { // price cannot match
            break;
        }

        // there is a match, fill all orders in book
        auto &queue = bids_[best_bid_price.value()];
        while (!queue.empty() && incoming.qty > 0)
        { // match the current order with the best price orders
            Order &bid_order = queue.front();
            uint64_t fulfilled_qty = std::min(incoming.qty, bid_order.qty);
            bid_order.qty -= fulfilled_qty;
            incoming.qty -= fulfilled_qty;
            Trade trade{bid_order.id, incoming.id, best_bid_price.value(), fulfilled_qty, std::chrono::system_clock::now()};
            trades.push_back(trade);
            record_trade(trade);
            if (bid_order.qty == 0)
            { // filled already
                queue.pop_front();
                order_index_.erase(bid_order.id);
            }
        }
        if (queue.empty())
        {
            bids_.erase(best_bid_price.value());
        }
    }

    return trades;
}

// ------------------------------------------------------------
// cancel_order
// ------------------------------------------------------------
bool OrderBook::cancel_order(OrderId id)
{
    std::lock_guard<std::mutex> lock(mu_);
    auto it = order_index_.find(id);
    if (it == order_index_.end())
    {
        return false;
    }
    OrderRef &order_ref = it->second;
    if (order_ref.side == Side::Buy)
    {
        auto map_it = bids_.find(order_ref.price);
        if (map_it == bids_.end())
        {
            return false;
        }
        auto &queue = map_it->second;
        queue.erase(order_ref.it);
        if (queue.empty())
        {
            bids_.erase(map_it);
        }
    }
    else
    {
        auto map_it = asks_.find(order_ref.price);
        if (map_it == asks_.end())
        {
            return false;
        }
        auto &queue = map_it->second;
        queue.erase(order_ref.it);
        if (queue.empty())
        {
            asks_.erase(map_it);
        }
    }
    order_index_.erase(it);
    return true;
}

// ------------------------------------------------------------
// snapshot_top
// ------------------------------------------------------------
std::string OrderBook::snapshot_top(size_t depth) const
{
    std::lock_guard<std::mutex> lock(mu_);

    // TODO:
    // Build a simple JSON-like string manually.
    // Something like:
    //
    // {
    //   "bids": [ ["price", "qty"], ... ],
    //   "asks": [ ["price", "qty"], ... ]
    // }

    std::ostringstream ss;
    ss << "{ \"bids\": [], \"asks\": [] }";
    return ss.str();
}

// ------------------------------------------------------------
// best_bid, we want to return the largest bid price
// ------------------------------------------------------------
std::optional<double> OrderBook::best_bid() const
{
    std::lock_guard<std::mutex> lock(mu_);
    if (!bids_.empty())
    {
        return bids_.rbegin()->first;
    }
    return std::nullopt;
}

// ------------------------------------------------------------
// best_ask, we want to return the lowest asking price
// ------------------------------------------------------------
std::optional<double> OrderBook::best_ask() const
{
    std::lock_guard<std::mutex> lock(mu_);
    if (!asks_.empty())
    {
        return asks_.begin()->first;
    }
    return std::nullopt;
}
