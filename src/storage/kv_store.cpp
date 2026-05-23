#include "kv_store.h"

namespace kvstore {

KVStore::KVStore(size_t capacity) : cache_(capacity) {};

std::optional<std::string> KVStore::get(const std::string& key) {
    if (is_expired(key)) {
        erase(key);
        return std::nullopt;
    }
    return cache_.get(key);
}

void KVStore::set(const std::string& key, const std::string& value) {
    cache_.set(key, value);
    meta_.erase(key); // remove any existing TTL metadata
}

void KVStore::set(const std::string& key, const std::string& value, int ttl_seconds) { 
    cache_.set(key, value);
    meta_[key] = EntryMeta{
        Clock::now() + std::chrono::seconds(ttl_seconds)
    };
}

bool KVStore::del(const std::string& key) {
    if (!cache_.contains(key)) {
        return false;
    }

    erase(key);
    return true;
}

bool KVStore::contains(const std::string& key) {
    if (is_expired(key)) {
        erase(key);
        return false;
    }

    return cache_.contains(key);
}

size_t KVStore::size() const {
    return cache_.size();
}

bool KVStore::is_expired(const std::string& key) const {
    auto it = meta_.find(key);
    if (it == meta_.end()) {
        return false; // no TTL set, never expires
    }

    const auto& expiry = it->second.expiry;

    if (!expiry.has_value()) {
        return false;
    }

    return Clock::now() > expiry.value();
}

void KVStore::erase(const std::string& key) {
    cache_.del(key);
    meta_.erase(key);
}

} // namespace kvstore