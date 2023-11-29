#pragma once
#include "webSocketSession.hpp"

namespace WS_listener {
    class WebSockListener : public std::enable_shared_from_this<WebSockListener> {
        asio::io_context& m_context;
        tcp::acceptor m_acceptor;
        std::unordered_set<WS_session::WebSockSession*> m_state;

    public:
        WebSockListener(asio::io_context& context, tcp::endpoint endpoint)
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
        }

        void run() {
            do_accept();
        }

    private:
        void do_accept() {
            // The new connection gets its own strand
            m_acceptor.async_accept(asio::make_strand(m_context),
                beast::bind_front_handler(&WebSockListener::on_accept, shared_from_this()));
        }

        void on_accept(beast::error_code ec, tcp::socket socket) {
            if (ec)
                return fail(ec, "accept");

            std::make_shared<WS_session::WebSockSession>(std::move(websocket::stream<beast::tcp_stream>(std::move(socket))),
                m_state)->run();

            do_accept();
        }
    };
}