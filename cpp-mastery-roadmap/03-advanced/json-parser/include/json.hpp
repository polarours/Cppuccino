#pragma once

#include <string>
#include <vector>
#include <map>
#include <variant>
#include <optional>
#include <iostream>

namespace json {

class JsonValue;

using JsonArray = std::vector<JsonValue>;
using JsonObject = std::map<std::string, JsonValue>;

class JsonValue {
public:
    enum class Type {
        null,
        boolean,
        number,
        string,
        array,
        object
    };

    JsonValue();
    JsonValue(std::nullptr_t);
    JsonValue(bool value);
    JsonValue(int value);
    JsonValue(double value);
    JsonValue(const char* value);
    JsonValue(const std::string& value);
    JsonValue(const JsonArray& value);
    JsonValue(const JsonObject& value);

    Type type() const;
    bool isNull() const;
    bool isBoolean() const;
    bool isNumber() const;
    bool isString() const;
    bool isArray() const;
    bool isObject() const;

    bool asBool() const;
    double asNumber() const;
    const std::string& asString() const;
    const JsonArray& asArray() const;
    const JsonObject& asObject() const;

    JsonArray& asArray();
    JsonObject& asObject();

    static JsonValue parse(const std::string& json);
    std::string stringify(int indent = 0) const;

private:
    Type type_;
    bool boolValue_;
    double numberValue_;
    std::string stringValue_;
    JsonArray arrayValue_;
    JsonObject objectValue_;
};

class Parser {
public:
    JsonValue parse(const std::string& json);

private:
    JsonValue parseValue();
    JsonValue parseNull();
    JsonValue parseBoolean();
    JsonValue parseNumber();
    JsonValue parseString();
    JsonValue parseArray();
    JsonValue parseObject();

    void skipWhitespace();
    char peek();
    char advance();
    bool match(char expected);
    bool match(const std::string& expected);

    std::string parseStringContent();

    const std::string* input_;
    std::size_t position_;
};

std::ostream& operator<<(std::ostream& os, const JsonValue& value);

} // namespace json
