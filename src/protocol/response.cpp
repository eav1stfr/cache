#include "response.h"

namespace kvstore {

std::string Response::value(const std::string& val) {
    return val + "\n";
}

std::string Response::not_found() {
    return "NOT FOUND\n";
}

std::string Response::ok() {
    return "OK\n";
}

std::string Response::deleted() {
    return "DELETED\n";
}

std::string Response::error(const std::string& message) {
    return "ERROR " + message + "\n";
}

std::string Response::from_get(const std::optional<std::string>& result) {
    if (result.has_value()) {
        return value(result.value());
    }
    return not_found();
}

} // namespace kvstore