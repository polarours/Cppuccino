// examples/builder-demo.cpp
// Demonstrates the Builder Pattern: step-by-step construction of a complex
// object, with a director for common configurations.
// Compile: g++ -std=c++20 -Wall -Wextra -o builder-demo builder-demo.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace builder_demo {

struct Query {
    std::string table;
    std::vector<std::string> columns;
    std::vector<std::string> where;
    std::string orderBy;
    int limit = -1;

    std::string str() const {
        std::string s = "SELECT ";
        for (size_t i = 0; i < columns.size(); ++i) {
            s += columns[i];
            if (i + 1 < columns.size()) s += ", ";
        }
        s += " FROM " + table;
        if (!where.empty()) {
            s += " WHERE ";
            for (size_t i = 0; i < where.size(); ++i) {
                s += where[i];
                if (i + 1 < where.size()) s += " AND ";
            }
        }
        if (!orderBy.empty()) s += " ORDER BY " + orderBy;
        if (limit >= 0) s += " LIMIT " + std::to_string(limit);
        return s;
    }
};

// Builder interface
class QueryBuilder {
public:
    virtual ~QueryBuilder() = default;
    virtual QueryBuilder& from(const std::string& table) = 0;
    virtual QueryBuilder& select(const std::string& col) = 0;
    virtual QueryBuilder& where(const std::string& cond) = 0;
    virtual QueryBuilder& orderBy(const std::string& col) = 0;
    virtual QueryBuilder& limit(int n) = 0;
    virtual Query build() const = 0;
};

class SqlQueryBuilder : public QueryBuilder {
public:
    QueryBuilder& from(const std::string& table) override { q_.table = table; return *this; }
    QueryBuilder& select(const std::string& col) override { q_.columns.push_back(col); return *this; }
    QueryBuilder& where(const std::string& cond) override { q_.where.push_back(cond); return *this; }
    QueryBuilder& orderBy(const std::string& col) override { q_.orderBy = col; return *this; }
    QueryBuilder& limit(int n) override { q_.limit = n; return *this; }
    Query build() const override { return q_; }
private:
    Query q_;
};

// Director: reusable recipes
class QueryDirector {
public:
    static Query allUsers(SqlQueryBuilder& b) {
        return b.from("users").select("id").select("name").select("email")
                .orderBy("name").build();
    }
    static Query topItems(SqlQueryBuilder& b, int n) {
        return b.from("items").select("id").select("title")
                .where("stock > 0").orderBy("created_at").limit(n).build();
    }
};

} // namespace builder_demo

int main() {
    using namespace builder_demo;
    std::cout << "=== Builder Pattern Demo ===\n\n";

    SqlQueryBuilder b1;
    std::cout << QueryDirector::allUsers(b1).str() << "\n\n";

    SqlQueryBuilder b2;
    std::cout << QueryDirector::topItems(b2, 10).str() << "\n\n";

    // Direct step-by-step use (fluent interface)
    SqlQueryBuilder b3;
    Query q = b3.from("orders").select("*").where("total > 100").where("status = 'paid'")
                .limit(5).build();
    std::cout << q.str() << "\n\n";

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
