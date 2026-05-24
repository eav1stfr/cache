#include <iostream>
#include <stdexcept>

#include "server/tcp_server.h"

int main(int argc, char* argv[]) {
    int port = 6379;
    size_t capacity = 1000;

    if (argc >= 2) {
        try {
            port = std::stoi(argv[1]);
        } catch (const std::exception&) {
            std::cerr << "Invalid port: " << argv[1] << std::endl;
            return 1;
        }
    }

    if (argc >= 3) {
        try {
            capacity = std::stoul(argv[2]);
        } catch (const std::exception&) {
            std::cerr << "Invalid capacity: " << argv[2] << std::endl;
            return 1;
        }
    }

    try {
        kvstore::TCPServer server(port, capacity);
        server.run();
    } catch (const std::exception& e) {
        std::cerr << "[fatal] " << e.what() << std::endl;
        return 1;
    }

    return 0;
}