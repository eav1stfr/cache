#pragma once

#include <string>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "storage/kv_store.h"

namespace kvstore {

class TCPServer {
private:
    int port_;      //the port number to bind to
    int server_fd_; // file descriptor for the listening socket
    KVStore store_; // the single shared KVStore instance, all clients share it

    // creates the socket, sets socker options, binds to the port, and starts listening
    // must be called once at the beginning of run()
    void setup();

    // runs an infinite loop calling accept() to wait for incoming connections
    // each accepted connection if handled synchronously by a ClientHandler
    void accept_loop();
public:
    // Constructor takes the port to liste on and the maxx num of entries
    // the KVstore can hold before LRU eviction kicks in
    TCPServer(int port, size_t capacity);

    // Destructor closes the server socker if it is still open. RAII pattern
    ~TCPServer();

    // Starts the server, blocks forever, cause runs the accept loop
    // Foe each incoming client, it must create a ClientHandler and process them synchronously 
    // before accepting new ones
    void run();
};

}