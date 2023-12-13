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

    struct dataInfo {
        responseStatus m_code;
        std::string m_data;
        std::string m_dataType;
        bool m_auth;

        dataInfo(const responseStatus& code, const std::string& data, const std::string& dataType = ".json", bool auth = false)
            : m_code(code), m_data(data), m_dataType(dataType), m_auth(auth) {}
    };
}