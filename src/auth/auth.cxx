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

    std::string stk = "";
    if (cookie.find("stk_=") != std::string::npos) {
        stk = cookie.substr(cookie.find("stk_=") + 5,
            cookie.find(";") > cookie.find("stk_=") + 5
            ? cookie.find(";") - 5 : cookie.length() - (cookie.find("stk_=") + 5)
        );
    }

    std::string rtk = "";
    if (cookie.find("rtk_=") != std::string::npos) {
        rtk = cookie.substr(cookie.find("rtk_=") + 5,
            cookie.find(";") > cookie.find("rtk_=") + 5
            ? cookie.find(";") - 5 : cookie.length() - (cookie.find("rtk_=") + 5)
        );
    }

    std::time_t tt;
    std::stringstream ss;
    std::string stk_utc, rtk_utc;
    long current_time = std::chrono::system_clock::now().time_since_epoch() / std::chrono::seconds(1);
    std::string username = "";
    
    // Check session token if we have it
    if (stk.length() > 0 && rtk.length() > 0)
    {
        try {
            // deconde refresh token
            const auto stk_decoded = jwt::decode(stk);

            // getting token's fields
            for (const auto& elem : stk_decoded.get_payload_json()) {
                // get session token expirency
                if (elem.first == "exp") {
                    tt = std::atoi(elem.second.to_str().c_str());
                    ss << std::put_time(std::gmtime(&tt), "%a %b %d %H:%M:%S %Y");
                    stk_utc = ss.str();

                    // If access token is not actual
                    if (std::atoi(elem.second.to_str().c_str()) < current_time) {
                        // first of all decode refresh token from base64
                        const auto rtk_decoded = jwt::decode(rtk);

                        // get refresh token expirency
                        for (const auto& elem : rtk_decoded.get_payload_json()) {
                            if (elem.first == "exp") {
                                tt = std::atoi(elem.second.to_str().c_str());
                                ss.str("");
                                ss << std::put_time(std::gmtime(&tt), "%a %b %d %H:%M:%S %Y");
                                rtk_utc = ss.str();

                                // return unauthorized if rtk expired
                                if (std::atoi(elem.second.to_str().c_str()) < current_time) {
                                    return {};
                                }

                                // then verify refresh token
                                const auto rtk_verifier = jwt::verify()
                                    .allow_algorithm(jwt::algorithm::hs256{ Authentication::secretKeyRefresh })
                                    .with_issuer(Authentication::issuer);
                                rtk_verifier.verify(rtk_decoded);
                            }
                            // get username from refresh token if rtk was verified
                            if (elem.first == "username") {
                                std::string tempStr = elem.second.to_str();
                                size_t start = tempStr.find("\"username\":") + 12;
                                size_t subStrSize = tempStr.find("\",\"roles\":") - start;
                                username = tempStr.substr(start, subStrSize);
                            }
                        }
                        // Get new tokens pair if old rtk was successfully verified
                        // Make sure we got username to put it in new tokens
                        if (username.length() > 0) {
                            return Authentication::getToken(username);
                        }
                        // Unauthorized if there was no username
                        else { return {}; }
                    }
                }
            }

            // Verify session token if it's not expired
            // If expired - we got new tokens pair in code above
            const auto stk_verifier = jwt::verify()
                .allow_algorithm(jwt::algorithm::hs256{ Authentication::secretKeySession })
                .with_issuer(Authentication::issuer);
            stk_verifier.verify(stk_decoded);

            // Getting refresh token expirency to return both current tokens back
            const auto rtk_decoded = jwt::decode(rtk);
            for (const auto& elem : rtk_decoded.get_payload_json()) {
                if (elem.first == "exp") {
                    tt = std::atoi(elem.second.to_str().c_str());
                    ss.str("");
                    ss << std::put_time(std::gmtime(&tt), "%a %b %d %H:%M:%S %Y");
                    rtk_utc = ss.str();
                }
            }
        }
        // Any token is wrong (not verified)
        catch(...) { return {}; }

        // Return both tokens with expirency time in utc format
        return {"stk_=" + stk, stk_utc, "rtk_=" + rtk, rtk_utc};
    }
    // Check refresh token if we haven't session token
    else if (rtk.length() > 0) {
        std::string username;

        try {
            // decode refresh token
            const auto rtk_decoded = jwt::decode(rtk);

            // getting token's fields
            for (const auto& elem : rtk_decoded.get_payload_json()) {
                if (elem.first == "username") {
                    std::string tempStr = elem.second.to_str();
                    size_t start = tempStr.find("\"username\":") + 12;
                    size_t subStrSize = tempStr.find("\",\"roles\":") - start;
                    username = tempStr.substr(start, subStrSize);
                }
                if (elem.first == "exp") {
                    if (std::atoi(elem.second.to_str().c_str()) < current_time) {
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
        // Token is wrong (not verified)
        catch(...) { return {}; }

        // Get new tokens pair if we verified refresh token
        // and got username from token's fields
        if (username.length() > 0) {
            return Authentication::getToken(username);
        }
        else { return {}; }
    }

    // Return unauthorized by default
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