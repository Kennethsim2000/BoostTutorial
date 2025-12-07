#pragma once

#include <map>
#include <deque>
#include <unordered_map>
#include <mutex>
#include <vector>
#include <string>

#include "types.hpp"
#include "csv_logger.hpp"
#include <list>

class OrderBook
{
public:
    // Construct with a reference to a CSVLogger (non-owning).
    // The caller is responsible for keeping the logger alive while OrderBook is used.
    explicit OrderBook(CSVLogger &logger);

    // Place an order into the book. The order may execute immediately (partial/full)
    // against resting orders on the opposite side. Returns the list of executed trades.
    std::vector<Trade> place_order(Order ord);

    // Cancel an existing order by id. Returns true if the order was found and removed.
    bool cancel_order(OrderId id);

    // Return a small JSON-ish snapshot of the top `depth` price levels for debugging/REST.
    std::string snapshot_top(size_t depth = 5) const;

    // Optional: expose best bid/ask (price) if needed by UI/tests. Returns std::nullopt if none.
    std::optional<double> best_bid() const;
    std::optional<double> best_ask() const;

private:
    // Helper matching functions (internal). They mutate the incoming Order and
    // generate trades which are returned to the caller.
    std::vector<Trade> match_buy(Order &incoming);
    std::vector<Trade> match_sell(Order &incoming);

    // Internal data structures:
    // - bids_: map keyed by price ascending; iterate rbegin() for best bid
    // - asks_: map keyed by price ascending; begin() is best ask
    std::map<double, std::list<Order>> bids_;
    std::map<double, std::list<Order>> asks_;

    // index to locate an order quickly for cancellation:
    // order_id -> (price, side)
    std::unordered_map<OrderId, OrderRef> order_index_;

    // mutex protecting all mutable state above
    mutable std::mutex mu_;

    // Logger (non-owning reference)
    CSVLogger &logger_;

    // Optional: internal id generator (if you want the OrderBook to assign ids)
    std::atomic<OrderId> next_order_id_{1};

    // Internal helper: record trade (calls logger_)
    void record_trade(const Trade &t);
};
