#include "order_book.hpp"
#include "csv_logger.hpp"
#include "tcp_server.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <thread>

using boost::asio::ip::tcp;
using net::TCPServer;

int main()
{
    // -------------------------------
    // Create OrderBook object
    // -------------------------------
    CSVLogger logger("fills.csv");
    auto order_book = OrderBook(logger);
    boost::asio::io_context io;

    // -------------------------------
    // Create TCP server
    // -------------------------------
    TCPServer server(io, tcp::endpoint(tcp::v4(), 12345), order_book);

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
    server.run();

    // -------------------------------
    // TODO 6: Clean shutdown (optional)
    // -------------------------------
    // - Flush logger
    // - Stop io_context
    // - Print shutdown message

    return 0;
}
