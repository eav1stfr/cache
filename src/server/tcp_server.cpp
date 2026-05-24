#include "tcp_server.h"
#include "server/client_handler.h"

#include <iostream>
#include <stdexcept>
#include <cstring>

#include <arpa/inet.h>

namespace kvstore {

TCPServer::TCPServer(int port, size_t capacity)
    : port_(port)
    , server_fd_(-1)
    , store_(capacity)
{}

TCPServer::~TCPServer() {
    if (server_fd_ != -1) {
        close(server_fd_);
    }
}

void TCPServer::run() {
    setup();
    accept_loop();
}

void TCPServer::setup() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        throw std::runtime_error("Failed to create socket: " + std::string(strerror(errno)));
    }

    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        throw std::runtime_error("Failed to set SO_REUSEADDR: " + std::string(strerror(errno)));
    }

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) == -1) {
        throw std::runtime_error("Failed to bind to port " + std::to_string(port_) + ": " + strerror(errno));
    }

    if (listen(server_fd_, 10) == -1) {
        throw std::runtime_error("Failed to listen: " + std::string(strerror(errno)));
    }

    std::cout << "[server] Listening on port " << port_ << std::endl;
}

void TCPServer::accept_loop() {
    while (true) {
        struct sockaddr_in client_address;
        socklen_t client_len = sizeof(client_address);

        int client_fd = accept(server_fd_, (struct sockaddr*)&client_address, &client_len);
        if (client_fd == -1) {
            std::cerr << "[server] accept() failed: " << strerror(errno) << std::endl;
            continue;
        }

        std::cout << "[server] Client connected: " << inet_ntoa(client_address.sin_addr) << std::endl;

        ClientHandler handler(client_fd, store_);
        handler.handle();

        std::cout << "[server] Client disconnected." << std::endl;
    }
}

} // namespace kvstore