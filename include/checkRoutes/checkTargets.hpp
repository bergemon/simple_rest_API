#pragma once
#include "../getData/getData.hpp"
#include "../auth/auth.hpp"
#include "../sharedFunctions.hpp"
#include "../HTTP/createResponse.hpp"

namespace RouteCheckers {
    struct Target {
        std::string targetDest;
        bool hasQuery;
        std::vector<std::string> m_queryParams;
    };

    const Target targets[] = {
        { "users", true, { "min", "max", "getUserPass" } },
        { "auth", false, {} }
    };

    // Funtion prototypes
    bool hasSuchQueryParams(const int pos, const std::string);

    bool isRequestTargetOk(const std::string target);

    int requestTargetPos(const std::string target);

    bool hasQueryParams(const int pos);

    ResponseInfo::resInfo routePreCheck(const std::string target, const http::verb method);

    ResponseInfo::dataInfo GetRequestedData(const int pos, const std::string body,
        const std::string query = 0, const std::string cookie = 0);
}