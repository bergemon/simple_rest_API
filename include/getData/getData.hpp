#pragma once
#include "../auth/auth.hpp"

namespace DataBase {
    std::string getUsers(const std::string query, const std::string cookie = 0);
}