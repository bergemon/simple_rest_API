#pragma once
#include "../responseInfo.hpp"

namespace Authentication {
    ResponseInfo::dataInfo validateUser(const std::string body);

    bool authUser(const std::string token);
    
    std::string getToken(const std::string username, const std::string password);
}