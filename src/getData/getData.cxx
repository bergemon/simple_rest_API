#include "../../include/getData/getData.hpp"

std::string DataBase::getUsers(const std::string query) {
    try {
        pqxx::connection connection_("host=127.0.0.1 port=5432 dbname=bergemon user=postgres password=Goldenm4a1");
        pqxx::work worker(connection_);
        pqxx::result res;

        if (query.size() > 0) {
            int min = std::atoi(query.substr(query.find("min") + 4,
                query.find("&") - query.find("=")).c_str());
            int max = std::atoi(query.substr(query.find("max") + 4,
                query.length() - query.rfind("=")).c_str());

            res = worker.exec("SELECT * FROM users WHERE id >=" + std::to_string(min) + " AND id <=" + std::to_string(max));
        }
        else
            res = worker.exec("SELECT * FROM users");

        nlohmann::ordered_json j = nlohmann::json::array();

        for (const auto& row : res) {
            nlohmann::ordered_json currObj = nlohmann::json::object();
            for (const auto& column : row) {
                currObj.push_back({ column.name(), column.c_str() });
            }
            j.push_back(currObj);
        }

        worker.commit();
        return j.dump();
    }
    catch (const std::exception& e) {
        nlohmann::ordered_json j = nlohmann::json::object();
        j.push_back({ "getUsers", "some error occured" });
        j.push_back({ "error_message", e.what() });
        return j.dump();
    }
}