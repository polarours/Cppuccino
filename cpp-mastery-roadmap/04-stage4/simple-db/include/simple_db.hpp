#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <functional>
#include <memory>

namespace simple_db {

using Value = std::variant<int, double, std::string, bool, std::nullptr_t>;
using Row = std::map<std::string, Value>;
using Rows = std::vector<Row>;

class Database {
public:
    Database();

    bool createTable(const std::string& name);
    bool dropTable(const std::string& name);
    bool tableExists(const std::string& name) const;

    bool insert(const std::string& table, const Row& row);
    Rows select(const std::string& table,
                const std::string& where = "",
                const std::function<bool(const Row&)>& predicate = nullptr);
    bool update(const std::string& table,
                const std::map<std::string, Value>& updates,
                const std::function<bool(const Row&)>& predicate = nullptr);
    bool remove(const std::string& table,
                const std::function<bool(const Row&)>& predicate = nullptr);

    std::size_t count(const std::string& table) const;
    void clear(const std::string& table);

    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    const std::map<std::string, Rows>& tables() const;

private:
    std::map<std::string, Rows> tables_;
};

std::string valueToString(const Value& value);
Value stringToValue(const std::string& str);

} // namespace simple_db
