#include "json.hpp"

#include <sstream>
#include <stdexcept>
#include <cmath>

namespace json {

// --- JsonValue ---

JsonValue::JsonValue()
    : type_(Type::null), boolValue_(false), numberValue_(0) {}

JsonValue::JsonValue(std::nullptr_t)
    : type_(Type::null), boolValue_(false), numberValue_(0) {}

JsonValue::JsonValue(bool value)
    : type_(Type::boolean), boolValue_(value), numberValue_(0) {}

JsonValue::JsonValue(int value)
    : type_(Type::number), boolValue_(false), numberValue_(static_cast<double>(value)) {}

JsonValue::JsonValue(double value)
    : type_(Type::number), boolValue_(false), numberValue_(value) {}

JsonValue::JsonValue(const char* value)
    : type_(Type::string), boolValue_(false), numberValue_(0), stringValue_(value) {}

JsonValue::JsonValue(const std::string& value)
    : type_(Type::string), boolValue_(false), numberValue_(0), stringValue_(value) {}

JsonValue::JsonValue(const JsonArray& value)
    : type_(Type::array), boolValue_(false), numberValue_(0), arrayValue_(value) {}

JsonValue::JsonValue(const JsonObject& value)
    : type_(Type::object), boolValue_(false), numberValue_(0), objectValue_(value) {}

JsonValue::Type JsonValue::type() const { return type_; }
bool JsonValue::isNull() const { return type_ == Type::null; }
bool JsonValue::isBoolean() const { return type_ == Type::boolean; }
bool JsonValue::isNumber() const { return type_ == Type::number; }
bool JsonValue::isString() const { return type_ == Type::string; }
bool JsonValue::isArray() const { return type_ == Type::array; }
bool JsonValue::isObject() const { return type_ == Type::object; }

bool JsonValue::asBool() const {
    if (type_ != Type::boolean) throw std::runtime_error("Not a boolean");
    return boolValue_;
}

double JsonValue::asNumber() const {
    if (type_ != Type::number) throw std::runtime_error("Not a number");
    return numberValue_;
}

const std::string& JsonValue::asString() const {
    if (type_ != Type::string) throw std::runtime_error("Not a string");
    return stringValue_;
}

const JsonArray& JsonValue::asArray() const {
    if (type_ != Type::array) throw std::runtime_error("Not an array");
    return arrayValue_;
}

const JsonObject& JsonValue::asObject() const {
    if (type_ != Type::object) throw std::runtime_error("Not an object");
    return objectValue_;
}

JsonArray& JsonValue::asArray() {
    if (type_ != Type::array) throw std::runtime_error("Not an array");
    return arrayValue_;
}

JsonObject& JsonValue::asObject() {
    if (type_ != Type::object) throw std::runtime_error("Not an object");
    return objectValue_;
}

JsonValue JsonValue::parse(const std::string& json) {
    Parser parser;
    return parser.parse(json);
}

std::string JsonValue::stringify(int indent) const {
    std::ostringstream oss;

    switch (type_) {
        case Type::null:
            oss << "null";
            break;
        case Type::boolean:
            oss << (boolValue_ ? "true" : "false");
            break;
        case Type::number: {
            if (numberValue_ == static_cast<int>(numberValue_)) {
                oss << static_cast<int>(numberValue_);
            } else {
                oss << numberValue_;
            }
            break;
        }
        case Type::string:
            oss << "\"" << stringValue_ << "\"";
            break;
        case Type::array:
            oss << "[";
            for (size_t i = 0; i < arrayValue_.size(); ++i) {
                if (i > 0) oss << ",";
                if (indent > 0) oss << "\n" << std::string(indent * 2, ' ');
                oss << arrayValue_[i].stringify(indent > 0 ? indent + 1 : 0);
            }
            if (indent > 0 && !arrayValue_.empty()) {
                oss << "\n" << std::string((indent - 1) * 2, ' ');
            }
            oss << "]";
            break;
        case Type::object:
            oss << "{";
            {
                bool first = true;
                for (const auto& [key, value] : objectValue_) {
                    if (!first) oss << ",";
                    if (indent > 0) oss << "\n" << std::string(indent * 2, ' ');
                    oss << "\"" << key << "\":" << value.stringify(indent > 0 ? indent + 1 : 0);
                    first = false;
                }
            }
            if (indent > 0 && !objectValue_.empty()) {
                oss << "\n" << std::string((indent - 1) * 2, ' ');
            }
            oss << "}";
            break;
    }

    return oss.str();
}

// --- Parser ---

JsonValue Parser::parse(const std::string& json) {
    input_ = &json;
    position_ = 0;
    JsonValue result = parseValue();
    skipWhitespace();
    if (position_ < input_->size()) {
        throw std::runtime_error("Unexpected characters after JSON value");
    }
    return result;
}

JsonValue Parser::parseValue() {
    skipWhitespace();
    char c = peek();
    switch (c) {
        case 'n': return parseNull();
        case 't': case 'f': return parseBoolean();
        case '"': return parseString();
        case '[': return parseArray();
        case '{': return parseObject();
        default:
            if (c == '-' || (c >= '0' && c <= '9')) {
                return parseNumber();
            }
            throw std::runtime_error(std::string("Unexpected character: ") + c);
    }
}

JsonValue Parser::parseNull() {
    if (!match("null")) throw std::runtime_error("Expected 'null'");
    return JsonValue(nullptr);
}

JsonValue Parser::parseBoolean() {
    if (match("true")) return JsonValue(true);
    if (match("false")) return JsonValue(false);
    throw std::runtime_error("Expected 'true' or 'false'");
}

JsonValue Parser::parseNumber() {
    std::size_t start = position_;
    if (peek() == '-') advance();
    while (position_ < input_->size() && (*input_)[position_] >= '0' && (*input_)[position_] <= '9') {
        advance();
    }
    if (position_ < input_->size() && (*input_)[position_] == '.') {
        advance();
        while (position_ < input_->size() && (*input_)[position_] >= '0' && (*input_)[position_] <= '9') {
            advance();
        }
    }
    if (position_ < input_->size() && ((*input_)[position_] == 'e' || (*input_)[position_] == 'E')) {
        advance();
        if (position_ < input_->size() && ((*input_)[position_] == '+' || (*input_)[position_] == '-')) {
            advance();
        }
        while (position_ < input_->size() && (*input_)[position_] >= '0' && (*input_)[position_] <= '9') {
            advance();
        }
    }
    std::string numStr = input_->substr(start, position_ - start);
    return JsonValue(std::stod(numStr));
}

JsonValue Parser::parseString() {
    return JsonValue(parseStringContent());
}

JsonValue Parser::parseArray() {
    if (!match('[')) throw std::runtime_error("Expected '['");
    JsonArray array;

    skipWhitespace();
    if (peek() == ']') {
        advance();
        return JsonValue(array);
    }

    while (true) {
        array.push_back(parseValue());
        skipWhitespace();
        if (peek() == ']') {
            advance();
            break;
        }
        if (!match(',')) throw std::runtime_error("Expected ',' or ']'");
    }

    return JsonValue(array);
}

JsonValue Parser::parseObject() {
    if (!match('{')) throw std::runtime_error("Expected '{'");
    JsonObject object;

    skipWhitespace();
    if (peek() == '}') {
        advance();
        return JsonValue(object);
    }

    while (true) {
        skipWhitespace();
        std::string key = parseStringContent();
        skipWhitespace();
        if (!match(':')) throw std::runtime_error("Expected ':'");
        JsonValue value = parseValue();
        object[key] = value;
        skipWhitespace();
        if (peek() == '}') {
            advance();
            break;
        }
        if (!match(',')) throw std::runtime_error("Expected ',' or '}'");
    }

    return JsonValue(object);
}

void Parser::skipWhitespace() {
    while (position_ < input_->size()) {
        char c = (*input_)[position_];
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            advance();
        } else {
            break;
        }
    }
}

