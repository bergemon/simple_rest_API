#pragma once
#include "dependencies.hpp"

namespace RouteCheckers {
    struct Target {
        std::string targetDest;
        bool hasQuery;
    };

    constexpr Target targets[] = {
        { "users", true },
        { "auth", false }
    };

    bool isRequestTargetOk(const std::string target);

    int requestTargetPos(const std::string target);

    bool hasQueryParams(const int pos);

    std::string GetRequestedData(const int pos, const std::string query = 0);
}