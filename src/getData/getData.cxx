#include "../../include/getData/getData.hpp"

ResponseInfo::dataInfo DataBase::getUsers(const std::string query, const std::string cookie) {

    try {
        pqxx::connection connection_("host=127.0.0.1 port=5432 dbname=bergemon user=postgres password=Goldenm4a1");
        pqxx::work worker(connection_);
        pqxx::result res;

        if (query.size() > 0) {
            if (query.find("getUserPass") != std::string::npos) {
                // Auth user and get new tokens or old tokens back
                ResponseInfo::tokensInfo tokens = Authentication::authUser(cookie);

                // Return userpass if user was authenticated
                if (tokens.hasTokens) {
                    std::string username = query.substr(query.find("=") + 1, query.size() - (query.find("=") + 1));
                    pqxx::row r = worker.exec1("SELECT password FROM users WHERE username LIKE '" + username + "'");
                    nlohmann::ordered_json j = nlohmann::json::object();
                    j.push_back({ "userPass", r.at(0).c_str() });
                    return { ResponseInfo::responseStatus::STATUS_OK, j.dump(), tokens};
                }
                else
                    throw std::invalid_argument("unauthorized");
            }


            int min = std::atoi(query.substr(query.find("min") + 4,
                query.find("&") - query.find("=")).c_str());
            int max = std::atoi(query.substr(query.find("max") + 4,
                query.length() - query.rfind("=")).c_str());

            res = worker.exec("SELECT id, email, username, date_of_sign_up, phone FROM users WHERE id >=" + std::to_string(min) + " AND id <=" + std::to_string(max));
        }
        else
            res = worker.exec("SELECT id, email, username, date_of_sign_up, phone FROM users");

        nlohmann::ordered_json j = nlohmann::json::array();

        for (const auto& row : res) {
            nlohmann::ordered_json currObj = nlohmann::json::object();
            for (const auto& column : row) {
                currObj.push_back({ column.name(), column.c_str() });
            }
            j.push_back(currObj);
        }

        worker.commit();
        return { ResponseInfo::responseStatus::STATUS_OK, j.dump() };
    }
    catch (const std::exception& e) {
        return { ResponseInfo::responseStatus::STATUS_SERVER_ERROR, e.what() };
    }
}