#pragma once
#include "createResponse.hpp"

namespace HandleRequest {
    template <class Body, class Allocator>
    http::message_generator handle_request(http::request<Body, http::basic_fields<Allocator>>&& req) {
        using namespace ResponseInfo;
        using namespace CreateResponse;

        // Route pre-check
        resInfo request_status = RouteCheckers::routePreCheck(req.target(), req.method());

        switch(request_status.m_code) {
            case STATUS_BAD_REQUEST:
                return bad_request(req.version(), request_status.m_why, req.target(), req.keep_alive());
            case STATUS_METHOD_NOT_ALLOWED:
                return method_not_allowed(req.version(), request_status.m_why, req.target(), req.keep_alive());
            case STATUS_NOT_FOUND:
                return not_found(req.version(), request_status.m_why, req.target(), req.keep_alive());
            case STATUS_PERMISSION_DENIED:
                return forbidden(req.version(), request_status.m_why, req.target(), req.keep_alive());
            case STATUS_SERVER_ERROR:
                return server_error(req.version(), request_status.m_why, req.target(), req.keep_alive());
            case STATUS_OK:
                break;
        }

        std::string cookie;
        for (auto const& field : req) {
            if (field.name_string() == "Cookie")
                cookie = field.value();
        }

        return createResponse(req.version(), req.target(),
            req.keep_alive(), req.method(), req.body(), cookie);
    }
}