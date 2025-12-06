
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
        std::map<double, std::deque<Order>> &
            book = ord.side == Side::Buy ? bids_ : asks_;
        std::deque<Order> &queue = book[ord.price];
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

    // TODO:
    // 1. While incoming.qty > 0:
    //      - Check best ask (asks_.begin()).
    //      - If price not matchable → break.
    //      - Pop from deque, match qty, create Trade, reduce qty.
    //      - record_trade(t);
    //      - Remove empty price levels & cleanup order_index_.
    // 2. Return trades.

    return trades;
}

// ------------------------------------------------------------
// match_sell (private)
// ------------------------------------------------------------
std::vector<Trade> OrderBook::match_sell(Order &incoming)
{
    std::vector<Trade> trades;

    // TODO:
    // Same logic as match_buy but using:
    //   - best bid = bids_.rbegin()
    //   - price condition reversed

    return trades;
}

// ------------------------------------------------------------
// cancel_order
// ------------------------------------------------------------
bool OrderBook::cancel_order(OrderId id)
{
    std::lock_guard<std::mutex> lock(mu_);

    // TODO:
    // 1. Look up index: order_index_[id]
    // 2. Go to (price, side) map → deque at that price.
    // 3. Locate element by id and erase it.
    // 4. If deque becomes empty, erase price level.
    // 5. Remove index entry.

    return false;
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
// best_bid
// ------------------------------------------------------------
std::optional<double> OrderBook::best_bid() const
{
    std::lock_guard<std::mutex> lock(mu_);

    // TODO: bids_.rbegin()->first if not empty
    return std::nullopt;
}

// ------------------------------------------------------------
// best_ask
// ------------------------------------------------------------
std::optional<double> OrderBook::best_ask() const
{
    std::lock_guard<std::mutex> lock(mu_);

    // TODO: asks_.begin()->first if not empty
    return std::nullopt;
}
