#pragma once
#include "session.hpp"

namespace Server {
    class server : public std::enable_shared_from_this<server> {

        asio::io_context& m_context;
        tcp::acceptor m_acceptor;

    public:
        server(asio::io_context& context, tcp::endpoint endpoint)
            : m_context(context), m_acceptor(asio::make_strand(m_context)) {

            beast::error_code ec;

            m_acceptor.open(endpoint.protocol(), ec);
            if (ec) {
                fail(ec, "open");
                return;
            }

            m_acceptor.set_option(asio::socket_base::reuse_address(true), ec);
            if (ec) {
                fail(ec, "set_option");
                return;
            }

            m_acceptor.bind(endpoint, ec);
            if (ec) {
                fail(ec, "bind");
                return;
            }

            m_acceptor.listen(asio::socket_base::max_listen_connections, ec);
            if (ec) {
                fail(ec, "listen");
                return;
            }
        } // End of constructor

        void run() {
            do_accept();
        }

    private:
        void do_accept() {
            m_acceptor.async_accept(asio::make_strand(m_context),
                beast::bind_front_handler(&server::on_accept, shared_from_this()));
        }

        void on_accept(beast::error_code ec, tcp::socket socket) {
            if (ec) {
                fail(ec, "accept");
                return;
            }
            else
                std::make_shared<Session::session>(std::move(socket))->run();

            do_accept();
        }
    };
}