#pragma once

#include <string>
#include <map>
#include <memory>
#include <iostream>

namespace flyweight_pattern {

// --- Flyweight (shared state) ---

class Font {
public:
    Font(const std::string& name, int size, bool bold)
        : name_(name), size_(size), bold_(bold) {}

    void render(const std::string& text) const {
        std::cout << "[Font: " << name_ << " size=" << size_
                  << " bold=" << (bold_ ? "true" : "false")
                  << "] " << text << "\n";
    }

    std::string getName() const { return name_; }
    int getSize() const { return size_; }
    bool isBold() const { return bold_; }

private:
    std::string name_;
    int size_;
    bool bold_;
};

// --- Flyweight Factory ---

class FontFactory {
public:
    std::shared_ptr<Font> getFont(const std::string& name, int size, bool bold) {
        std::string key = name + "_" + std::to_string(size) + "_" + (bold ? "1" : "0");

        auto it = fonts_.find(key);
        if (it != fonts_.end()) {
            return it->second;
        }

        auto font = std::make_shared<Font>(name, size, bold);
        fonts_[key] = font;
        return font;
    }

    std::size_t size() const { return fonts_.size(); }

    void clear() { fonts_.clear(); }

private:
    std::map<std::string, std::shared_ptr<Font>> fonts_;
};

// --- Context (extrinsic state) ---

class TextCharacter {
public:
    TextCharacter(char ch, std::shared_ptr<Font> font)
        : character_(ch), font_(font) {}

    void render() const {
        font_->render(std::string(1, character_));
    }

    char getCharacter() const { return character_; }
    std::shared_ptr<Font> getFont() const { return font_; }

private:
    char character_;
    std::shared_ptr<Font> font_;
};

} // namespace flyweight_pattern
