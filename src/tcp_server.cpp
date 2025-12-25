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
    // TODO:
    // Call do_read();
    std::cout << "Client connected from: "
              << socket->remote_endpoint().address().to_string()
              << ":" << socket->remote_endpoint().port() << std::endl;
}

// ------------------------------------------------------------
// Set up async read until newline
// ------------------------------------------------------------
void TCPServer::Session::do_read()
{
    // TODO:
    // async_read_until(socket_, buffer_, '\n', callback)
    //
    // callback calls on_read(...)
}

// ------------------------------------------------------------
// Handle a full incoming line
// ------------------------------------------------------------
void TCPServer::Session::on_read(error_code ec,
                                 std::size_t bytes_transferred)
{
    // TODO:
    // if (ec) handle disconnect or error
    // else:
    //   extract line from buffer_:
    //     std::istream is(&buffer_);
    //     std::string line;
    //     std::getline(is, line);
    //
    //   process_line(line);
    //
    //   continue reading:
    //     do_read();
}

// ------------------------------------------------------------
// Parse commands and call OrderBook
// ------------------------------------------------------------
void TCPServer::Session::process_line(const std::string &line)
{
    // HINTS:
    // 1. Tokenize using std::istringstream iss(line)
    // 2. Read first word → command
    // 3. if ORDER:
    //       parse "side price qty client"
    //       build Order ord;
    //       auto trades = book_.place_order(...)
    //       format a response string summarizing trades
    //       write_response(resp);
    //
    //    if CANCEL:
    //       parse id
    //       bool ok = book_.cancel_order(id)
    //       write_response(ok?"CANCELLED\n":"NOT_FOUND\n");
    //
    //    if SNAPSHOT:
    //       parse depth
    //       auto json = book_.snapshot_top(depth)
    //       write_response(json + "\n");
    //
    //    else:
    //       write_response("ERROR unknown\n");
}

// ------------------------------------------------------------
// Send response back to client
// ------------------------------------------------------------
void TCPServer::Session::write_response(const std::string &resp)
{
    // TODO:
    // Use async_write(socket_, buffer, callback)
    //
    // IMPORTANT:
    // Response must live until async_write finishes.
    // So wrap in shared_ptr<string>:
    //
    // auto out = std::make_shared<std::string>(resp);
    // async_write(socket_, buffer(*out), [self,out](...) { ... });
}

} // namespace net
