#include "template_engine.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

using template_engine::Value;

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_variable() {
    template_engine::Context ctx;
    ctx["who"] = Value::string("world");
    expect(template_engine::render("hi {{who}}", ctx) == "hi world", "substitution");
    expect(template_engine::render("hi {{nope}}", ctx) == "hi ", "missing -> empty");
}

void test_bool_section() {
    template_engine::Context ctx;
    ctx["on"] = Value::boolean(true);
    ctx["off"] = Value::boolean(false);
    expect(template_engine::render("[{{#on}}yes{{/on}}]", ctx) == "[yes]", "truthy renders");
    expect(template_engine::render("[{{#off}}yes{{/off}}]", ctx) == "[]", "falsy skips");
    expect(template_engine::render("[{{^off}}no{{/off}}]", ctx) == "[no]", "inverted renders");
    expect(template_engine::render("[{{^on}}no{{/on}}]", ctx) == "[]", "inverted skips");
}

void test_list_section() {
    template_engine::Context ctx;
    template_engine::Context a; a["x"] = Value::string("1");
    template_engine::Context b; b["x"] = Value::string("2");
    ctx["rows"] = Value::fromList({a, b});
    expect(template_engine::render("{{#rows}}[{{x}}]{{/rows}}", ctx) == "[1][2]",
           "renders once per item");
}

void test_nested_sections() {
    template_engine::Context ctx;
    ctx["outer"] = Value::boolean(true);
    ctx["inner"] = Value::boolean(false);
    expect(template_engine::render("{{#outer}}A{{#inner}}B{{/inner}}C{{/outer}}", ctx)
               == "AC", "inner false inside true outer");
    expect(template_engine::render("{{#outer}}{{^inner}}D{{/inner}}{{/outer}}", ctx)
               == "D", "inverted inside section");
}

void test_unclosed_variable_stays_literal() {
    template_engine::Context ctx;
    expect(template_engine::render("a {{ b", ctx) == "a {{ b", "unclosed tag literal");
}

void test_section_close_without_open_ignored() {
    template_engine::Context ctx;
    expect(template_engine::render("x{{/nope}}y", ctx) == "xy", "stray close ignored");
}

}  // namespace

int main() {
    try {
        test_variable();
        test_bool_section();
        test_list_section();
        test_nested_sections();
        test_unclosed_variable_stays_literal();
        test_section_close_without_open_ignored();
        std::cout << "All template_engine tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
