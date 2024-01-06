#include "../include/HTTP/createResponse.hpp"

http::message_generator CreateResponse::createResponse(const uint32_t version, const std::string target,
    const bool keep_alive, const http::verb method, const std::string body, const std::string cookies) {

    // Getting request URI and query string
    boost::url_view url { target };
    std::string query = url.query();

    // NEED TO CHECK BODY TYPE IN RESPONSE HERE!!!
    // NEED TO CHECK BODY TYPE IN RESPONSE HERE!!!
    // NEED TO CHECK BODY TYPE IN RESPONSE HERE!!!
    // Body
    http::string_body::value_type resBody;

    using namespace ResponseInfo;
    int32_t targetPos = RouteCheckers::requestTargetPos(target);
    dataInfo resTempData = RouteCheckers::GetRequestedData(targetPos, body, query, cookies);

    switch(resTempData.m_code) {
        case STATUS_NOT_FOUND:
            return not_found(version, resTempData.m_data, target, keep_alive);
        case STATUS_PERMISSION_DENIED:
            return forbidden(version, resTempData.m_data, target, keep_alive);
        case STATUS_SERVER_ERROR:
            return server_error(version, resTempData.m_data, target, keep_alive);
        case STATUS_OK:
            resBody = resTempData.m_data;
            break;
    }
    auto const size = resBody.size();

    // Respond to HEAD request
    if (method == http::verb::head)
    {
        http::response<http::empty_body> res{ http::status::ok, version };
        res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
        res.set(http::field::content_type, mime_type(".json"));
        res.set(http::field::access_control_allow_origin, "*");
        if (resTempData.tokens.hasTokens)
        {
            // Session token
            res.set("Set-Cookie",
                resTempData.tokens.sessionToken
                + "; path=/; secure; httponly; expires="
                + resTempData.tokens.stk_utc
            );
            // Refresh token
            res.insert("Set-Cookie",
                resTempData.tokens.refreshToken
                + "; path=/; secure; httponly; expires="
                + resTempData.tokens.rtk_utc
            );
        }
        res.content_length(size);
        res.keep_alive(keep_alive);
        return res;
    }

    // Respond to GET request
    http::response<http::string_body> res{ std::piecewise_construct, std::make_tuple(std::move(resBody)),
        std::make_tuple(http::status::ok, version) };
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, mime_type(".json"));
    res.set(http::field::access_control_allow_origin, "*");
    if (resTempData.tokens.hasTokens)
    {
        // Session token
        res.set("Set-Cookie",
            resTempData.tokens.sessionToken
            + "; path=/; secure; httponly; expires="
            + resTempData.tokens.stk_utc
        );
        // Refresh token
        res.insert("Set-Cookie",
            resTempData.tokens.refreshToken
            + "; path=/; secure; httponly; expires="
            + resTempData.tokens.rtk_utc
        );
    }
    res.content_length(size);
    res.keep_alive(keep_alive);
    return res;
}