#pragma once
#include "../responseInfo.hpp"

namespace Authentication {
    ResponseInfo::dataInfo validateUser(const std::string body);

    ResponseInfo::tokensInfo authUser(const std::string cookies);
    
    ResponseInfo::tokensInfo getToken(const std::string username);
}