#include "template_engine.hpp"

namespace template_engine {

namespace {

struct Segment {
    enum class Kind { Text, Var, SectionOpen, SectionClose, InvertOpen };
    Kind kind;
    std::string key;
    std::string text;  // for Kind::Text
};

// Tokenize into text/variable/section tokens (nesting handled in render loop).
std::vector<Segment> tokenize(const std::string& tpl) {
    std::vector<Segment> segs;
    std::size_t pos = 0;
    while (pos < tpl.size()) {
        std::size_t open = tpl.find("{{", pos);
        if (open == std::string::npos) {
            segs.push_back({Segment::Kind::Text, "", tpl.substr(pos)});
            break;
        }
        if (open > pos)
            segs.push_back({Segment::Kind::Text, "", tpl.substr(pos, open - pos)});
        std::size_t close = tpl.find("}}", open);
        if (close == std::string::npos) {
            segs.push_back({Segment::Kind::Text, "", tpl.substr(open)});
            break;
        }
        std::string tag = tpl.substr(open + 2, close - open - 2);
        if (tag.empty()) {
            segs.push_back({Segment::Kind::Text, "", "{{}}"});
        } else if (tag[0] == '#') {
            segs.push_back({Segment::Kind::SectionOpen, tag.substr(1), ""});
        } else if (tag[0] == '^') {
            segs.push_back({Segment::Kind::InvertOpen, tag.substr(1), ""});
        } else if (tag[0] == '/') {
            segs.push_back({Segment::Kind::SectionClose, tag.substr(1), ""});
        } else {
            segs.push_back({Segment::Kind::Var, tag, ""});
        }
        pos = close + 2;
    }
    return segs;
}

std::string lookup(const Context& ctx, const std::string& key) {
    auto it = ctx.find(key);
    if (it == ctx.end()) return "";
    const Value& v = it->second;
    return v.kind == Value::Kind::String ? v.str : "";
}

// Recursive render with a cursor; sections consume their body.
void renderRange(const std::vector<Segment>& segs, std::size_t& i,
                 const Context& ctx, std::string& out, bool stopAtClose) {
    while (i < segs.size()) {
        const auto& s = segs[i];
        if (s.kind == Segment::Kind::SectionClose) {
            if (stopAtClose) return;  // caller consumes its matching close
            ++i;                      // stray close at top level: ignore
            continue;
        }
        if (s.kind == Segment::Kind::Text) {
            out += s.text;
            ++i;
            continue;
        }
        if (s.kind == Segment::Kind::Var) {
            out += lookup(ctx, s.key);
            ++i;
            continue;
        }
        if (s.kind == Segment::Kind::SectionOpen ||
            s.kind == Segment::Kind::InvertOpen) {
            bool invert = (s.kind == Segment::Kind::InvertOpen);
            std::string key = s.key;
            ++i;
            // find matching close at same nesting level
            std::size_t bodyStart = i;
            int depth = 1;
            while (i < segs.size() && depth > 0) {
                if (segs[i].kind == Segment::Kind::SectionOpen &&
                    segs[i].key == key) ++depth;
                else if (segs[i].kind == Segment::Kind::SectionClose &&
                         segs[i].key == key) --depth;
                if (depth > 0) ++i;
            }
            std::size_t bodyEnd = i;  // points at matching close
            if (i < segs.size()) ++i;  // skip the close tag

            auto it = ctx.find(key);
            bool present = it != ctx.end();
            const Value* v = present ? &it->second : nullptr;

            if (invert) {
                bool falsy = !present || !v->truthy();
                if (falsy) {
                    std::size_t j = bodyStart;
                    renderRange(segs, j, ctx, out, true);
                }
            } else {
                if (present && v->kind == Value::Kind::List) {
                    // Render body once per item, item context chained
                    for (const auto& item : v->list) {
                        // merge: item keys shadow parent
                        Context merged = ctx;
                        for (const auto& [k, val] : item) merged[k] = val;
                        std::size_t j = bodyStart;
                        renderRange(segs, j, merged, out, true);
                    }
                } else if (present && v->truthy()) {
                    std::size_t j = bodyStart;
                    renderRange(segs, j, ctx, out, true);
                }
            }
            continue;
        }
        ++i;
    }
}

}  // namespace

std::string render(const std::string& tpl, const Context& ctx) {
    auto segs = tokenize(tpl);
    std::string out;
    std::size_t i = 0;
    renderRange(segs, i, ctx, out, false);
    return out;
}

}  // namespace template_engine
