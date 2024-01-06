#include "../../include/auth/auth.hpp"

namespace Authentication {
    static constexpr char secretKeySession[] = "gfjdioghjdfghdfjgnsdOGJNSDFOGNOSRJghnosgnhosdjgnodjgosrijg90r4"
    "8t83548auGHUARIHgurshgozfhxbuidfzhbuifdhguiozsrhnhuobdtubrhsjz0gihrozughdughduzighdtuhzogushdrjgiu5"
    "e90yzujt94w7t8(IHuISHsuivhS89yg7rsghoURSJviosRHigurhsOUVRsgiUHSRUIGBEshuoghOSUGhfuoHSOGUrYIShorugh";

    static constexpr char secretKeyRefresh[] = "gfdgfdgfdgfdrjijoriptuorituwrioptkrwopfkmwfmekldsmeopkfropeut83"
    "dsjfdusoivhds7vytsuhfeosfkedsiofjdisufhdygfyeadjfieusuagfaxgcasczxcvxpjuiuiyvpdpfosifsfjrsghisrhfyfrsgrsgs"
    "GFKdgnjdfhgdfuighfd9uu8ty573489hgIZUJHGFDgvuofghdfioghsruiyg79drszghesdipfleshtdkohfopiju[klfjdsjfdios4345";

    static constexpr char issuer[] = "bergemon_REST_API";
}

ResponseInfo::tokensInfo Authentication::authUser(const std::string cookie) {

    const std::string stk = cookie.substr(cookie.find("stk_=") + 5,
        cookie.find(";") > cookie.find("stk_=") + 5
        ? cookie.find(";") - 5 : cookie.length() - (cookie.find("stk_=") + 5)
    );

    const std::string rtk = cookie.substr(cookie.find("rtk_=") + 5,
        cookie.find(";") > cookie.find("rtk_=") + 5
        ? cookie.find(";") - 5 : cookie.length() - (cookie.find("rtk_=") + 5)
    );

    std::time_t tt;
    std::stringstream ss;
    std::string stk_utc, rtk_utc;
    
    if (stk.length() > 0)
    {
        try {
            const auto stk_decoded = jwt::decode(stk);
            for (const auto& elem : stk_decoded.get_payload_json()) {
                // DEBUG
                std::cout << elem.first << ": " << elem.second.to_str() << std::endl;
                // DEBUG
                if (elem.first == "exp") {
                    tt = std::atoi(elem.second.to_str().c_str());
                    ss << std::put_time(std::gmtime(&tt), "%a %b %d %H:%M:%S %Y");
                    stk_utc = ss.str();

                    if (std::atoi(elem.second.to_str().c_str()) < std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1)) {
                        return {};
                    }
                }
            }

            // Verify session token
            const auto stk_verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{ Authentication::secretKeySession })
                .with_issuer(Authentication::issuer);
            stk_verifier.verify(stk_decoded);

            // Refresh token
            const auto rtk_decoded = jwt::decode(rtk);
            for (const auto& elem : rtk_decoded.get_payload_json()) {
                if (elem.first == "exp") {
                    tt = std::atoi(elem.second.to_str().c_str());
                    ss.str("");
                    ss << std::put_time(std::gmtime(&tt), "%a %b %d %H:%M:%S %Y");
                    rtk_utc = ss.str();

                    if (std::atoi(elem.second.to_str().c_str()) < std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1)) {
                        return {};
                    }
                }
            }
            // Verify refresh token
            const auto rtk_verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{ Authentication::secretKeyRefresh })
                .with_issuer(Authentication::issuer);

            rtk_verifier.verify(rtk_decoded);
        }
        catch(...) { return {}; }

        return {stk, stk_utc, rtk, rtk_utc};
    }
    else if (rtk.length() > 0) {
        try {
            const auto rtk_decoded = jwt::decode(rtk);
            for (const auto& elem : rtk_decoded.get_payload_json()) {
                if (elem.first == "exp") {
                    if (std::atoi(elem.second.to_str().c_str()) < std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1)) {
                        return {};
                    }
                }
            }
            // Verify refresh token
            const auto verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{ Authentication::secretKeyRefresh })
                .with_issuer(Authentication::issuer);

            verifier.verify(rtk_decoded);
        }
        catch(...) { return {}; }

        return {};
    }

    return {};
}

ResponseInfo::dataInfo Authentication::validateUser(const std::string body) {
    using namespace ResponseInfo;

    /*  JSON structure for this route:
    {
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
                            throw std::invalid_argument("Wrong JSON structure");
                        if (key == "username")
                            usrname = value.dump().substr(1, value.dump().size() - 2);
                        else if (key == "password")
                            usrpass = value.dump().substr(1, value.dump().size() - 2);
                    }
                }
                if (itemsCount > 1)
                    throw std::invalid_argument("Wrong JSON structure");
            }

            pqxx::row r = worker.exec1("SELECT username, password FROM users WHERE username LIKE '" + usrname + "'");
            if (usrpass == r.at(1).c_str()) {
                nlohmann::ordered_json j = nlohmann::json::object();
                j.push_back({ "authorization", "success" });
                return { STATUS_OK, j.dump(), getToken(usrname), "", true };
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

ResponseInfo::tokensInfo Authentication::getToken(const std::string username) {

	jwt::claim from_raw_json(
        std::string(
            R"###({"username":")###" + username + R"###(","roles":["user", "admin"]})###"
        )
    );

    std::chrono::time_point stp = std::chrono::system_clock::now() + std::chrono::minutes(10);
    std::chrono::time_point rtp = std::chrono::system_clock::now() + std::chrono::days(90);
    std::time_t tt;
    std::stringstream ss;

    // Session token UTC time
    tt = std::chrono::system_clock::to_time_t(stp);
    ss << std::put_time(std::gmtime(&tt), "%a %b %d %H:%M:%S %Y");
    std::string stk_utc = ss.str();
    ss.str("");

    // Refresh token UTC time
    tt = std::chrono::system_clock::to_time_t(rtp);
    ss << std::put_time(std::gmtime(&tt), "%a %b %d %H:%M:%S %Y");
    std::string rtk_utc = ss.str();

    const std::string sToken = jwt::create()
        .set_issuer(Authentication::issuer)
        .set_type("JWT")
        .set_id("access_token")
        .set_payload_claim("username", from_raw_json)
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(stp)
        .sign(jwt::algorithm::hs256{ Authentication::secretKeySession });

    const std::string rToken = jwt::create()
        .set_issuer(Authentication::issuer)
        .set_type("JWT")
        .set_id("refresh_token")
        .set_payload_claim("username", from_raw_json)
        .set_issued_at(std::chrono::system_clock::now())
        .set_expires_at(rtp)
        .sign(jwt::algorithm::hs256{ Authentication::secretKeyRefresh });

    return ResponseInfo::tokensInfo("stk_=" + sToken, stk_utc, "rtk_=" + rToken, rtk_utc);
}