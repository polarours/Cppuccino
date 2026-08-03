# 建造者模式 (Builder Pattern)

## 什么是建造者模式？

建造者模式（Builder Pattern）是一种创建型设计模式，它将复杂对象的构建过程与其表示分离，使得同样的构建过程可以创建不同的表示。

> **核心思想**：将对象的构建步骤封装到建造者中，通过逐步构建来创建复杂对象。

## 为什么需要建造者模式？

### 问题场景

假设我们有一个 Pizza 对象，有很多可选参数：

```cpp
// 问题：构造函数参数过多
class Pizza {
public:
    Pizza(bool cheese, bool pepperoni, bool mushrooms, 
          bool olives, bool onions, bool peppers,
          bool sausage, bool ham, bool bacon,
          double size, bool thinCrust, bool stuffedCrust)
        : cheese_(cheese), pepperoni_(pepperoni), ... {}
    
private:
    bool cheese_;
    bool pepperoni_;
    // ... 10+ 个参数
};

// 调用时难以阅读
Pizza p(true, false, true, false, true, false, false, false, false, 14, true, false);
```

**问题**：
1. 参数过多导致构造函数难以使用
2. 参数顺序容易混淆
3. 难以验证参数合法性

### 解决方案

使用建造者模式：

```cpp
class PizzaBuilder {
public:
    PizzaBuilder& size(double size) { size_ = size; return *this; }
    PizzaBuilder& thinCrust() { thinCrust_ = true; return *this; }
    PizzaBuilder& addCheese() { cheese_ = true; return *this; }
    PizzaBuilder& addPepperoni() { pepperoni_ = true; return *this; }
    // ... 其他方法
    
    Pizza build() const {
        validate();
        return Pizza(size_, thinCrust_, cheese_, pepperoni_, ...);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool cheese_ = false;
    bool pepperoni_ = false;
    // ...
};

// 使用清晰易读
Pizza p = PizzaBuilder()
    .size(14)
    .thinCrust()
    .addCheese()
    .addPepperoni()
    .build();
```

**优点**：
1. 可读性好：方法调用清晰表达意图
2. 可验证：可以在 build() 时验证参数
3. 灵活：可以创建不同配置的 object
4. 支持不可变对象：build() 后对象不可修改

## 模式结构

```
                    ┌─────────────┐
                    │  Director   │
                    │  (导演)      │
                    └──────┬──────┘
                           │ uses
                           │
              ┌────────────▼────────────┐
              │       Builder           │ ← 抽象建造者
              │   (PizzaBuilder)        │
              └────────────┬────────────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Hawaiian   │ │  Vegetarian │ │  MeatLover  │ ← 具体建造者
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │        Product          │ ← 产品
              │        (Pizza)          │
              └─────────────────────────┘
```

## 代码实现

### 1. 定义产品

```cpp
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

class Pizza {
public:
    Pizza(double size, bool thinCrust, bool stuffedCrust,
          bool cheese, bool tomatoSauce,
          std::vector<std::string> toppings)
        : size_(size), thinCrust_(thinCrust), stuffedCrust_(stuffedCrust),
          cheese_(cheese), tomatoSauce_(tomatoSauce),
          toppings_(std::move(toppings)) {}
    
    void display() const {
        std::cout << "=== Pizza ===\n";
        std::cout << "Size: " << size_ << " inches\n";
        std::cout << "Crust: " << (thinCrust_ ? "Thin" : "Regular");
        if (stuffedCrust_) std::cout << " (Stuffed)";
        std::cout << "\n";
        std::cout << "Cheese: " << (cheese_ ? "Yes" : "No") << "\n";
        std::cout << "Sauce: " << (tomatoSauce_ ? "Tomato" : "None") << "\n";
        std::cout << "Toppings:\n";
        for (const auto& t : toppings_) {
            std::cout << "  - " << t << "\n";
        }
    }

private:
    double size_;
    bool thinCrust_;
    bool stuffedCrust_;
    bool cheese_;
    bool tomatoSauce_;
    std::vector<std::string> toppings_;
};
```

### 2. 定义抽象建造者

```cpp
class PizzaBuilder {
public:
    virtual ~PizzaBuilder() = default;
    virtual PizzaBuilder& size(double size) = 0;
    virtual PizzaBuilder& thinCrust() = 0;
    virtual PizzaBuilder& stuffedCrust() = 0;
    virtual PizzaBuilder& cheese() = 0;
    virtual PizzaBuilder& tomatoSauce() = 0;
    virtual PizzaBuilder& addTopping(const std::string& topping) = 0;
    virtual Pizza build() const = 0;
};
```

