#include "../include/checkRoutes/checkTargets.hpp"

// Route pre-check
ResponseInfo::resInfo RouteCheckers::routePreCheck(const std::string target,
    const http::verb method)
{
    using namespace ResponseInfo;
    // Getting target position
    int32_t targetPos = RouteCheckers::requestTargetPos(target);

    // Make sure we can handle the method
    if (method != http::verb::get && method != http::verb::head && method != http::verb::post)
        return { STATUS_BAD_REQUEST, "Unknown HTTP-method" };

    // The requested target must handle the specified method
    if (targetPos == 1 && method != http::verb::post)
        return { STATUS_METHOD_NOT_ALLOWED, "Wrong method"};

    // Requested target must exist
    if (targetPos == -1)
        return { STATUS_NOT_FOUND, target };

    // Request target must be absolute and not contain "..".
    if (!RouteCheckers::isRequestTargetOk(target))
        return { STATUS_BAD_REQUEST,
            "Illegal request-target" };

    // If there are query string but target does not have it
    std::string query = boost::url_view(target).query();
    if (query.size() > 0
        && !RouteCheckers::hasQueryParams(targetPos))
        return { STATUS_BAD_REQUEST,
            "There are no query parameters in this target" };

    // Wrong query params
    if (query.size() > 0
        && !RouteCheckers::hasSuchQueryParams(targetPos, query))
        return { STATUS_BAD_REQUEST,
            "Passed wrong query parameters: '" + query + "'" };

    return { STATUS_OK, "" };
}

// RouteCheckers
// Requested target must exist
bool RouteCheckers::isRequestTargetOk(const std::string target) {
    if (target.size() == 0 || target[0] != '/'
        || target.find("..") != beast::string_view::npos)
        return false;

    return true;
}

// Request target must be absolute and not contain "..".
int RouteCheckers::requestTargetPos(const std::string target) {
    int pos = 0;

    for (const auto& elem : RouteCheckers::targets)
        if (static_cast<int>(target.find(elem.targetDest)) == 1)
            return pos;
        else
            ++pos;

    return -1;
}

// If target request have no query parameters
bool RouteCheckers::hasQueryParams(const int pos) {
    if (RouteCheckers::targets[pos].hasQuery)
        return true;
            
    return false;
}

// Check query string
bool RouteCheckers::hasSuchQueryParams(const int pos, const std::string query) {
    for (const auto& elem : RouteCheckers::targets[pos].m_queryParams) {
        if (query.find(elem) != std::string::npos)
            return true;
    }
    return false;
}

// Get requested data
ResponseInfo::dataInfo RouteCheckers::GetRequestedData
    (const int pos, const std::string body, const std::string query, const std::string cookie)
{
    using namespace ResponseInfo;

    // users
    if (pos == 0)
        return DataBase::getUsers(query, cookie);

    // auth
    if (pos == 1)
        return Authentication::validateUser(body);

    return { responseStatus::STATUS_NOT_FOUND, "Bad request" };
}