#include "json.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_parse_null() {
    auto val = json::JsonValue::parse("null");
    expect(val.isNull(), "expected null");
}

void test_parse_boolean() {
    auto t = json::JsonValue::parse("true");
    expect(t.isBoolean() && t.asBool() == true, "expected true");

    auto f = json::JsonValue::parse("false");
    expect(f.isBoolean() && f.asBool() == false, "expected false");
}

void test_parse_number() {
    auto n1 = json::JsonValue::parse("42");
    expect(n1.isNumber() && n1.asNumber() == 42, "expected 42");

    auto n2 = json::JsonValue::parse("-3.14");
    expect(n2.isNumber() && n2.asNumber() < -3.13, "expected -3.14");

    auto n3 = json::JsonValue::parse("1e10");
    expect(n3.isNumber() && n3.asNumber() == 1e10, "expected 1e10");
}

void test_parse_string() {
    auto s = json::JsonValue::parse("\"hello\"");
    expect(s.isString() && s.asString() == "hello", "expected hello");

    auto escaped = json::JsonValue::parse("\"line1\\nline2\"");
    expect(escaped.asString() == "line1\nline2", "expected newline");

    auto unicode = json::JsonValue::parse("\"\\u0041\"");
    expect(unicode.asString() == "A", "expected A");
}

void test_parse_array() {
    auto arr = json::JsonValue::parse("[1, 2, 3]");
    expect(arr.isArray(), "expected array");
    expect(arr.asArray().size() == 3, "expected 3 elements");
    expect(arr.asArray()[0].asNumber() == 1, "expected first element 1");

    auto empty = json::JsonValue::parse("[]");
    expect(empty.asArray().empty(), "expected empty array");
}

void test_parse_object() {
    auto obj = json::JsonValue::parse("{\"name\": \"Alice\", \"age\": 25}");
    expect(obj.isObject(), "expected object");
    expect(obj.asObject().at("name").asString() == "Alice", "expected name Alice");
    expect(obj.asObject().at("age").asNumber() == 25, "expected age 25");

    auto empty = json::JsonValue::parse("{}");
    expect(empty.asObject().empty(), "expected empty object");
}

void test_parse_nested() {
    auto nested = json::JsonValue::parse(R"({
        "users": [
            {"name": "Alice", "scores": [95, 87, 92]},
            {"name": "Bob", "scores": [88, 91, 85]}
        ]
    })");

    expect(nested.isObject(), "expected object");
    expect(nested.asObject().at("users").isArray(), "expected users array");
    expect(nested.asObject().at("users").asArray().size() == 2, "expected 2 users");

    auto& alice = nested.asObject().at("users").asArray()[0];
    expect(alice.asObject().at("name").asString() == "Alice", "expected Alice");
    expect(alice.asObject().at("scores").asArray().size() == 3, "expected 3 scores");
}

void test_stringify_null() {
    json::JsonValue val(nullptr);
    expect(val.stringify() == "null", "expected null");
}

void test_stringify_boolean() {
    json::JsonValue t(true);
    expect(t.stringify() == "true", "expected true");

    json::JsonValue f(false);
    expect(f.stringify() == "false", "expected false");
}

void test_stringify_number() {
    json::JsonValue n(42);
    expect(n.stringify() == "42", "expected 42");

    json::JsonValue d(3.14);
    expect(d.stringify() == "3.14", "expected 3.14");
}

void test_stringify_string() {
    json::JsonValue s("hello");
    expect(s.stringify() == "\"hello\"", "expected \"hello\"");
}

void test_stringify_array() {
    json::JsonArray arr = {json::JsonValue(1), json::JsonValue(2), json::JsonValue(3)};
    json::JsonValue val(arr);
    expect(val.stringify() == "[1,2,3]", "expected [1,2,3]");
}

void test_stringify_object() {
    json::JsonObject obj;
    obj["a"] = json::JsonValue(1);
    obj["b"] = json::JsonValue(2);
    json::JsonValue val(obj);
    std::string result = val.stringify();
    expect(result.find("\"a\":1") != std::string::npos, "expected a:1");
    expect(result.find("\"b\":2") != std::string::npos, "expected b:2");
}

void test_stringify_indented() {
    json::JsonArray arr = {json::JsonValue(1), json::JsonValue(2), json::JsonValue(3)};
    json::JsonValue val(arr);
    std::string indented = val.stringify(2);
    expect(indented.find("\n") != std::string::npos, "expected newlines");
}

void test_roundtrip() {
    std::string original = R"({"name":"test","values":[1,2,3],"flag":true})";
    auto parsed = json::JsonValue::parse(original);
    std::string serialized = parsed.stringify();
    auto reparsed = json::JsonValue::parse(serialized);

    expect(reparsed.asObject().at("name").asString() == "test", "expected test");
    expect(reparsed.asObject().at("values").asArray().size() == 3, "expected 3 values");
    expect(reparsed.asObject().at("flag").asBool() == true, "expected true");
}

