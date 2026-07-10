#pragma once

#include <string>
#include <vector>
#include <memory>
#include <iostream>

namespace memento_pattern {

// --- Memento ---

class EditorMemento {
public:
    EditorMemento(std::string content) : content_(std::move(content)) {}

    std::string getContent() const { return content_; }

private:
    std::string content_;
};

// --- Originator ---

class Editor {
public:
    void type(const std::string& text) {
        content_ += text;
        std::cout << "Typed: " << text << "\n";
    }

    std::string getContent() const { return content_; }

    std::shared_ptr<EditorMemento> save() const {
        return std::make_shared<EditorMemento>(content_);
    }

    void restore(std::shared_ptr<EditorMemento> memento) {
        content_ = memento->getContent();
        std::cout << "Restored: " << content_ << "\n";
    }

private:
    std::string content_;
};

// --- Caretaker ---

class EditorHistory {
public:
    void saveState(std::shared_ptr<EditorMemento> memento) {
        history_.push_back(memento);
    }

    std::shared_ptr<EditorMemento> undo() {
        if (history_.empty()) {
            return nullptr;
        }

        auto memento = history_.back();
        history_.pop_back();
        return memento;
    }

    std::size_t historySize() const { return history_.size(); }

private:
    std::vector<std::shared_ptr<EditorMemento>> history_;
};

} // namespace memento_pattern
