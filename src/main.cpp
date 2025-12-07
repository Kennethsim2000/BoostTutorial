#include "order_book.hpp"
#include "csv_logger.hpp"
// #include "tcp_server.hpp"   // <-- you'll include this once you create tcp_server.cpp

#include <boost/asio.hpp>
#include <memory>
#include <thread>

int main()
{
    // -------------------------------
    // TODO 1: Create OrderBook object
    // -------------------------------
    // - Stack or heap? Up to you.
    // - If heap, use std::make_shared<OrderBook>() so tcp_server can share it.
    //
    // Example:
    // auto order_book = std::make_shared<OrderBook>();

    // -------------------------------
    // TODO 2: Create CSVLogger object
    // -------------------------------
    // - You need the file path, e.g. "fills.csv".
    // - The OrderBook will call logger->log_fill(...) when trades occur.
    //
    // Example:
    // CSVLogger logger("fills.csv");

    // -------------------------------
    // TODO 3: Initialize Boost ASIO
    // -------------------------------
    // - Create io_context.
    // - You may also want a thread pool (2–4 threads).
    //
    // Example:
    // boost::asio::io_context io;

    // -------------------------------
    // TODO 4: Create TCP server
    // -------------------------------
    // - Pass:
    //   (1) io_context
    //   (2) shared OrderBook*
    //   (3) pointer/reference to CSVLogger
    //
    // Example:
    // TCPServer server(io, order_book, &logger);
    // server.start_accept();

    // -------------------------------
    // TODO 5: Run io_context
    // -------------------------------
    // - Run in main thread, OR
    // - Start thread pool:
    //
    // Example for thread pool:
    //
    // std::vector<std::thread> threads;
    // for (int i = 0; i < 4; i++) {
    //     threads.emplace_back([&io]() { io.run(); });
    // }
    //
    // for (auto &t : threads) t.join();
    //
    // If single-threaded:
    // io.run();

    // -------------------------------
    // TODO 6: Clean shutdown (optional)
    // -------------------------------
    // - Flush logger
    // - Stop io_context
    // - Print shutdown message

    return 0;
}
