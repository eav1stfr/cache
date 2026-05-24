# kvstore

A lightweight in-memory key-value store written in C++, inspired by Redis. Supports basic get/set/delete operations, TTL-based expiry, and LRU eviction over a TCP connection using a simple text protocol.

---

## Features

- `SET key value` — store a key-value pair
- `SET key value <ttl>` — store a key-value pair that expires after `<ttl>` seconds
- `GET key` — retrieve a value by key; returns `NOT FOUND` if missing or expired
- `DEL key` — delete a key explicitly
- **LRU eviction** — when the store reaches max capacity, the least recently used key is evicted automatically
- **Lazy TTL expiry** — expired keys are checked and removed on access, no background thread required
- **TCP server** — single-threaded, handles one client at a time over a simple newline-delimited text protocol

---

## Project Structure

```
kvstore/
├── CMakeLists.txt
├── README.md
│
├── src/
│   ├── main.cpp
│   │
│   ├── server/
│   │   ├── tcp_server.cpp / tcp_server.h       # socket lifecycle: bind, listen, accept loop
│   │   └── client_handler.cpp / client_handler.h  # per-connection read, parse, respond
│   │
│   ├── storage/
│   │   ├── lru_cache.cpp / lru_cache.h         # pure LRU data structure (unordered_map + list)
│   │   └── kv_store.cpp / kv_store.h           # wraps LRU cache, adds TTL logic
│   │
│   ├── protocol/
│   │   ├── parser.cpp / parser.h               # parses raw text into Command structs
│   │   └── response.cpp / response.h           # builds response strings
│
└── scripts/
    ├── client.py                               # simple Python client to interact with the server
```

---

## Architecture

### Storage layer

`LRUCache` is a pure data structure with no awareness of TTL. It maintains an `unordered_map` from key to list iterator, and a `std::list` to track access order. On every `get` or `set`, the accessed key is moved to the front of the list. When capacity is exceeded, the key at the back is evicted.

`KVStore` wraps `LRUCache` and adds TTL logic. Each entry stores an optional expiry timestamp. On `get`, the store checks whether the key has expired before returning a value. If expired, the key is deleted and `NOT FOUND` is returned.

### Protocol

The protocol is newline-delimited plain text. Each command is a single line:

```
SET foo bar\n
SET foo bar 60\n
GET foo\n
DEL foo\n
```

Responses are single lines:

```
OK\n
bar\n
NOT FOUND\n
DELETED\n
ERROR <message>\n
```

The `parser` module splits the raw line into tokens and returns a `Command` struct. The `response` module formats the result back into a string.

### Server

`TCPServer` handles the socket lifecycle: creates the socket, binds to a port, and runs an accept loop. For each accepted connection, it delegates to `ClientHandler`, which reads data in a loop (handling partial `recv()` reads), passes complete lines to the parser, calls into `KVStore`, and writes the response back.

---

## Build

Requires CMake 3.15+ and a C++17 compiler.

```bash
mkdir build && cd build
cmake ..
make
```

Run the server:

```bash
./kvstore --port 6379 --capacity 1000
```

---

## Usage

Connect with the Python client:

```bash
python scripts/client.py
```

```bash
SET foo bar
GET foo
SET session abc123 30
GET session
DEL foo
```


## Design Decisions

**Why lazy TTL expiry instead of a background thread?**
A background expiry thread adds concurrency complexity (mutexes, condition variables) without meaningful benefit for a single-threaded server. Lazy expiry on access is simpler, correct, and sufficient at this scale.

**Why LRU over LFU?**
LRU is simpler to implement with O(1) operations using a doubly-linked list and hash map. LFU provides better eviction accuracy but requires more complex bookkeeping. For a portfolio project, LRU is the right tradeoff.

**Why a custom text protocol over RESP?**
Implementing a subset of Redis's RESP protocol would add parsing complexity without adding clarity to the project. A plain newline-delimited protocol is easier to test manually and easier to explain.

**Why single-threaded?**
Concurrency is the most common source of bugs in systems software. A single-threaded design eliminates race conditions entirely, keeps the codebase focused on the core data structure and networking logic, and is still sufficient to demonstrate the concepts.