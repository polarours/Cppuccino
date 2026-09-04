// examples/mvc-pattern-demo.cpp
// Demonstrates MVC Pattern with TodoModel / TodoView / TodoController.
// Compile: g++ -std=c++20 -o mvc-pattern-demo mvc-pattern-demo.cpp

#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

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
        for (auto& item : items_)
            if (item.id == id) { item.done = !item.done; return; }
    }
    void removeItem(int id) {
        items_.erase(std::remove_if(items_.begin(), items_.end(),
            [id](const TodoItem& i){ return i.id == id; }), items_.end());
    }
    const std::vector<TodoItem>& items() const { return items_; }
    std::size_t doneCount() const {
        return std::count_if(items_.begin(), items_.end(),
            [](const TodoItem& i){ return i.done; });
    }
private:
    std::vector<TodoItem> items_;
    int nextId_ = 1;
};

class TodoView {
public:
    using RenderFn = std::function<void(const std::string&)>;
    explicit TodoView(RenderFn r) : render_(std::move(r)) {}
    void renderList(const std::vector<TodoItem>& items) const {
        for (const auto& item : items)
            render_((item.done ? "[x] " : "[ ] ") + item.text);
    }
    void renderSummary(std::size_t total, std::size_t done) const {
        render_("Total: " + std::to_string(total) + ", Done: " + std::to_string(done));
    }
private:
    RenderFn render_;
};

class TodoController {
public:
    TodoController() : view_([this](const std::string& s){ output_.push_back(s); }) {}
    void addItem(const std::string& text) { model_.addItem(text); }
    void toggleItem(int id) { model_.toggleItem(id); }
    void removeItem(int id) { model_.removeItem(id); }
    void refresh() {
        output_.clear();
        view_.renderList(model_.items());
        view_.renderSummary(model_.items().size(), model_.doneCount());
    }
    const std::vector<std::string>& output() const { return output_; }
private:
    TodoModel model_;
    TodoView view_;
    std::vector<std::string> output_;
};

} // namespace mvc_pattern

int main() {
    using namespace mvc_pattern;
    std::cout << "=== MVC Pattern Demo ===\n\n";

    TodoController ctrl;
    ctrl.addItem("Buy milk");
    ctrl.addItem("Walk dog");
    ctrl.addItem("Write code");
    ctrl.toggleItem(1);
    ctrl.refresh();

    for (const auto& line : ctrl.output())
        std::cout << line << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
