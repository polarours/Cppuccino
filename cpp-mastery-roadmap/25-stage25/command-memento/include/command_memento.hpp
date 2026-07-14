#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <iostream>

namespace command_memento {

class Editor;

class Memento {
public:
    virtual ~Memento() = default;
protected:
    friend class Editor;
};

class EditorMemento : public Memento {
public:
    explicit EditorMemento(std::string state) : state_(std::move(state)) {}
    const std::string& state() const { return state_; }
private:
    std::string state_;
};

class Editor {
public:
    void type(const std::string& text) {
        content_ += text;
    }

    void remove(std::size_t count) {
        if (count >= content_.size()) {
            content_.clear();
        } else {
            content_ = content_.substr(0, content_.size() - count);
        }
    }

    std::string content() const { return content_; }

    std::shared_ptr<EditorMemento> save() const {
        return std::make_shared<EditorMemento>(content_);
    }

    void restore(const std::shared_ptr<EditorMemento>& memento) {
        content_ = memento->state();
    }

private:
    std::string content_;
};

class History {
public:
    void push(std::shared_ptr<EditorMemento> memento) {
        snapshots_.push_back(std::move(memento));
    }

    std::shared_ptr<EditorMemento> pop() {
        if (snapshots_.empty()) return nullptr;
        auto m = snapshots_.back();
        snapshots_.pop_back();
        return m;
    }

    std::size_t size() const { return snapshots_.size(); }
    void clear() { snapshots_.clear(); }

private:
    std::vector<std::shared_ptr<EditorMemento>> snapshots_;
};

} // namespace command_memento
