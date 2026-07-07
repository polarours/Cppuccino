#include "simple_db.hpp"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

namespace simple_db {

Database::Database() {}

bool Database::createTable(const std::string& name) {
    if (tables_.find(name) != tables_.end()) {
        return false;
    }
    tables_[name] = Rows();
    return true;
}

bool Database::dropTable(const std::string& name) {
    return tables_.erase(name) > 0;
}

bool Database::tableExists(const std::string& name) const {
    return tables_.find(name) != tables_.end();
}

bool Database::insert(const std::string& table, const Row& row) {
    auto it = tables_.find(table);
    if (it == tables_.end()) {
        return false;
    }
    it->second.push_back(row);
    return true;
}

Rows Database::select(const std::string& table,
                      const std::string& where,
                      const std::function<bool(const Row&)>& predicate) {
    Rows result;
    auto it = tables_.find(table);
    if (it == tables_.end()) {
        return result;
    }

    for (const auto& row : it->second) {
        if (predicate && !predicate(row)) {
            continue;
        }
        result.push_back(row);
    }
    return result;
}

bool Database::update(const std::string& table,
                      const std::map<std::string, Value>& updates,
                      const std::function<bool(const Row&)>& predicate) {
    auto it = tables_.find(table);
    if (it == tables_.end()) {
        return false;
    }

    bool updated = false;
    for (auto& row : it->second) {
        if (predicate && !predicate(row)) {
            continue;
        }
        for (const auto& [key, value] : updates) {
            row[key] = value;
        }
        updated = true;
    }
    return updated;
}

bool Database::remove(const std::string& table,
                      const std::function<bool(const Row&)>& predicate) {
    auto it = tables_.find(table);
    if (it == tables_.end()) {
        return false;
    }

    auto& rows = it->second;
    auto newEnd = std::remove_if(rows.begin(), rows.end(),
        [&predicate](const Row& row) {
            return predicate && predicate(row);
        });

    bool removed = (newEnd != rows.end());
    rows.erase(newEnd, rows.end());
    return removed;
}

std::size_t Database::count(const std::string& table) const {
    auto it = tables_.find(table);
    return (it != tables_.end()) ? it->second.size() : 0;
}

void Database::clear(const std::string& table) {
    auto it = tables_.find(table);
    if (it != tables_.end()) {
        it->second.clear();
    }
}

bool Database::save(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    for (const auto& [tableName, rows] : tables_) {
        file << "TABLE:" << tableName << "\n";
        for (const auto& row : rows) {
            file << "ROW";
            for (const auto& [key, value] : row) {
                file << "|" << key << "=" << valueToString(value);
            }
            file << "\n";
        }
    }
    return true;
}

bool Database::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    tables_.clear();
    std::string line;
    std::string currentTable;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        if (line.substr(0, 6) == "TABLE:") {
            currentTable = line.substr(6);
            tables_[currentTable] = Rows();
        } else if (line.find("ROW") == 0 && !currentTable.empty()) {
            Row row;
            std::string rowContent = line.substr(4);
            std::istringstream stream(rowContent);
            std::string token;

            while (std::getline(stream, token, '|')) {
                if (token.empty()) continue;
                size_t eq = token.find('=');
                if (eq != std::string::npos) {
                    std::string key = token.substr(0, eq);
                    std::string value = token.substr(eq + 1);
                    row[key] = stringToValue(value);
                }
            }
            tables_[currentTable].push_back(row);
        }
    }
    return true;
}

const std::map<std::string, Rows>& Database::tables() const {
    return tables_;
}

std::string valueToString(const Value& value) {
    return std::visit([](const auto& v) -> std::string {
        using T = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<T, std::nullptr_t>) {
            return "NULL";
        } else if constexpr (std::is_same_v<T, bool>) {
            return v ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return v;
        } else {
            return std::to_string(v);
        }
    }, value);
}

Value stringToValue(const std::string& str) {
    if (str == "NULL") return nullptr;
    if (str == "true") return true;
    if (str == "false") return false;

    try {
        size_t pos;
        int i = std::stoi(str, &pos);
        if (pos == str.size()) return i;
    } catch (...) {}

    try {
        size_t pos;
        double d = std::stod(str, &pos);
        if (pos == str.size()) return d;
    } catch (...) {}

    return str;
}

} // namespace simple_db
