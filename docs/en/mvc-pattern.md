# MVC Pattern

## What Is the MVC Pattern?

MVC (Model-View-Controller) is an architectural pattern that splits an
application into three core components: the model, the view, and the
controller.

> **Core idea**: separate concerns so the code stays maintainable and
> extensible.

## Pattern Components

### Model

- Owns the application's data and business logic
- Depends on neither View nor Controller
- Notifies observers when the data changes

### View

- Renders the data
- Gets its data from the Model
- Contains no business logic

### Controller

- Handles user input
- Updates the Model
- Selects the appropriate View

## Implementation

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>

// Model: the data layer
class TodoModel {
public:
    struct Item {
        int id;
        std::string text;
        bool completed;
    };

    void addItem(const std::string& text) {
        items_.push_back({next_id_++, text, false});
        notifyObservers();
    }

    void toggleItem(int id) {
        for (auto& item : items_) {
            if (item.id == id) {
                item.completed = !item.completed;
            }
        }
        notifyObservers();
    }

    const std::vector<Item>& getItems() const { return items_; }

    using Observer = std::function<void()>;
    void addObserver(Observer obs) { observers_.push_back(std::move(obs)); }

private:
    void notifyObservers() {
        for (auto& obs : observers_) obs();
    }

    std::vector<Item> items_;
    int next_id_ = 1;
    std::vector<Observer> observers_;
};

// View: the presentation layer
class TodoView {
public:
    void render(const TodoModel& model) const {
        std::cout << "=== Todo List ===\n";
        for (const auto& item : model.getItems()) {
            std::cout << "[" << (item.completed ? "x" : " ") << "] "
                      << item.id << ": " << item.text << "\n";
        }
        std::cout << "Total: " << model.getItems().size() << " items\n";
    }
};

// Controller: the control layer
class TodoController {
public:
    TodoController(TodoModel& model, TodoView& view)
        : model_(model), view_(view) {}

    void run() {
        model_.addItem("Learn C++");
        model_.addItem("Implement MVC");
        model_.addItem("Write tests");

        view_.render(model_);

        model_.toggleItem(1);
        view_.render(model_);
    }

private:
    TodoModel& model_;
    TodoView& view_;
};

int main() {
    TodoModel model;
    TodoView view;
    TodoController controller(model, view);

    controller.run();

    return 0;
}
```

## Variant: MVVM

MVVM (Model-View-ViewModel) is the modern variant of MVC:

```cpp
// ViewModel: connects Model and View
class TodoViewModel {
public:
    void addItem(const std::string& text) {
        model_.addItem(text);
        onChanged();
    }

    void onChanged() {
        if (callback_) callback_();
    }

    void setCallback(std::function<void()> cb) {
        callback_ = std::move(cb);
    }

    const std::vector<TodoModel::Item>& getItems() const {
        return model_.getItems();
    }

private:
    TodoModel model_;
    std::function<void()> callback_;
};
```

## Best Practices

1. **Keep the Model pure**: no UI-framework dependencies
2. **Use dependency injection**: testing stays easy
3. **Separate concerns**: each component does one thing

## Summary

MVC keeps the code structure clear and maintainable by separating Model,
View, and Controller.
