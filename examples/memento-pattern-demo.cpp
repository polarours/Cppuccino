// examples/memento-pattern-demo.cpp
// Demonstrates Memento Pattern: Editor + History for undo/redo.
// Compile: g++ -std=c++20 -o memento-pattern-demo memento-pattern-demo.cpp

#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace memento_pattern {

class Memento {
public:
    explicit Memento(std::string state) : state_(std::move(state)) {}
    const std::string& getState() const { return state_; }
private:
    std::string state_;
};

class Editor {
public:
    void type(const std::string& text) { content_ += text; }
    void deleteChar() { if (!content_.empty()) content_.pop_back(); }
    const std::string& getContent() const { return content_; }
    std::unique_ptr<Memento> save() const {
        return std::make_unique<Memento>(content_);
    }
    void restore(const Memento& m) {
        content_ = m.getState();
    }
private:
    std::string content_;
};

class History {
public:
    void saveState(std::unique_ptr<Memento> memento) {
        history_.push_back(std::move(memento));
        if (history_.size() > maxHistory_)
            history_.erase(history_.begin());
    }

    const Memento* undo() {
        if (history_.empty()) return nullptr;
        current_ = history_.size() - 1;
        return history_[current_].get();
    }

    const Memento* redo() {
        if (current_ + 1 >= history_.size()) return nullptr;
        current_++;
        return history_[current_].get();
    }

private:
    std::vector<std::unique_ptr<Memento>> history_;
    std::size_t current_ = 0;
    static constexpr std::size_t maxHistory_ = 10;
};

} // namespace memento_pattern

int main() {
    using namespace memento_pattern;
    std::cout << "=== Memento Pattern Demo ===\n\n";

    Editor editor;
    History history;

    editor.type("Hello");
    history.saveState(editor.save());

    editor.type(" World");
    history.saveState(editor.save());

    editor.type("!");
    std::cout << "Current: \"" << editor.getContent() << "\"\n";

    std::cout << "\nUndo steps:\n";
    for (int i = 0; i < 3; ++i) {
        auto m = history.undo();
        if (!m) { std::cout << "  (no more undo)\n"; break; }
        editor.restore(*m);
        std::cout << "  Undo -> \"" << editor.getContent() << "\"\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
