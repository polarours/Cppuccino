# 抽象工厂模式 (Abstract Factory Pattern)

## 什么是抽象工厂模式？

抽象工厂模式（Abstract Factory Pattern）是一种创建型设计模式，它提供一个接口来创建一系列相关或依赖的对象，而无需指定它们具体的类。

> **核心思想**：将工厂方法模式进一步抽象，创建一个工厂来创建其他工厂。

## 为什么需要抽象工厂模式？

### 问题场景

假设我们有一个游戏开发系统，需要创建不同类型的角色：

```cpp
// 问题：每个角色类型需要不同的工厂
class CharacterFactory {
public:
    virtual Warrior* createWarrior() = 0;
    virtual Mage* createMage() = 0;
    virtual Healer* createHealer() = 0;
};

// 人类角色工厂
class HumanFactory : public CharacterFactory {
public:
    Warrior* createWarrior() override { return new HumanWarrior(); }
    Mage* createMage() override { return new HumanMage(); }
    Healer* createHealer() override { return new HumanHealer(); }
};

// 兽人角色工厂
class OrcFactory : public CharacterFactory {
public:
    Warrior* createWarrior() override { return new OrcWarrior(); }
    Mage* createMage() override { return new OrcMage(); }
    Healer* createHealer() override { return new OrcHealer(); }
};
```

**问题**：
1. 需要为每种角色类型创建工厂
2. 添加新角色类型需要修改所有工厂
3. 难以创建相关的对象族

### 解决方案

使用抽象工厂模式：

```cpp
// 抽象产品族
class Warrior {
public:
    virtual ~Warrior() = default;
    virtual std::string attack() = 0;
};

class Mage {
public:
    virtual ~Mage() = default;
    virtual std::string castSpell() = 0;
};

// 抽象工厂
class CharacterFactory {
public:
    virtual ~CharacterFactory() = default;
    virtual std::unique_ptr<Warrior> createWarrior() = 0;
    virtual std::unique_ptr<Mage> createMage() = 0;
};

// 具体工厂
class HumanFactory : public CharacterFactory {
public:
    std::unique_ptr<Warrior> createWarrior() override {
        return std::make_unique<HumanWarrior>();
    }
    std::unique_ptr<Mage> createMage() override {
        return std::make_unique<HumanMage>();
    }
};

class OrcFactory : public CharacterFactory {
public:
    std::unique_ptr<Warrior> createWarrior() override {
        return std::make_unique<OrcWarrior>();
    }
    std::unique_ptr<Mage> createMage() override {
        return std::make_unique<OrcMage>();
    }
};

// 客户端使用
void playGame(CharacterFactory& factory) {
    auto warrior = factory.createWarrior();
    auto mage = factory.createMage();
    
    std::cout << warrior->attack() << "\n";
    std::cout << mage->castSpell() << "\n";
}
```

**优点**：
1. 确保相关产品一起创建
2. 易于切换产品族
3. 符合开闭原则

## 模式结构

```
                    ┌─────────────┐
                    │AbstractFactory│
                    │  (抽象工厂)   │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │HumanFactory │ │OrcFactory   │ │ElfFactory   │ ← 具体工厂
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │      AbstractProduct    │ ← 抽象产品
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │HumanWarrior │ │OrcWarrior   │ │ElfWarrior   │ ← 具体产品
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义抽象产品

```cpp
#include <memory>
#include <string>
#include <iostream>

class Button {
public:
    virtual ~Button() = default;
    virtual std::string render() const = 0;
    virtual std::string onClick() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual std::string render() const = 0;
    virtual bool isChecked() const = 0;
};
```

### 2. 实现具体产品族

```cpp
// Windows 风格组件
class WindowsButton : public Button {
public:
    std::string render() const override {
        return "Rendering Windows button";
    }
    
    std::string onClick() const override {
        return "Windows button clicked";
    }
};

class WindowsCheckbox : public Checkbox {
public:
    std::string render() const override {
        return "Rendering Windows checkbox";
    }
    
    bool isChecked() const override {
        return true;
    }
};

// Mac 风格组件
class MacButton : public Button {
public:
    std::string render() const override {
        return "Rendering Mac button";
    }
    
    std::string onClick() const override {
        return "Mac button clicked";
    }
};

class MacCheckbox : public Checkbox {
public:
    std::string render() const override {
        return "Rendering Mac checkbox";
    }
    
    bool isChecked() const override {
        return false;
    }
};
```

### 3. 定义抽象工厂

```cpp
class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
};
```

### 4. 实现具体工厂

```cpp
class WindowsFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<WindowsCheckbox>();
    }
};

class MacFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<MacCheckbox>();
    }
};
```

### 5. 客户端使用

```cpp
class Application {
public:
    explicit Application(std::unique_ptr<GUIFactory> factory)
        : factory_(std::move(factory)) {
        button_ = factory_->createButton();
        checkbox_ = factory_->createCheckbox();
    }
    
    void render() const {
        std::cout << button_->render() << "\n";
        std::cout << checkbox_->render() << "\n";
    }
    
    void handleClick() const {
        std::cout << button_->onClick() << "\n";
    }
    
