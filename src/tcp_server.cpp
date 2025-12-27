// tcp_server.cpp
#include "tcp_server.hpp"
#include <iostream>
#include <sstream>

namespace net
{

    TCPServer::TCPServer(boost::asio::io_context &ioc,
                         tcp::endpoint endpoint,
                         OrderBook &book)
        : ioc_(ioc),
          acceptor_(ioc, endpoint), // acceptor typically created using the io_context and tcp::endpoint
          book_(book)
    {
        std::cout << "TCP server created" << std::endl;
    }

    // ------------------------------------------------------------
    // Start accepting new connections
    // ------------------------------------------------------------
    void TCPServer::run()
    {
        do_accept();
    }

    // ------------------------------------------------------------
    // Accept loop
    // ------------------------------------------------------------
    void TCPServer::do_accept()
    {
        auto socket = tcp::socket(ioc_);
        // When a client connects, the acceptor.accept() (or async_accept()) function creates a new tcp::socket object to handle that specific connection.

        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) // Constructs a new tcp::socket internally and passes it by value to the completion handler.
            {
                if (!ec)
                {
                    auto session = std::make_shared<Session>(std::move(socket), book_);
                    session->start();
                }
                // Call do_accept again to continue listening for the next client
                do_accept();
            });
    }

    // ======================================================================
    // ============================= SESSION =================================
    // ======================================================================

    TCPServer::Session::Session(tcp::socket socket, OrderBook &book)
        : socket_(std::move(socket)),
          book_(book)
    {
    }

    // ------------------------------------------------------------
    // Session entry point
    // ------------------------------------------------------------
    void TCPServer::Session::start()
    {
        do_read();
        std::cout << "Client connected from: "
                  << socket_.remote_endpoint().address().to_string()
                  << ":" << socket_.remote_endpoint().port() << std::endl;
    }

    // ------------------------------------------------------------
    // Set up async read until newline
    // ------------------------------------------------------------
    void TCPServer::Session::do_read()
    {
        // Capturing self ensures the Session stays alive until handler completes
        async_read_until(socket_, buffer_, "\n",
                         [this, self = shared_from_this()](boost::system::error_code ec, size_t n)
                         {
                             on_read(ec, n);
                         });
    }

    // ------------------------------------------------------------
    // Handle a full incoming line
    // ------------------------------------------------------------
    void TCPServer::Session::on_read(boost::system::error_code ec,
                                     std::size_t bytes_transferred)
    {
        if (ec)
        {
            if (ec == boost::asio::error::eof)
            {
                if (socket_.is_open())
                    socket_.close();
            }
            else
            {
                std::cerr << "Read error: " << ec.message() << std::endl;
            }
            return;
        }
        std::istream is(&buffer_);
        std::string line;
        std::getline(is, line);
        process_line(line);
        do_read();
    }

    // ------------------------------------------------------------
    // Parse commands and call OrderBook
    // ------------------------------------------------------------
    void TCPServer::Session::process_line(const std::string &line)
    {
        std::vector<std::string> tokens;
        std::istringstream iss(line);
        std::string token;
        while (iss >> token)
        {
            tokens.push_back(token);
        }
        if (tokens.empty())
        {
            write_response("ERROR empty command\n");
        }
        std::string command = tokens.at(0);
        std::transform(command.begin(), command.end(), command.begin(), ::toupper);

        if (command == "ORDER")
        {
            if (tokens.size() != 5)
            {
                write_response("ERROR Invalid ORDER arguments\n");
                return;
            }
            std::string token_str = tokens.at(1);
            std::transform(token_str.begin(), token_str.end(), token_str.begin(), ::tolower);
            Side side;
            if (token_str == "buy")
            {
                side = Side::Buy;
            }
            else if (token_str == "sell")
            {
                side = Side::Sell;
            }
            else
            {
                write_response("ERROR Invalid side provided for ORDER command\n");
                return;
            }
            double price = stod(tokens.at(2));
            uint64_t qty = stoull(tokens.at(3));
            if (price < 0 || qty == 0)
            {
                write_response("ERROR Invalid price or quantity provided for ORDER command\n");
                return;
            }
            ClientId clientId = tokens.at(4);
            Order ord(-1, clientId, side, price, qty, qty, std::chrono::system_clock::now());
            auto trades = book_.place_order(ord);
            std::string trade_response = formatTrades(trades);
            write_response(trade_response);
        }
        else if (command == "CANCEL")
        {
            if (tokens.size() != 2)
            {
                write_response("ERROR invalid CANCEL arguments\n");
                return;
            }
            OrderId orderId = stoull(tokens.at(1));
            bool ok = book_.cancel_order(orderId);
            write_response(ok ? "CANCELLED\n" : "NOT_FOUND\n");
        }
        else if (command == "SNAPSHOT")
        {
            if (tokens.size() != 2)
            {
                write_response("ERROR invalid SNAPSHOT arguments\n");
                return;
            }
            size_t depth = stoull(tokens.at(1));
            auto json = book_.snapshot_top(depth);
            write_response(json + "\n");
        }
        else
        {
            write_response("ERROR unknown command\n");
            return;
        }
    }

    std::string formatTrades(const std::vector<Trade> &trades)
    {
        std::ostringstream oss;

        oss << "--- Trade List ---\n";
        for (size_t i = 0; i < trades.size(); ++i)
        {
            const auto &trade = trades[i];
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(trade.ts.time_since_epoch()).count();

            oss << "Trade " << i + 1 << ":\n";
            oss << "  BuyOrder: " << trade.buy_order << "\n";
            oss << "  SellOrder: " << trade.sell_order << "\n";
            oss << "  Price: $" << trade.price << "\n";
            oss << "  Quantity: " << trade.qty << "\n";
            oss << "  Timestamp: " << ms << "\n";
        }
        oss << "--- End of List ---\n";
        return oss.str();
    }

    // ------------------------------------------------------------
    // Send response back to client
    // ------------------------------------------------------------
    void TCPServer::Session::write_response(const std::string &resp)
    {
        auto out = std::make_shared<std::string>(resp);

        boost::asio::async_write(socket_, boost::asio::buffer(*out), [this, self = shared_from_this()](boost::system::error_code ec, size_t n)
                                 { 
                                    if(ec) {
                                        std::cerr << "Write error: " << ec.message() << std::endl;
                                        boost::system::error_code ignored;
                                        socket_.close(ignored);
                                    } });
    }

}
