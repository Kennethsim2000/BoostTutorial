#pragma once

// Protocol (text):
//   ORDER <buy|sell> <price> <qty> <client>
//   CANCEL <order_id>
//   SNAPSHOT <depth>

#include <boost/asio.hpp>
#include <memory>
#include <string>
#include "order_book.hpp"

namespace net
{

    using boost::asio::ip::tcp;
    using asio = boost::asio;

    class TCPServer : public std::enable_shared_from_this<TCPServer>
    {
    public:
        TCPServer(asio::io_context &ioc, tcp::endpoint endpoint, OrderBook &book);

        // Start accepting connections
        void run();

    private:
        void do_accept();

        // inner per-connection session
        struct Session : public std::enable_shared_from_this<Session>
        {
            Session(tcp::socket socket, OrderBook &book);
            void start();

        private:
            void do_read();
            void on_read(boost::system::error_code ec, std::size_t bytes_transferred);
            void process_line(const std::string &line);
            void write_response(const std::string &resp);

            tcp::socket socket_;
            boost::asio::streambuf buffer_;
            OrderBook &book_;
        };

        asio::io_context &ioc_;
        tcp::acceptor acceptor_;
        OrderBook &book_;
    };

} // namespace net
