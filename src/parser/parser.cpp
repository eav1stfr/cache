#include <parser.h>

#include <sstream>
#include <algorithm>
#include <cctype>

namespace kvstore {

Command Parser::parse(const std::string& line) {
    std::vector<std::string> tokens = tokenize(line);

    if (tokens.empty()) {
        return Command{CommandType::UNKNOWN, {}};
    }

    // Normalize command name to uppercase
    std::string command_token = tokens[0];
    std::transform(command_token.begin(), command_token.end(), command_token.begin(), ::toupper);

    CommandType type = resolve_type(command_token);
    if (type == CommandType::UNKNOWN) {
        return Command{CommandType::UNKNOWN, {}};
    }

    // Args are everything after the command name
    std::vector<std::string> args(tokens.begin()+1, tokens.end());

    if (!validate(type, args)) {
        return Command{CommandType::UNKNOWN, {}};
    }

    return Command{type, args};
}

std::vector<std::string> Parser::tokenize(const std::string& line) {
    std::vector<std::string> tokens;
    std::istringstream stream(line);
    std::string token;

    while (stream >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

CommandType Parser::resolve_type(const std::string& token) {
    if (token == "GET") return CommandType::GET;
    if (token == "SET") return CommandType::SET;
    if (token == "DEL") return CommandType::DEL;
    return CommandType::UNKNOWN;
}

bool Parser::validate(CommandType type, const std::vector<std::string>& args) {
    switch (type) {
        case CommandType::GET:
            return args.size() == 1;
        case CommandType::SET:
            return args.size() == 2 || args.size() == 3;
        case CommandType::DEL:
            return args.size() == 1;
        default:
            return false; 
    }
}

} // namespace kvstore