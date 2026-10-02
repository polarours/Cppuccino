#pragma once

#include <map>
#include <string>
#include <vector>

namespace template_engine {

struct Value;
using Context = std::map<std::string, Value>;
using List = std::vector<Context>;

struct Value {
    enum class Kind { String, Bool, List };
    Kind kind = Kind::String;
    std::string str;
    bool flag = false;
    List list;

    static Value string(std::string s) {
        Value v; v.kind = Kind::String; v.str = std::move(s); return v;
    }
    static Value boolean(bool b) {
        Value v; v.kind = Kind::Bool; v.flag = b; return v;
    }
    static Value fromList(List l) {
        Value v; v.kind = Kind::List; v.list = std::move(l); return v;
    }

    bool truthy() const {
        if (kind == Kind::Bool) return flag;
        if (kind == Kind::List) return !list.empty();
        return !str.empty();
    }
};

// Minimal mustache-like engine:
//   {{name}}              - variable (missing key -> empty string)
//   {{#key}}..{{/key}}    - section: rendered only if truthy; for a list,
//                           rendered once per item with the item as context
//   {{^key}}..{{/key}}    - inverted section: rendered only if falsy
// No HTML escaping, no partials, no comments (documented non-goals).
std::string render(const std::string& tpl, const Context& ctx);

}  // namespace template_engine
