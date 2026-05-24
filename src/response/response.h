#pragma once

#include <string>
#include <optional>

namespace kvstore {

class Response {
public:
    // Returned when a GET hits an existing, non-expired key.
    static std::string value(const std::string& val);

    // Returned when a GET misses (key not found or expired).
    static std::string not_found();

    // Returned after a successful SET.
    static std::string ok();

    // Returned after a successful DEL.
    static std::string deleted();

    // Returned when the command could not be parsed or arguments are invalid.
    static std::string error(const std::string& message);

    // Convenience: wraps an optional GET result into either value() or not_found().
    static std::string from_get(const std::optional<std::string>& result);
};

} // namespace kvstore