### 3. 实现具体建造者

```cpp
class HawaiianPizzaBuilder : public PizzaBuilder {
public:
    PizzaBuilder& size(double size) override {
        size_ = size;
        return *this;
    }
    
    PizzaBuilder& thinCrust() override {
        thinCrust_ = true;
        return *this;
    }
    
    PizzaBuilder& stuffedCrust() override {
        stuffedCrust_ = true;
        return *this;
    }
    
    PizzaBuilder& cheese() override {
        cheese_ = true;
        return *this;
    }
    
    PizzaBuilder& tomatoSauce() override {
        sauce_ = "Tomato";
        return *this;
    }
    
    PizzaBuilder& addTopping(const std::string& topping) override {
        toppings_.push_back(topping);
        return *this;
    }
    
    Pizza build() const override {
        if (toppings_.empty()) {
            throw std::invalid_argument("Hawaiian pizza must have toppings");
        }
        return Pizza(size_, thinCrust_, stuffedCrust_, cheese_, 
                     sauce_ == "Tomato", toppings_);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool stuffedCrust_ = false;
    bool cheese_ = true;
    std::string sauce_ = "None";
    std::vector<std::string> toppings_;
};

class VegetarianPizzaBuilder : public PizzaBuilder {
public:
    PizzaBuilder& size(double size) override { size_ = size; return *this; }
    PizzaBuilder& thinCrust() override { thinCrust_ = true; return *this; }
    PizzaBuilder& stuffedCrust() override { stuffedCrust_ = true; return *this; }
    PizzaBuilder& cheese() override { cheese_ = true; return *this; }
    PizzaBuilder& tomatoSauce() override { sauce_ = "Tomato"; return *this; }
    PizzaBuilder& addTopping(const std::string& topping) override {
        toppings_.push_back(topping);
        return *this;
    }
    
    Pizza build() const override {
        if (toppings_.empty()) {
            throw std::invalid_argument("Vegetarian pizza must have toppings");
        }
        return Pizza(size_, thinCrust_, stuffedCrust_, cheese_,
                     sauce_ == "Tomato", toppings_);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool stuffedCrust_ = false;
    bool cheese_ = true;
    std::string sauce_ = "None";
    std::vector<std::string> toppings_;
};
```

### 4. 实现导演类

```cpp
class Waiter {
public:
    Pizza construct(const PizzaBuilder& builder) {
        // 导演控制建造过程
        return builder.size(14)
                    .thinCrust()
                    .cheese()
                    .tomatoSauce()
                    .addTopping("Peppers")
                    .addTopping("Onions")
                    .build();
    }
};
```

### 5. 客户端使用

```cpp
int main() {
    // 直接创建
    HawaiianPizzaBuilder hawaiian;
    auto pizza1 = hawaiian.size(14)
                          .thinCrust()
                          .cheese()
                          .addTopping("Ham")
                          .addTopping("Pineapple")
                          .build();
    pizza1.display();
    
    // 通过导演创建
    Waiter waiter;
    VegetarianPizzaBuilder veggie;
    auto pizza2 = waiter.construct(veggie);
    pizza2.display();
    
    return 0;
}
```

## 现代 C++ 实现

### 使用流式 API

```cpp
class ModernPizzaBuilder {
public:
    ModernPizzaBuilder& size(double s) { size_ = s; return *this; }
    ModernPizzaBuilder& thinCrust() { thinCrust_ = true; return *this; }
    ModernPizzaBuilder& stuffedCrust() { stuffedCrust_ = true; return *this; }
    ModernPizzaBuilder& cheese() { cheese_ = true; return *this; }
    ModernPizzaBuilder& sauce(const std::string& s) { sauce_ = s; return *this; }
    ModernPizzaBuilder& topping(const std::string& t) { 
        toppings_.push_back(t); return *this; 
    }
    
    Pizza build() const {
        return Pizza(size_, thinCrust_, stuffedCrust_, cheese_, 
                     sauce_ == "Tomato", toppings_);
    }

private:
    double size_ = 12.0;
    bool thinCrust_ = false;
    bool stuffedCrust_ = false;
    bool cheese_ = false;
    std::string sauce_ = "None";
    std::vector<std::string> toppings_;
};
```

