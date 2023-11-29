#include "../include/checkRoutes.hpp"
#include "../include/getData/getData.hpp"
#include "../include/handleRequest.hpp"

// Route pre-check
HandleRequest::resInfo HandleRequest::routePreCheck(std::string target, http::verb method) {
    // Make sure we can handle the method
    if (method != http::verb::get && method != http::verb::head)
        return { HandleRequest::responseStatus::STATUS_BAD_REQUEST,
            "Unknown HTTP-method" };

    // Requested target must exist
    int targetPos = RouteCheckers::requestTargetPos(target);
    if (targetPos == -1)
        return { HandleRequest::responseStatus::STATUS_NOT_FOUND,
            target };

    // Request target must be absolute and not contain "..".
    if (!RouteCheckers::isRequestTargetOk(target))
        return { HandleRequest::responseStatus::STATUS_BAD_REQUEST,
            "Illegal request-target" };

    // If target request have no query parameters            
    if (!RouteCheckers::hasQueryParams(targetPos))
        return { HandleRequest::responseStatus::STATUS_BAD_REQUEST,
            "There are no query parameters in this target" };

    return { HandleRequest::responseStatus::STATUS_OK, "" };
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
std::string RouteCheckers::GetRequestedData(const int pos, const std::string query) {
    // users
    if (pos == 0)
        return DataBase::getUsers(query);

    return "Bad request";
}