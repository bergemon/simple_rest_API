#pragma once
#include "dependencies.hpp"

namespace ResponseInfo {
    enum responseStatus {
        STATUS_BAD_REQUEST,
        STATUS_METHOD_NOT_ALLOWED,
        STATUS_NOT_FOUND,
        STATUS_SERVER_ERROR,
        STATUS_PERMISSION_DENIED,
        STATUS_OK
    };

    struct resInfo {
        responseStatus m_code;
        std::string m_why;

        resInfo(responseStatus code, std::string why)
            : m_code(code), m_why(why) {}
    };


    struct tokensInfo {
        const bool hasTokens = false;

        const std::string sessionToken;
        const std::string stk_utc;

        const std::string refreshToken;
        const std::string rtk_utc;

        tokensInfo() {}

        tokensInfo(std::string stk, std::string stk_utc, std::string rtk, std::string rtk_utc)
            : hasTokens(true), sessionToken(stk), stk_utc(stk_utc), refreshToken(rtk), rtk_utc(rtk_utc)
        {}
    };

    struct dataInfo {
        const responseStatus m_code;
        const std::string m_data;
        const std::string m_dataType;
        const tokensInfo tokens;

        dataInfo(const responseStatus& code, const std::string& data, const std::string& dataType = ".json", bool auth = false)
            : m_code(code), m_data(data), m_dataType(dataType) {}

        dataInfo(const responseStatus& code, const std::string& data, tokensInfo t, const std::string& dataType = ".json", bool auth = false)
        : m_code(code), m_data(data), m_dataType(dataType), tokens(t) {}
    };
}