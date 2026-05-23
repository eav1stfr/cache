#pragma once

#include <string>
#include <list>
#include <unordered_map>
#include <optional>

namespace kvstore {

// A single entry in the cache: stores the key and value together
// so that on eviction we can remove the key from the map using only
// the back of the list (without a separate reverse lookup).
struct CacheEntry {
    std::string key;
    std::string value;
};

class LRUCache {
private:
    size_t capacity_;

    // Ordered from most recently used (front) to least recently used (back).
    std::list<CacheEntry> list_;

    // Maps each key to its position in the list for O(1) access.
    std::unordered_map<std::string, std::list<CacheEntry>::iterator> map_;

    // Moves an existing list iterator to the front (most recently used).
    void move_to_front(std::list<CacheEntry>::iterator it);

    // Evicts the least recently used entry (back of the list).
    void evict();
    
public:
    explicit LRUCache(size_t capacity);

    // Returns the value for the given key, or std::nullopt if not found.
    // Marks the key as most recently used on hit.
    std::optional<std::string> get(const std::string& key);

    // Inserts or updates a key-value pair.
    // If the key already exists, its value is updated and it is moved
    // to the front (most recently used).
    // If at capacity, the least recently used entry is evicted first.
    void set(const std::string& key, const std::string& value);

    // Removes a key from the cache.
    // Returns true if the key existed, false otherwise.
    bool del(const std::string& key);

    // Returns true if the key exists in the cache.
    bool contains(const std::string& key) const;

    // Returns the current number of entries in the cache.
    size_t size() const;

    // Returns the maximum capacity of the cache.
    size_t capacity() const;
};

} 