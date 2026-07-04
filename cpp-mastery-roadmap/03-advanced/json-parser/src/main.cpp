#include "json.hpp"

#include <iostream>
#include <string>

int main() {
    std::cout << "=== JSON Parser Demo ===\n\n";

    // Parse JSON
    std::string jsonStr = R"({
        "name": "Cppuccino",
        "version": 1.0,
        "features": ["docs", "examples", "pitfalls"],
        "active": true,
        "author": {
            "name": "Developer",
            "projects": [1, 2, 3]
        }
    })";

    std::cout << "Parsing JSON:\n" << jsonStr << "\n\n";

    auto value = json::JsonValue::parse(jsonStr);

    std::cout << "Parsed successfully!\n\n";

    // Access values
    std::cout << "Accessing values:\n";
    std::cout << "  name: " << value.asObject().at("name").asString() << "\n";
    std::cout << "  version: " << value.asObject().at("version").asNumber() << "\n";
    std::cout << "  active: " << std::boolalpha << value.asObject().at("active").asBool() << "\n";

    const auto& features = value.asObject().at("features").asArray();
    std::cout << "  features: [";
    for (size_t i = 0; i < features.size(); ++i) {
        if (i > 0) std::cout << ", ";
        std::cout << features[i].asString();
    }
    std::cout << "]\n\n";

    // Stringify
    std::cout << "Stringified (compact):\n";
    std::cout << value.stringify() << "\n\n";

    std::cout << "Stringified (indented):\n";
    std::cout << value.stringify(2) << "\n\n";

    // Build JSON programmatically
    std::cout << "Building JSON programmatically:\n";
    json::JsonObject newObject;
    newObject["language"] = json::JsonValue("C++");
    newObject["standard"] = json::JsonValue(17);
    newObject["features"] = json::JsonValue(json::JsonArray{
        json::JsonValue("move semantics"),
        json::JsonValue("smart pointers"),
        json::JsonValue("concepts")
    });

    json::JsonValue newValue(newObject);
    std::cout << newValue.stringify(2) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
