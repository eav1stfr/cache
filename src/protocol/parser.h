#pragma once

#include <string>
#include <vector>
#include <optional>

namespace kvstore {

enum class CommandType {
    GET,
    SET,
    DEL,
    UNKNOWN
};

struct Command {
    CommandType type;
    std::vector<std::string> args; // arguments after the command name

    bool is_valid() const { return type != CommandType::UNKNOWN; }
    const std::string& arg(size_t index) const { return args.at(index); }
    size_t arg_count() const { return args.size(); }
};

class Parser {
private:
    // splits a string by whitespace into tokens
    static std::vector<std::string> tokenize(const std::string& line);

    // maps a ocmmand name string to a CommandType
    static CommandType resolve_type(const std::string& token);

    // validates argument count for a given command type
    // SET requires 2 or 3 args (key, value, optional ttl)
    // GET requires 1 arg (key)
    // DEL requires 1 arg (key)
    static bool validate(CommandType type, const std::vector<std::string>& args);

public:
    // Parses a single raw line (e.g., "SET foo bar 60") into a Command
    // Leading / trailing whitespaces is stripped, the line must not include new line char
    // Returns a Command with type UNKNOWN if the input is empty or unrecognized
    static Command parse(const std::string& line);
};

}