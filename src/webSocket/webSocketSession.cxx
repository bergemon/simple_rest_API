#include "../../include/webSocket/webSocketSession.hpp"

WS_session::WebSockSession::WebSockSession(websocket::stream<beast::tcp_stream> webSock,
    std::unordered_set<WS_session::WebSockSession*>& state)
        : m_webSocket(std::move(webSock)), m_state(state)
{
    m_state.insert(this);
}

WS_session::WebSockSession::~WebSockSession() {
    m_state.erase(this);
}

websocket::stream<beast::tcp_stream>& WS_session::WebSockSession::getSock() {
    return m_webSocket;
}

void WS_session::WebSockSession::run() {
    // Set suggested timeout settings for the websocket
    m_webSocket.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

    // Set a decorator to change the Server of the handshake
    m_webSocket.set_option(websocket::stream_base::decorator([](websocket::response_type& res) {
        res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
    }));

    // Accept the websocket handshake
    m_webSocket.async_accept(beast::bind_front_handler(&WebSockSession::on_accept, shared_from_this()));
}

void WS_session::WebSockSession::on_accept(beast::error_code ec) {
    if (ec)
        return fail(ec, "accept handshake");

    // Read a message
    m_webSocket.async_read(m_buffer,
        beast::bind_front_handler(&WebSockSession::on_read, shared_from_this()));
}

void WS_session::WebSockSession::on_read(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    std::lock_guard guard(m_mutex);

    // This indicates that the session was closed
    if (ec == websocket::error::closed)
        return;

    if (ec)
        return fail(ec, "read");

    // Echo the message
    m_webSocket.text(m_webSocket.got_text());

    for (const auto& elem : m_state)
        elem->getSock().async_write(asio::buffer(beast::buffers_to_string(m_buffer.cdata())),
            beast::bind_front_handler(&WebSockSession::on_write, shared_from_this()));
}

void WS_session::WebSockSession::on_write(beast::error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    std::lock_guard guard(m_mutex);

    if (ec)
        return fail(ec, "write");

    // Clear the buffer
    m_buffer.clear();

    // Do another read
    m_webSocket.async_read(m_buffer,
        beast::bind_front_handler(&WebSockSession::on_read, shared_from_this()));
}

void WS_session::WebSockSession::on_close(beast::error_code ec) {
    if(ec)
        return fail(ec, "close");
}