#pragma once

#ifdef _WIN32
#define _WIN32_WINNT 0x0601
#endif

#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/config.hpp>
#include <boost/url.hpp>
#include <boost/optional.hpp>
#include <nlohmann/json.hpp>
#include <pqxx/pqxx>
#include <jwt-cpp/jwt.h>
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <filesystem>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <ctime>
#include <unordered_set>

namespace beast = boost::beast;				// from <boost/beast.hpp>
namespace http = beast::http;				// from <boost/beast/http.hpp>
namespace asio = boost::asio;				// from <boost/asio.hpp>
namespace ssl = boost::asio::ssl;			// from <boost/asio/ssl.hpp>
using tcp = boost::asio::ip::tcp;			// from <boost/asio/ip/tcp.hpp>
namespace filesystem = std::filesystem;		// filesystem
namespace websocket = beast::websocket;		// from <boost/beast/websocket.hpp>