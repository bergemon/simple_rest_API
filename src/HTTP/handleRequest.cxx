#include "../../include/HTTP/handleRequest.hpp"

// Returns a bad request response
http::message_generator CreateResponse::bad_request(const uint32_t version, const std::string why,
    const std::string target, const bool keep_alive)
{
    http::response<http::string_body> res{ http::status::bad_request, version };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(keep_alive);
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
}

// Returns wrong method response
http::message_generator CreateResponse::method_not_allowed(const uint32_t version, const std::string why,
    const std::string target, const bool keep_alive)
{
    http::response<http::string_body> res{ http::status::method_not_allowed, version };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(keep_alive);
    res.body() = std::string(why);
    res.prepare_payload();
    return res;
}

// Returns a not found response
http::message_generator CreateResponse::not_found(const uint32_t version, const std::string why,
    const std::string target, const bool keep_alive)
{
    http::response<http::string_body> res{ http::status::not_found, version };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(keep_alive);
    res.body() = "Error 404. Target '" + std::string(target) + "' does not exist.";
    res.prepare_payload();
    return res;
}

// Returns a permission denied response
http::message_generator CreateResponse::forbidden(const uint32_t version, const std::string why,
    const std::string target, const bool keep_alive)
{
    http::response<http::string_body> res{ http::status::forbidden, version };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(keep_alive);
    res.body() = "Error 403. '" + std::string(why) + "'";
    res.prepare_payload();
    return res;
}

// Returns a server error response
http::message_generator CreateResponse::server_error(const uint32_t version, const std::string why,
    const std::string target, const bool keep_alive)
{
    http::response<http::string_body> res{ http::status::internal_server_error, version };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::access_control_allow_origin, "*");
    res.set(http::field::content_type, "text/html");
    res.keep_alive(keep_alive);
    res.body() = "Error 500. An error occurred: '" + std::string(why) + "'";
    res.prepare_payload();
    return res;
}