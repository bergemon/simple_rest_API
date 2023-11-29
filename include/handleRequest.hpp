#pragma once
#include "sharedFunctions.hpp"
#include "checkRoutes.hpp"
#include "getData/getData.hpp"
#include "auth/auth.hpp"

namespace HandleRequest {
    enum responseStatus {
        STATUS_BAD_REQUEST,
        STATUS_NOT_FOUND,
        STATUS_SERVER_ERROR,
        STATUS_OK
    };

    struct resInfo {
        responseStatus m_code;
        std::string m_why;

        resInfo(responseStatus code, std::string why)
            : m_code(code), m_why(why) {}
    };
    HandleRequest::resInfo routePreCheck(std::string target, http::verb method);

    template <class Body, class Allocator>
    http::message_generator handle_request(http::request<Body, http::basic_fields<Allocator>>&& req) {
        // Returns a bad request response
        auto const bad_request = [&req](beast::string_view why) {
            http::response<http::string_body> res{ http::status::bad_request, req.version() };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = std::string(why);
            res.prepare_payload();
            return res;
        };

        // Returns a not found response
        auto const not_found = [&req](beast::string_view target) {
            http::response<http::string_body> res{ http::status::not_found, req.version() };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "Target '" + std::string(target) + "' does not exist.";
            res.prepare_payload();
            return res;
        };

        // Returns a server error response
        auto const server_error = [&req](beast::string_view what) {
            http::response<http::string_body> res{ http::status::internal_server_error, req.version() };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, "text/html");
            res.keep_alive(req.keep_alive());
            res.body() = "An error occurred: '" + std::string(what) + "'";
            res.prepare_payload();
            return res;
        };

        // Route pre-check
        resInfo request_status = routePreCheck(req.target(), req.method());

        switch(request_status.m_code) {
            case STATUS_BAD_REQUEST:
                return bad_request(request_status.m_why);
            case STATUS_NOT_FOUND:
                return not_found(request_status.m_why);
            case STATUS_SERVER_ERROR:
                return server_error(request_status.m_why);
            case STATUS_OK:
                break;
        }

        // Getting request URI and query string
        boost::url_view url { req.target() };
        std::string query = url.query();

        // Body
        http::string_body::value_type body;
        body = RouteCheckers::GetRequestedData(RouteCheckers::requestTargetPos(req.target()), query);

        auto const size = body.size();

        // Respond to HEAD request
        if (req.method() == http::verb::head) {
            http::response<http::empty_body> res{ http::status::ok, req.version() };
            res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
            res.set(http::field::content_type, mime_type(".json"));
            res.set(http::field::set_cookie, "Lalalalalaley; Secure; HttpOnly");
            res.content_length(size);
            res.keep_alive(req.keep_alive());
            return res;
        }

        // Respond to GET request
        http::response<http::string_body> res{ std::piecewise_construct, std::make_tuple(std::move(body)),
            std::make_tuple(http::status::ok, req.version()) };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(".json"));
        res.set(http::field::set_cookie, "Lalalalalaley; Secure; HttpOnly");
        res.content_length(size);
        res.keep_alive(req.keep_alive());
        return res;
    }
}