### 使用静态方法

```cpp
class PizzaBuilder {
public:
    // 静态工厂方法
    static ModernPizzaBuilder hawaiian() {
        return ModernPizzaBuilder()
            .cheese()
            .sauce("Tomato")
            .topping("Ham")
            .topping("Pineapple");
    }
    
    static ModernPizzaBuilder vegetarian() {
        return ModernPizzaBuilder()
            .cheese()
            .sauce("Tomato")
            .topping("Peppers")
            .topping("Onions")
            .topping("Mushrooms");
    }
    
    static ModernPizzaBuilder meatLover() {
        return ModernPizzaBuilder()
            .cheese()
            .sauce("Tomato")
            .topping("Pepperoni")
            .topping("Sausage")
            .topping("Bacon");
    }
};

// 使用
auto pizza = PizzaBuilder::hawaiian().size(14).thinCrust().build();
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **工厂模式** | 工厂创建完整对象；建造者逐步构建对象 |
| **抽象工厂** | 抽象工厂创建产品族；建造者关注单个对象构建 |
| **原型模式** | 原型通过克隆创建；建造者通过步骤构建 |

## 最佳实践

### 1. 使用不可变对象

```cpp
class Pizza {
public:
    // 所有成员都是 const
    const double size_;
    const bool thinCrust_;
    // ...
    
private:
    Pizza(...) = default;  // 私有构造函数
    friend class PizzaBuilder;  // 只允许建造者创建
};
```

### 2. 验证参数

```cpp
Pizza build() const {
    if (size_ < 8 || size_ > 24) {
        throw std::invalid_argument("Invalid size: " + std::to_string(size_));
    }
    if (toppings_.empty()) {
        throw std::invalid_argument("Pizza must have at least one topping");
    }
    return Pizza(size_, thinCrust_, stuffedCrust_, cheese_, 
                 sauce_ == "Tomato", toppings_);
}
```

### 3. 支持可选参数

```cpp
class PizzaBuilder {
public:
    PizzaBuilder& size(double s) { size_ = s; return *this; }
    PizzaBuilder& thinCrust() { thinCrust_ = true; return *this; }
    // 可选参数
    PizzaBuilder& extraCheese() { extraCheese_ = true; return *this; }
    PizzaBuilder& garlicButter() { garlicButter_ = true; return *this; }
    
    // 有默认值
    PizzaBuilder& sauce(const std::string& s = "Tomato") { 
        sauce_ = s; return *this; 
    }
};
```

## 常见陷阱

### 陷阱 1：建造者过于复杂

```cpp
// 不推荐：建造者包含过多逻辑
class ComplexBuilder {
public:
    PizzaBuilder& step1() { /* 复杂逻辑 */ return *this; }
    PizzaBuilder& step2() { /* 复杂逻辑 */ return *this; }
    // ... 50+ 个步骤
};

// 推荐：保持简单
class SimpleBuilder {
public:
    PizzaBuilder& size(double s) { size_ = s; return *this; }
    PizzaBuilder& topping(const std::string& t) { 
        toppings_.push_back(t); return *this; 
    }
};
```

### 陷阱 2：忘记调用 build()

```cpp
// 不推荐：忘记 build()
PizzaBuilder builder;
builder.size(14).cheese();  // 忘记 build()！

// 推荐：使用作用域或RAII
auto pizza = PizzaBuilder{}.size(14).cheese().build();
```

### 陷阱 3：建造者状态泄露

```cpp
// 不推荐：建造者状态可以被修改
class BadBuilder {
public:
    void setCheese(bool c) { cheese_ = c; }  // 暴露内部状态
    
private:
    bool cheese_ = false;
};

// 推荐：只暴露建造接口
class GoodBuilder {
public:
    GoodBuilder& cheese() { cheese_ = true; return *this; }
    
private:
    bool cheese_ = false;
};
```

## 总结

建造者模式的核心要点：

- **适用场景**：对象参数多、构建过程复杂、需要多种表示
- **核心优点**：可读性好、可验证、支持不可变对象
- **实现要点**：使用流式 API、验证参数、支持可选参数
- **常见陷阱**：过于复杂、忘记 build()、状态泄露

> **记住**：建造者模式的关键是"分步构建"——将复杂对象的构建分解为多个简单步骤，使构建过程清晰可控。
