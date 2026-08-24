# MVC 模式 (MVC Pattern)

## 什么是 MVC 模式？

MVC（Model-View-Controller）模式是一种架构模式，它将应用程序分为三个核心组件：模型（Model）、视图（View）和控制器（Controller）。

> **核心思想**：分离关注点，使代码更易于维护和扩展。

## 模式组件

### Model（模型）
- 管理应用程序的数据和业务逻辑
- 不依赖于 View 和 Controller
- 当数据改变时通知观察者

### View（视图）
- 负责数据的展示
- 从 Model 获取数据
- 不包含业务逻辑

### Controller（控制器）
- 处理用户输入
- 更新 Model
- 选择适当的 View

## 代码实现

```cpp
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <map>

// Model: 数据层
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

// View: 展示层
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

// Controller: 控制层
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

## 变体：MVVM

MVVM（Model-View-ViewModel）是 MVC 的现代变体：

```cpp
// ViewModel: 连接 Model 和 View
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
    
    const TodoModel::Items& getItems() const {
        return model_.getItems();
    }

private:
    TodoModel model_;
    std::function<void()> callback_;
};
```

## 最佳实践

1. **保持 Model 纯粹**：不依赖 UI 框架
2. **使用依赖注入**：便于测试
3. **分离关注点**：每个组件只做一件事

## 总结

MVC 模式通过分离 Model、View、Controller，使代码结构清晰、易于维护。
