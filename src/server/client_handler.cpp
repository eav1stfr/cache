#include "client_handler.h"
#include "parser/parser.h"
#include "response/response.h"

#include <iostream>
#include <cstring>

namespace kvstore {

ClientHandler::ClientHandler(int client_fd, KVStore& store)
    : client_fd_(client_fd)
    , store_(store)
{}

ClientHandler::~ClientHandler() {
    if (client_fd_ != -1) {
        close(client_fd_);
    }
}

void ClientHandler::handle() {
    std::string line;

    while (read_line(line)) {
        if (line.empty()) {
            continue;
        }

        std::string response = process(line);
        send_response(response);
    }
}

bool ClientHandler::read_line(std::string& out_line) {
    out_line.clear();

    char byte;
    while (true) {
        ssize_t n = recv(client_fd_, &byte, 1, 0);

        if (n == 0) {
            return false;
        }

        if (n == -1) {
            std::cerr << "[client_handler] recv() error: " << strerror(errno) << std::endl;
            return false;
        }

        if (byte == '\r') {
            continue;
        }

        if (byte == '\n') {
            return true;
        }

        out_line += byte;
    }
}

std::string ClientHandler::process(const std::string& line) {
    Command cmd = Parser::parse(line);

    if (!cmd.is_valid()) {
        return Response::error("unknown command or wrong number of arguments");
    }

    switch (cmd.type) {
        case CommandType::GET: {
            const std::string& key = cmd.arg(0);
            return Response::from_get(store_.get(key));
        }

        case CommandType::SET: {
            const std::string& key   = cmd.arg(0);
            const std::string& value = cmd.arg(1);

            if (cmd.arg_count() == 3) {
                try {
                    int ttl = std::stoi(cmd.arg(2));
                    if (ttl <= 0) {
                        return Response::error("TTL must be a positive integer");
                    }
                    store_.set(key, value, ttl);
                } catch (const std::exception&) {
                    return Response::error("TTL must be a valid integer");
                }
            } else {
                store_.set(key, value);
            }

            return Response::ok();
        }

        case CommandType::DEL: {
            const std::string& key = cmd.arg(0);
            bool removed = store_.del(key);
            return removed ? Response::deleted() : Response::not_found();
        }

        default:
            return Response::error("unhandled command");
    }
}

void ClientHandler::send_response(const std::string& response) {
    size_t total_sent = 0;
    size_t total = response.size();

    while (total_sent < total) {
        ssize_t n = send(client_fd_, response.c_str() + total_sent, total - total_sent, 0);

        if (n == -1) {
            std::cerr << "[client_handler] send() error: " << strerror(errno) << std::endl;
            return;
        }

        total_sent += n;
    }
}

} // namespace kvstore