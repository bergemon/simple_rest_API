#include "../include/dependencies.hpp"
#include "../include/timer/timer.hpp"
#include "../include/webSocket/webSocketListener.hpp"
#include "../include/server.hpp"
#include "../include/webSocket/webSocketListener.hpp"
#include "variables.hpp"

#define _DEBUG

int main(int argc, char** argv) {

#ifdef _WIN32
#ifdef _DEBUG
    setlocale(LC_ALL, "Rus");

    // Timer that can be used in some functions
    // Clock::Timer mainTimer;
#endif _DEBUG
#endif _WIN32

    std::cout << "Pet REST API by bergemon ver. "
        << APP_VERSION_MAJOR << '.' << APP_VERSION_MINOR << '\n'
        << "WebSocket listener will run on the passed port plus five."
        << std::endl;

    // Check command line arguments.
    // if (argc < 2 && argc != 3) {
    //     std::cerr << "Usage: [AppName] <port> <threads>\n" << "Example: [AppName] 80 1\n"
    //         << "Passing number of threads is not necessary.\n"
    //         << "If you don't pass number of threads, there will be only one."
    //         << std::endl;
    //     return EXIT_FAILURE;
    // }
    // auto const port = static_cast<unsigned short>(std::atoi(argv[1]));
    unsigned short port = 80;
    int threads = 1;
    if (argc > 2)
        threads = std::max<int>(1, std::atoi(argv[2]));

    asio::io_context context_{ threads };

    // Create and launch a listening port
    std::make_shared<Server::server>(context_, tcp::endpoint{ tcp::v4(), port })->run();

    // Create and launch a websocket listening port
    unsigned short webSockPort = port + 5;
    std::make_shared<WS_listener::WebSockListener>(context_, tcp::endpoint{ tcp::v4(), webSockPort })->run();

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> threadsArray;
    threadsArray.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        threadsArray.emplace_back([&context_] { context_.run(); });
    context_.run();

    return EXIT_SUCCESS;

    return 0;
}