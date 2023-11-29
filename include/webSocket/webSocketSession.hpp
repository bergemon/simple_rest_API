#pragma once
#include "../dependencies.hpp"
#include "../sharedFunctions.hpp"

namespace WS_session {
    class WebSockSession : public std::enable_shared_from_this<WebSockSession> {
        std::mutex m_mutex;
        beast::flat_buffer m_buffer;
        std::unordered_set<WS_session::WebSockSession*>& m_state;
        websocket::stream<beast::tcp_stream> m_webSocket;

    public:
        // Take ownership of the stream
        WebSockSession(websocket::stream<beast::tcp_stream> webSock,
            std::unordered_set<WS_session::WebSockSession*>& state);

        ~WebSockSession();

        websocket::stream<beast::tcp_stream>& getSock();
        // Start the asynchronous operation
        void run();

        void on_accept(beast::error_code ec);

        void on_read(beast::error_code ec, std::size_t bytes_transferred);

        void on_write(beast::error_code ec, std::size_t bytes_transferred);

        void on_close(beast::error_code ec);
    };
}