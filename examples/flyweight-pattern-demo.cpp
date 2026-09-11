// examples/flyweight-pattern-demo.cpp
// Demonstrates Flyweight Pattern: Shared fonts in text editor.
// Compile: g++ -std=c++20 -o flyweight-pattern-demo flyweight-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace flyweight_pattern {

struct FontKey {
    std::string family;
    int size;
    bool bold;
    bool operator<(const FontKey& o) const {
        return std::tie(family, size, bold) < std::tie(o.family, o.size, o.bold);
    }
    bool operator==(const FontKey& o) const {
        return std::tie(family, size, bold) == std::tie(o.family, o.size, o.bold);
    }
};

std::hash<std::string> str_hash;
struct FontKeyHash {
    std::size_t operator()(const FontKey& k) const {
        return std::hash<int>{}(k.size) ^ (str_hash(k.family) << 1);
    }
};

class Font {
public:
    Font(std::string family, int size, bool bold)
        : family_(std::move(family)), size_(size), bold_(bold) {}

    void render(const std::string& text) const {
        std::cout << "[font:" << family_ << " " << size_ << (bold_ ? " BOLD" : "") << "] "
                  << text << "\n";
    }

    const FontKey& key() const { return key_; }

private:
    std::string family_;
    int size_;
    bool bold_;
    FontKey key_{family_, size_, bold_};
};

class TextCharacter {
public:
    TextCharacter(char ch, const Font& font) : ch_(ch), font_(font) {}
    void render() const {
        std::string s(1, ch_);
        font_.render(s);
    }
private:
    char ch_;
    const Font& font_;
};

class FontFactory {
public:
    Font& getFont(const std::string& family, int size, bool bold) {
        FontKey key{family, size, bold};
        auto it = cache_.find(key);
        if (it == cache_.end()) {
            auto [inserted, _] = cache_.emplace(key, Font{family, size, bold});
            return inserted->second;
        }
        return it->second;
    }

    std::size_t size() const { return cache_.size(); }

private:
    std::map<FontKey, Font> cache_;
};

} // namespace flyweight_pattern

int main() {
    using namespace flyweight_pattern;
    std::cout << "=== Flyweight Pattern Demo ===\n\n";

    FontFactory factory;

    auto& arial12  = factory.getFont("Arial", 12, false);
    auto& arial14  = factory.getFont("Arial", 14, false);
    auto& arial12b = factory.getFont("Arial", 12, true);

    std::cout << "Unique fonts: " << factory.size() << "\n\n";

    std::string text = "Hello";
    for (char ch : text) {
        TextCharacter tc(ch, arial12);
        tc.render();
    }

    std::cout << "\nBold version:\n";
    for (char ch : text) {
        TextCharacter tc(ch, arial12b);
        tc.render();
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
