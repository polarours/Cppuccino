// examples/template-method-demo.cpp
// Demonstrates the Template Method Pattern: a base class defines the
// algorithm skeleton, subclasses override individual steps.
// Compile: g++ -std=c++20 -Wall -Wextra -o template-method-demo template-method-demo.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace template_method {

// Abstract class defines the skeleton: fetch -> transform -> save
class DataExporter {
public:
    virtual ~DataExporter() = default;

    // Template method: final algorithm, non-virtual
    void exportData() {
        auto raw = fetch();
        auto processed = transform(raw);
        save(processed);
        std::cout << "[" << name() << "] exported " << processed.size() << " records\n";
    }

protected:
    virtual std::vector<std::string> fetch() = 0;
    virtual std::vector<std::string> transform(const std::vector<std::string>& raw) const;
    virtual void save(const std::vector<std::string>& data) const;
    virtual std::string name() const = 0;
};

// Default hooks: subclasses can override selectively
std::vector<std::string> DataExporter::transform(const std::vector<std::string>& raw) const {
    return raw;
}

void DataExporter::save(const std::vector<std::string>& data) const {
    for (const auto& row : data) std::cout << "  saved: " << row << "\n";
}

// CSV exporter: overrides fetch + name only
class CsvExporter : public DataExporter {
protected:
    std::vector<std::string> fetch() override {
        return {"id,name", "1,alice", "2,bob"};
    }
    std::string name() const override { return "CSV"; }
};

// JSON exporter: overrides every step (including a custom transform)
class JsonExporter : public DataExporter {
protected:
    std::vector<std::string> fetch() override {
        return {"{\"id\":1}", "{\"id\":2}", "{\"id\":3}"};
    }
    std::vector<std::string> transform(const std::vector<std::string>& raw) const override {
        std::vector<std::string> out;
        for (const auto& r : raw) out.push_back("  " + r);
        return out;
    }
    void save(const std::vector<std::string>& data) const override {
        std::cout << "  [json] {\n";
        for (const auto& row : data) std::cout << row << ",\n";
        std::cout << "  [json] }\n";
    }
    std::string name() const override { return "JSON"; }
};

} // namespace template_method

int main() {
    using namespace template_method;
    std::cout << "=== Template Method Demo ===\n\n";

    CsvExporter csv;
    csv.exportData();
    std::cout << "\n";

    JsonExporter json;
    json.exportData();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
