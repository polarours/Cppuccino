#pragma once

#include <string>
#include <unordered_map>

namespace key_value_store {

class StorePersistence {
public:
    static std::unordered_map<std::string, std::string> load(const std::string& path);
    static void save(
        const std::string& path,
        const std::unordered_map<std::string, std::string>& data
    );
};

} // namespace key_value_store
