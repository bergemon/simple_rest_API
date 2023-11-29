cmake_minimum_required(VERSION 3.27.0)

project(BeastFirstServer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(
    SOURCE_FILES

    # Entry point
    src/main.cxx

    # Websocket
    src/webSocket/webSocketSession.cxx

    # Handle HTTP session requests
    src/sharedFunctions.cxx

    # Authentication
    src/auth/auth.cxx

    # Work with database
    src/getData/getData.cxx

    # Check routes
    src/checkTargets.cxx
)

set (
    HEADER_FILES

    # Project general dependencies
    include/dependencies.hpp

    # Chrono class timer
    include/timer/timer.hpp

    # HTTP listener & sessions
    include/server.hpp
    include/session.hpp

    # Handle HTTP session requests
    include/handleRequest.hpp
    include/sharedFunctions.hpp

    # Websocket
    include/webSocket/webSocketListener.hpp
    include/webSocket/webSocketSession.hpp

    # Authentication
    include/auth/auth.hpp

    # Work with database
    include/getData/getData.hpp

    # Check routes
    include/checkRoutes.hpp
)

set(
    EXECUTABLE_NAME
    server
)

add_executable(
    ${EXECUTABLE_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
)

find_package(jwt-cpp REQUIRED)
if (jwt-cpp_FOUND)
    target_link_libraries(${EXECUTABLE_NAME} jwt-cpp::jwt-cpp)
else()
    message(STATUS "jwt-cpp NOT found!")
endif(jwt-cpp_FOUND)

find_package(libpqxx REQUIRED)
if(libpqxx_FOUND)
    target_link_libraries(${EXECUTABLE_NAME} libpqxx::pqxx)
else()
    message(STATUS "libpqxx NOT found!")
endif(libpqxx_FOUND)

find_package(PostgreSQL REQUIRED)
if(PostgreSQL_FOUND)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${PostgreSQL_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} ${PostgreSQL_LIBRARIES})
else()
    message(STATUS "PostgreSQL NOT found!")
endif(PostgreSQL_FOUND)

find_package(nlohmann_json REQUIRED)
if(nlohmann_json_FOUND)
    target_link_libraries(${EXECUTABLE_NAME} nlohmann_json::nlohmann_json)
else()
    message(STATUS "nlohmann_json NOT found!")
endif(nlohmann_json_FOUND)

find_package(Boost 1.82 REQUIRED url)
if (Boost_FOUND)
    target_include_directories(${EXECUTABLE_NAME} PUBLIC ${Boost_INCLUDE_DIRS})
    target_link_libraries(${EXECUTABLE_NAME} Boost::url)
else ()
    message(STATUS "Boost NOT found!")
endif(Boost_FOUND)

find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
    target_link_libraries(${EXECUTABLE_NAME} OpenSSL::SSL)
else()
    message(STATUS "OpenSSL NOT found!")
endif(OpenSSL_FOUND)

if(WIN32)
    target_link_libraries(${EXECUTABLE_NAME} crypt32)
    target_link_libraries(${EXECUTABLE_NAME} ws2_32)
    target_link_libraries(${EXECUTABLE_NAME} wsock32)
endif(WIN32)

configure_file(
    variables.hpp.in
    variables.hpp
)

target_include_directories(
    ${EXECUTABLE_NAME}
    PUBLIC
    ${PROJECT_BINARY_DIR}
)