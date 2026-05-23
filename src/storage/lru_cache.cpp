#include <lru_cache.h>

namespace kvstore {

LRUCache::LRUCache(size_t capacity) : capacity_(capacity) {}

CacheEntry e{"hello", "hello"};

std::optional<std::string> LRUCache::get(const std::string& key) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return std::nullopt;
    }

    move_to_front(it->second);
    return it->second->value;
}

void LRUCache::set(const std::string& key, const std::string& value) {
    auto it = map_.find(key);
    if (it != map_.end()) {
        it->second->value = value;
        move_to_front(it->second);
        return;
    }

    if (map_.size() >= capacity_) {
        evict();
    }

    list_.push_front(CacheEntry{key, value});
    map_[key] = list_.begin();
}

bool LRUCache::del(const std::string& key) {
    auto it = map_.find(key);
    if (it == map_.end()) {
        return false;
    }

    list_.erase(it->second);
    map_.erase(key);
    return true;
}

bool LRUCache::contains(const std::string& key) const {
    return map_.find(key) != map_.end();
}

size_t LRUCache::size() const {
    return map_.size();
}

size_t LRUCache::capacity() const {
    return capacity_;
}

void LRUCache::move_to_front(std::list<CacheEntry>::iterator it) {
    list_.splice(list_.begin(), list_, it);
}

void LRUCache::evict() {
    if (list_.empty()) {
        return;
    }

    const std::string& lru_key = list_.back().key;
    map_.erase(lru_key);
    list_.pop_back();
}

} // namespace kvstore