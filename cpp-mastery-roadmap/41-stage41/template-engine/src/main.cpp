#include "template_engine.hpp"

#include <iostream>

int main() {
    using template_engine::Value;
    const std::string tpl =
        "Hello {{name}}!{{#admin}} (admin){{/admin}}{{^admin}} (user){{/admin}}\n"
        "{{#items}}- {{title}} ({{qty}})\n{{/items}}"
        "{{#empty}}should not print{{/empty}}";

    template_engine::Context ctx;
    ctx["name"] = Value::string("Ada");
    ctx["admin"] = Value::boolean(true);

    template_engine::Context it1;
    it1["title"] = Value::string("Widget");
    it1["qty"] = Value::string("3");
    template_engine::Context it2;
    it2["title"] = Value::string("Gadget");
    it2["qty"] = Value::string("1");
    ctx["items"] = Value::fromList({it1, it2});
    ctx["empty"] = Value::boolean(false);

    std::cout << template_engine::render(tpl, ctx);
    return 0;
}
