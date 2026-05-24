cmake_minimum_required(VERSION 3.15)
project(kvstore)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include src/ so that includes like "storage/kv_store.h" resolve correctly
include_directories(src)

# Collect all source files
set(SOURCES
    src/main.cpp
    src/server/tcp_server.cpp
    src/server/client_handler.cpp
    src/storage/lru_cache.cpp
    src/storage/kv_store.cpp
    src/protocol/parser.cpp
    src/protocol/response.cpp
)

add_executable(kvstore ${SOURCES})

# Link pthread — required for std::thread and related primitives on Linux
target_link_libraries(kvstore pthread)

# Compiler warnings
target_compile_options(kvstore PRIVATE -Wall -Wextra)