    bool isChecked() const {
        return checkbox_->isChecked();
    }

private:
    std::unique_ptr<GUIFactory> factory_;
    std::unique_ptr<Button> button_;
    std::unique_ptr<Checkbox> checkbox_;
};

int main() {
    // 使用 Windows 风格
    auto winApp = Application(std::make_unique<WindowsFactory>());
    winApp.render();
    winApp.handleClick();
    
    std::cout << "\n";
    
    // 使用 Mac 风格
    auto macApp = Application(std::make_unique<MacFactory>());
    macApp.render();
    macApp.handleClick();
    
    return 0;
}
```

## 现代 C++ 实现

### 使用工厂注册表

```cpp
class FactoryRegistry {
public:
    template<typename T>
    static void registerFactory(const std::string& name) {
        factories_[name] = []() { return std::make_unique<T>(); };
    }
    
    static std::unique_ptr<GUIFactory> create(const std::string& name) {
        auto it = factories_.find(name);
        if (it != factories_.end()) {
            return it->second();
        }
        throw std::runtime_error("Unknown factory: " + name);
    }

private:
    static std::map<std::string, std::function<std::unique_ptr<GUIFactory>()>> factories_;
};
```

### 使用模板简化

```cpp
template<typename ButtonType, typename CheckboxType>
class TypedGUIFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<ButtonType>();
    }
    
    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<CheckboxType>();
    }
};

// 使用
using WindowsGUIFactory = TypedGUIFactory<WindowsButton, WindowsCheckbox>;
using MacGUIFactory = TypedGUIFactory<MacButton, MacCheckbox>;
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **工厂方法** | 工厂方法创建单个产品；抽象工厂创建产品族 |
| **建造者模式** | 建造者逐步构建复杂对象；抽象工厂创建相关产品族 |
| **单例模式** | 单例确保唯一实例；抽象工厂创建对象族 |

## 最佳实践

### 1. 使用工厂方法替代

```cpp
// 如果只需要创建一种产品，使用工厂方法
class SimpleFactory {
public:
    static std::unique_ptr<Button> createButton() {
        return std::make_unique<WindowsButton>();
    }
};

// 如果需要创建产品族，使用抽象工厂
class ComplexFactory {
public:
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
};
```

### 2. 支持产品变体

```cpp
class ThemedButton : public Button {
public:
    explicit ThemedButton(const std::string& theme) : theme_(theme) {}
    
    std::string render() const override {
        return "Rendering themed button (" + theme_ + ")";
    }

private:
    std::string theme_;
};
```

### 3. 使用依赖注入

```cpp
class DIContainer {
public:
    template<typename T>
    void registerSingleton() {
        instances_[typeid(T).name()] = std::make_shared<T>();
    }
    
    template<typename T>
    std::shared_ptr<T> get() {
        auto it = instances_.find(typeid(T).name());
        if (it != instances_.end()) {
            return std::static_pointer_cast<T>(it->second);
        }
        throw std::runtime_error("No registration for: " + typeid(T).name());
    }

private:
    std::map<std::string, std::shared_ptr<void>> instances_;
};
```

## 常见陷阱

### 陷阱 1：工厂过于复杂

```cpp
// 不推荐：工厂包含过多逻辑
class BadFactory {
public:
    std::unique_ptr<Button> createButton() override {
        // 太多逻辑...
        if (condition1) {
            return std::make_unique<WindowsButton>();
        } else if (condition2) {
            return std::make_unique<MacButton>();
        }
        // ...
    }
};

// 推荐：工厂只做创建
class GoodFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
};
```

### 陷阱 2：违反开闭原则

```cpp
// 不推荐：添加新产品需要修改工厂
class OldFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }
    // 添加新产品需要修改这里
};

// 推荐：通过继承扩展
class NewButtonFactory : public GUIFactory {
    // 添加新产品
};
```

### 陷阱 3：忘记管理生命周期

```cpp
// 不推荐：手动管理内存
class BadApplication {
public:
    BadApplication(GUIFactory* factory) : factory_(factory) {
        button_ = factory_->createButton();  // 返回裸指针
    }
    
private:
    GUIFactory* factory_;
    Button* button_;  // 谁负责 delete？
};

// 推荐：使用智能指针
class GoodApplication {
public:
    explicit GoodApplication(std::unique_ptr<GUIFactory> factory)
        : factory_(std::move(factory)) {
        button_ = factory_->createButton();
    }
    
private:
    std::unique_ptr<GUIFactory> factory_;
    std::unique_ptr<Button> button_;
};
```

## 总结

抽象工厂模式的核心要点：

- **适用场景**：需要创建相关产品族、多种产品变体、需要解耦创建过程
- **核心优点**：确保相关产品一起创建、易于切换产品族、符合开闭原则
- **实现要点**：使用智能指针、保持工厂简单、支持依赖注入
- **常见陷阱**：工厂过于复杂、违反开闭原则、生命周期管理

> **记住**：抽象工厂模式的关键是"产品族创建"——通过一个接口创建一系列相关对象，确保它们可以一起工作。
