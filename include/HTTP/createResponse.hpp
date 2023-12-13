#pragma once
#include "../checkRoutes/checkTargets.hpp"

namespace CreateResponse {
    // Funtion prototypes
    // Returns a bad request response
    http::message_generator bad_request(const uint32_t version, const std::string why, const std::string target, const bool keep_alive);
    // Returns wrong method response
    http::message_generator method_not_allowed(const uint32_t version, const std::string why,
    const std::string target, const bool keep_alive);
    // Returns a not found response
    http::message_generator not_found(const uint32_t version, const std::string why, const std::string target, const bool keep_alive);
    // Returns a permission denied response
    http::message_generator forbidden(const uint32_t version, const std::string why, const std::string target, const bool keep_alive);
    // Returns a server error response
    http::message_generator server_error(const uint32_t version, const std::string why, const std::string target, const bool keep_alive);
    // Generate a response if preCheck status was good
    http::message_generator createResponse(const uint32_t version, const std::string target,
        const bool keep_alive, const http::verb method, const std::string body, const std::string head);
}