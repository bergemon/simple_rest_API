#include "../../include/auth/auth.hpp"
#include <fstream>

namespace Authentication {
    static constexpr char secretKey[] = "AFGkdjfoir897438543fdhnsgfdgfd";
    static constexpr char issuer[] = "bergemon_REST_API";
}

bool Authentication::authUser(const std::string token) {
    const auto decoded = jwt::decode(token);
    
    const auto verifier = jwt::verify()
        .allow_algorithm(jwt::algorithm::hs256{ Authentication::secretKey })
        .with_issuer(Authentication::issuer);

    try {
        verifier.verify(decoded);
    }
    catch(const std::exception& e) {
        return false;
    }

    return true;
}

ResponseInfo::dataInfo Authentication::validateUser(const std::string body) {
    using namespace ResponseInfo;

    /* {
        Authorization: {
            username: *,
            password: *
        }
    } */

    if (body.find("Authorization") != std::string::npos
        && body.find("username") != std::string::npos
        && body.find("password") != std::string::npos)
    {
        try {
            pqxx::connection connection_("host=127.0.0.1 port=5432 dbname=bergemon user=postgres password=Goldenm4a1");
            pqxx::work worker(connection_);

            nlohmann::json data = nlohmann::json::parse(body);
            std::string usrname, usrpass;
            int itemsCount = 0;
            for (auto& [key, value] : data.items()) {
                ++itemsCount;
                if (key == "Authorization" && data[key].is_object()) {
                    for (auto& [key, value] : data[key].items()) {
                        if (key != "username" && key != "password")
                            throw std::exception("Wrong JSON structure");
                        if (key == "username")
                            usrname = value.dump().substr(1, value.dump().size() - 2);
                        else if (key == "password")
                            usrpass = value.dump().substr(1, value.dump().size() - 2);
                    }
                }
                if (itemsCount > 1)
                    throw std::exception("Wrong JSON structure");
            }

            pqxx::row r = worker.exec1("SELECT username, password FROM users WHERE username LIKE '" + usrname + "'");
            if (usrpass == r.at(1).c_str()) {
                return { STATUS_OK, getToken(usrname, usrpass), ".json", true };
            }
            else
                return { STATUS_PERMISSION_DENIED, "Wrong password, try again" };

        }
        catch (const std::exception& e) {
            std::cerr << "ValidateUser Error: " << e.what() << std::endl;
            if (std::string(e.what()).find("json") != std::string::npos)
                return { STATUS_PERMISSION_DENIED, "Wrong JSON format" };
            else if (std::string(e.what()).find("Expected 1 row(s)") != std::string::npos)
                return { STATUS_PERMISSION_DENIED, "There is no such username, try again" };

            return { STATUS_PERMISSION_DENIED, e.what() };
        }
    }

    return { STATUS_PERMISSION_DENIED, "Wrong JSON structure" };
}

std::string Authentication::getToken(const std::string username, const std::string password) {

	jwt::claim from_raw_json;
	std::istringstream iss{R"({"roles":["user", "admin"]})"};
	iss >> from_raw_json;

    auto token = jwt::create()
        .set_issuer(Authentication::issuer)
        .set_type("JWT")
        .set_id("access_token")
        .set_payload_claim("roles", from_raw_json)
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(std::chrono::system_clock::now() + std::chrono::seconds(3600))
        .sign(jwt::algorithm::hs256{ Authentication::secretKey });

    return token.c_str();
}