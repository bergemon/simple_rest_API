#pragma once
#include "../auth/auth.hpp"

namespace DataBase {
    ResponseInfo::dataInfo getUsers(const std::string query, const std::string cookie = 0);
}