#pragma once

#include <string>

#include <unistd.h>
#include <sys/socket.h>

#include "storage/kv_store.h"

namespace kvstore {

class ClientHandler {
public:
    ClientHandler(int client_fd, KVStore& store);
    ~ClientHandler();

    void handle();

private:
    int client_fd_;
    KVStore& store_;

    bool read_line(std::string& out_line);
    std::string process(const std::string& line);
    void send_response(const std::string& response);
};

} // namespace kvstore