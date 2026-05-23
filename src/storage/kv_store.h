#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <unordered_map>

#include <lru_cache.h>

namespace kvstore {

using Clock     = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;

// Metadata stored alongside each key to support TTL
// If expity is std::nullopt, the key never expires
struct EntryMeta {
    std::optional<TimePoint> expiry;
};

class KVStore {
private:
    LRUCache cache_;

    // Tracks expiry metadata per key. Keys with no TTL have no entry here.
    std::unordered_map<std::string, EntryMeta> meta_;

    // Returns true if the key exists in meta_ and its expiry has passed.
    bool is_expired(const std::string& key) const;

    // Removes a key from both the cache and meta_.
    void erase(const std::string& key);

public:
    explicit KVStore(size_t capacity);

    // Returns the value for the given key, or std::nullopt if the key does
    // not exist or has expired. Expired keys are deleted on access (lazy expiry).
    std::optional<std::string> get(const std::string& key);

    // Inserts or updates a key-value pair with no expiry.
    void set(const std::string& key, const std::string& value);

    // Inserts or updates a key-value pair that expires after ttl_seconds.
    void set(const std::string& key, const std::string& value, int ttl_seconds);

    // Deletes a key. Returns true if the key existed, false otherwise.
    bool del(const std::string& key);

    // Returns true if the key exists and has not expired.
    bool contains(const std::string& key);

    // Returns the number of keys currently in the store (including not-yet-
    // lazily-expired keys, so treat this as an approximation).
    size_t size() const;
};

}
