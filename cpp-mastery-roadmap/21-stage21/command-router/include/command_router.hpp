#pragma once

#include <string>
#include <map>
#include <functional>
#include <iostream>

namespace command_router {

using CommandHandler = std::function<std::string(const std::string&)>;

class CommandRouter {
public:
    void addRoute(const std::string& command, CommandHandler handler) {
        routes_[command] = handler;
    }

    std::string route(const std::string& command, const std::string& args = "") {
        auto it = routes_.find(command);
        if (it != routes_.end()) {
            return it->second(args);
        }
        return "Unknown command: " + command;
    }

    bool hasRoute(const std::string& command) const {
        return routes_.find(command) != routes_.end();
    }

    std::size_t routeCount() const {
        return routes_.size();
    }

private:
    std::map<std::string, CommandHandler> routes_;
};

} // namespace command_router