char Parser::peek() {
    if (position_ >= input_->size()) return '\0';
    return (*input_)[position_];
}

char Parser::advance() {
    if (position_ >= input_->size()) return '\0';
    return (*input_)[position_++];
}

bool Parser::match(char expected) {
    skipWhitespace();
    if (peek() == expected) {
        advance();
        return true;
    }
    return false;
}

bool Parser::match(const std::string& expected) {
    skipWhitespace();
    if (input_->compare(position_, expected.size(), expected) == 0) {
        position_ += expected.size();
        return true;
    }
    return false;
}

std::string Parser::parseStringContent() {
    if (!match('"')) throw std::runtime_error("Expected '\"'");
    std::string result;

    while (position_ < input_->size()) {
        char c = advance();
        if (c == '"') break;
        if (c == '\\') {
            c = advance();
            switch (c) {
                case '"': result += '"'; break;
                case '\\': result += '\\'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                case 'u': {
                    std::string hex;
                    for (int i = 0; i < 4; ++i) hex += advance();
                    int codepoint = std::stoi(hex, nullptr, 16);
                    if (codepoint < 0x80) {
                        result += static_cast<char>(codepoint);
                    } else if (codepoint < 0x800) {
                        result += static_cast<char>(0xC0 | (codepoint >> 6));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    } else {
                        result += static_cast<char>(0xE0 | (codepoint >> 12));
                        result += static_cast<char>(0x80 | ((codepoint >> 6) & 0x3F));
                        result += static_cast<char>(0x80 | (codepoint & 0x3F));
                    }
                    break;
                }
                default: result += c;
            }
        } else {
            result += c;
        }
    }

    return result;
}

std::ostream& operator<<(std::ostream& os, const JsonValue& value) {
    os << value.stringify();
    return os;
}

} // namespace json
