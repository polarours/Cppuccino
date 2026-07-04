#include "store_persistence.hpp"

#include <fstream>
#include <iomanip>
#include <stdexcept>

namespace key_value_store {

std::unordered_map<std::string, std::string> StorePersistence::load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for reading: " + path);
    }

    std::unordered_map<std::string, std::string> data;
    std::string key;
    std::string value;

    while (file >> std::quoted(key) >> std::quoted(value)) {
        data[key] = value;
    }

    if (!file.eof()) {
        throw std::runtime_error("Failed to parse persistence file: " + path);
    }

    return data;
}

void StorePersistence::save(
    const std::string& path,
    const std::unordered_map<std::string, std::string>& data
) {
    std::ofstream file(path, std::ios::trunc);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + path);
    }

    for (const auto& [key, value] : data) {
        file << std::quoted(key) << ' ' << std::quoted(value) << '\n';
    }
}

} // namespace key_value_store