void test_parse_error() {
    bool threw = false;
    try {
        json::JsonValue::parse("{invalid}");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "expected parse error");

    threw = false;
    try {
        json::JsonValue::parse("[1, 2,]");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "expected trailing comma error");
}

// Regression: the serializer wrote the characters of a string as they
// were, so a string with a quote or a newline in it came out as JSON
// that could not be parsed again.
void test_stringify_escapes_the_string() {
    const std::string original = "say \"hi\"\n\tand \u0001 more";
    json::JsonValue value(original);

    const std::string serialized = value.stringify();
    auto reparsed = json::JsonValue::parse(serialized);

    expect(reparsed.asString() == original,
           "expected the string to survive the roundtrip, got [" +
               reparsed.asString() + "]");
}

// Regression: a number that does not fit in an int was cast to one
// anyway, which is undefined. The undefined behaviour sanitizer found
// it on 1e10.
void test_stringify_a_number_that_is_too_big_for_an_int() {
    auto value = json::JsonValue::parse("1e10");

    // A whole number is written without a decimal point.
    expect(value.stringify() == "10000000000",
           "expected 10000000000, got " + value.stringify());

    // And it still parses back to the same number.
    auto reparsed = json::JsonValue::parse(value.stringify());
    expect(reparsed.asNumber() == 1e10, "expected 1e10");
}

// Regression: the input ended in the middle of a string, and the half
// string was handed back as if the document were complete.
void test_unterminated_string_throws() {
    bool threw = false;
    try {
        json::JsonValue::parse("\"unterminated");
    } catch (const std::runtime_error&) {
        threw = true;
    }
    expect(threw, "expected an unterminated string to be reported");
}

// Regression: a number that was cut short was accepted. std::stod stops
// at the first character it cannot read, so "1e" was read as 1, and "1."
// is a number in C but not in JSON.
void test_malformed_numbers_are_refused() {
    for (const char* text : {"1.", "1e", "01", "-", "1e+"}) {
        bool threw = false;
        try {
            json::JsonValue::parse(text);
        } catch (const std::runtime_error&) {
            threw = true;
        }
        expect(threw, std::string("expected ") + text + " to be refused");
    }

    // The numbers that are proper are still read.
    expect(json::JsonValue::parse("1").asNumber() == 1, "expected 1");
    expect(json::JsonValue::parse("1.5").asNumber() == 1.5, "expected 1.5");
    expect(json::JsonValue::parse("1e10").asNumber() == 1e10, "expected 1e10");
}

} // namespace

int main() {
    try {
        std::cout << "JSON Parser Tests:\n";
        test_parse_null();
        std::cout << "  test_parse_null: PASS\n";

        test_parse_boolean();
        std::cout << "  test_parse_boolean: PASS\n";

        test_parse_number();
        std::cout << "  test_parse_number: PASS\n";

        test_parse_string();
        std::cout << "  test_parse_string: PASS\n";

        test_parse_array();
        std::cout << "  test_parse_array: PASS\n";

        test_parse_object();
        std::cout << "  test_parse_object: PASS\n";

        test_parse_nested();
        std::cout << "  test_parse_nested: PASS\n";

        test_stringify_null();
        std::cout << "  test_stringify_null: PASS\n";

        test_stringify_boolean();
        std::cout << "  test_stringify_boolean: PASS\n";

        test_stringify_number();
        std::cout << "  test_stringify_number: PASS\n";

        test_stringify_string();
        std::cout << "  test_stringify_string: PASS\n";

        test_stringify_array();
        std::cout << "  test_stringify_array: PASS\n";

        test_stringify_object();
        std::cout << "  test_stringify_object: PASS\n";

        test_stringify_indented();
        std::cout << "  test_stringify_indented: PASS\n";

        test_roundtrip();
        std::cout << "  test_roundtrip: PASS\n";

        test_parse_error();
        test_malformed_numbers_are_refused();
        std::cout << "  test_malformed_numbers_are_refused: PASS\n";
        std::cout << "  test_parse_error: PASS\n";

        test_stringify_escapes_the_string();
        std::cout << "  test_stringify_escapes_the_string: PASS\n";

        test_stringify_a_number_that_is_too_big_for_an_int();
        std::cout << "  test_stringify_a_number_that_is_too_big_for_an_int: PASS\n";

        test_unterminated_string_throws();
        std::cout << "  test_unterminated_string_throws: PASS\n";
    } catch (const std::exception& exception) {
        std::cerr << "json_parser_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\njson_parser_tests passed" << std::endl;
    return 0;
}
