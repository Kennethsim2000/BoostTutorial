#include "order_book.hpp"
#include "csv_logger.hpp"
#include "tcp_server.hpp"

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <iostream>

using boost::asio::ip::tcp;
using net::TCPServer;

int main()
{
    try
    {
        // -------------------------------
        // 1. Create CSV logger and OrderBook
        // -------------------------------
        CSVLogger logger("fills.csv");
        auto order_book = OrderBook(logger);

        std::cout << "[INFO] OrderBook and CSVLogger initialized" << std::endl;
        // -------------------------------
        // 2. Initialize Boost ASIO io_context
        // -------------------------------
        boost::asio::io_context io_context;

        // -------------------------------
        // 3.Create TCP server
        // -------------------------------
        TCPServer server(io_context, tcp::endpoint(tcp::v4(), 12345), order_book);
        server.run(); // schedules async_accept
        std::cout << "[INFO] TCP server is running on port 12345" << std::endl;
        // -------------------------------
        // 4. Thread pool for handling clients
        // -------------------------------
        const int num_threads = 4;
        std::vector<std::thread> threads;
        for (int i = 0; i < num_threads; i++)
        {
            threads.emplace_back([&io_context, i]()
                                 {
                                     std::cout << "[DEBUG] thread" << i << "starting io_context run" << std::endl;
                                     io_context.run();
                                     std::cout << "[DEBUG] thread" << i << "finished io_context run" << std::endl; });
        }
        // -------------------------------
        // 5. Main thread also runs io_context
        // -------------------------------
        std::cout << "[DEBUG] Main thread running io_context" << std::endl;
        io_context.run();
        std::cout << "[DEBUG] Main thread finished io_context" << std::endl;

        // -------------------------------
        // 6. Join all threads
        // -------------------------------
        for (auto &t : threads)
            t.join();
        std::cout << "[INFO] Server shutting down" << std::endl;

        // -------------------------------
        // TODO 6: Clean shutdown (optional)
        // -------------------------------
        // - Flush logger
        // - Stop io_context
        // - Print shutdown message
    }
    catch (std::exception &ex)
    {
        std::cerr << "[FATAL] Exception: " << ex.what() << std::endl;
    }

    return 0;
}
