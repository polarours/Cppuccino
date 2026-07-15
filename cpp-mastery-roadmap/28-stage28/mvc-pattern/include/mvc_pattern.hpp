#pragma once

#include <string>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>

namespace mvc_pattern {

struct TodoItem {
    int id;
    std::string text;
    bool done;
};

class TodoModel {
public:
    void addItem(const std::string& text) {
        items_.push_back({nextId_++, text, false});
    }

    void toggleItem(int id) {
        for (auto& item : items_) {
            if (item.id == id) {
                item.done = !item.done;
                return;
            }
        }
    }

    void removeItem(int id) {
        items_.erase(
            std::remove_if(items_.begin(), items_.end(),
                [id](const TodoItem& i) { return i.id == id; }),
            items_.end());
    }

    const std::vector<TodoItem>& items() const { return items_; }

    std::size_t size() const { return items_.size(); }

    std::size_t doneCount() const {
        return std::count_if(items_.begin(), items_.end(),
            [](const TodoItem& i) { return i.done; });
    }

private:
    std::vector<TodoItem> items_;
    int nextId_ = 1;
};

class TodoView {
public:
    using RenderFn = std::function<void(const std::string&)>;

    explicit TodoView(RenderFn render) : render_(std::move(render)) {}

    void renderList(const std::vector<TodoItem>& items) {
        for (const auto& item : items) {
            render_((item.done ? "[x] " : "[ ] ") + item.text);
        }
    }

    void renderSummary(std::size_t total, std::size_t done) {
        render_("Total: " + std::to_string(total) + ", Done: " + std::to_string(done));
    }

private:
    RenderFn render_;
};

class TodoController {
public:
    TodoController() : view_([this](const std::string& s) { output_.push_back(s); }) {}

    void addItem(const std::string& text) {
        model_.addItem(text);
    }

    void toggleItem(int id) {
        model_.toggleItem(id);
    }

    void removeItem(int id) {
        model_.removeItem(id);
    }

    void refresh() {
        output_.clear();
        view_.renderList(model_.items());
        view_.renderSummary(model_.size(), model_.doneCount());
    }

    const std::vector<std::string>& output() const { return output_; }
    const TodoModel& model() const { return model_; }

private:
    TodoModel model_;
    TodoView view_;
    std::vector<std::string> output_;
};

} // namespace mvc_pattern
