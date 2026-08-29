# 工厂方法模式 (Factory Method Pattern)

## 什么是工厂方法模式？

工厂方法模式（Factory Method Pattern）将"对象的创建"封装到一个方法中，让子类或注册项决定具体实例化哪个类。调用方只依赖抽象接口，不直接 `new` 具体类。

> **核心思想**：用工厂方法代替 `new`，把"用什么类"从使用点移到注册表/子类。

## 适用场景

- 需要根据类型字符串/配置创建不同实现
- 对象构造逻辑较复杂（依赖注入、池化、缓存）
- 想在运行时扩展产品类型（开放-封闭原则）

## 核心概念

- **产品接口（Product）**：所有具体产品实现的公共接口
- **具体产品（Concrete Product）**：实现 Product 的具体类
- **工厂（Factory）**：暴露 `create(type, ...)` 方法，按输入返回产品
- **注册表（Registry）**：用 `type → creator` 映射代替 `if/else` 或 `switch`

## 与 Simple Factory 的区别

`if (type=="book") return new Book(...)` 也常被叫做"简单工厂"——但严格说它不是 GoF 23 模式，而是工厂方法的退化。GoF 工厂方法把创建逻辑下放到**子类**或**注册回调**里：

```cpp
// 退化：所有逻辑在一个函数里
Product* make(std::string type) {
    if (type == "book") return new Book();
    if (type == "toy")  return new Toy();
    return nullptr;
}

// 工厂方法：每种产品一个 creator
factory.registerProduct("book", [](auto name, auto price){
    return std::make_unique<Book>(name, price);
});
factory.registerProduct("toy", [](auto name, auto price){
    return std::make_unique<Toy>(name, price);
});
```

## 代码实现

```cpp
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <map>

class Product {
public:
    virtual ~Product() = default;
    virtual std::string name() const = 0;
    virtual double price() const = 0;
};

class Book : public Product {
public:
    Book(std::string title, double price) : title_(std::move(title)), price_(price) {}
    std::string name() const override { return "Book: " + title_; }
    double price() const override { return price_; }
private:
    std::string title_; double price_;
};

class Electronics : public Product {
public:
    Electronics(std::string model, double price) : model_(std::move(model)), price_(price) {}
    std::string name() const override { return "Electronics: " + model_; }
    double price() const override { return price_; }
private:
    std::string model_; double price_;
};

class ProductFactory {
public:
    using Creator = std::function<std::unique_ptr<Product>(const std::string&, double)>;

    static void registerProduct(const std::string& type, Creator creator) {
        getRegistry()[type] = std::move(creator);
    }

    static std::unique_ptr<Product> create(const std::string& type,
                                           const std::string& name, double price) {
        auto& reg = getRegistry();
        auto it = reg.find(type);
        if (it == reg.end()) throw std::runtime_error("Unknown product: " + type);
        return it->second(name, price);
    }

private:
    static std::map<std::string, Creator>& getRegistry() {
        static std::map<std::string, Creator> r;
        return r;
    }
};
```

完整可运行示例见 [factory-pattern 项目](../../cpp-mastery-roadmap/10-stage10/factory-pattern)。

## 使用示例

```cpp
ProductFactory::registerProduct("book",
    [](const std::string& n, double p){ return std::make_unique<Book>(n, p); });
ProductFactory::registerProduct("electronics",
    [](const std::string& n, double p){ return std::make_unique<Electronics>(n, p); });

auto p = ProductFactory::create("book", "The C++ Programming Language", 59.99);
assert(p->price() == 59.99);
```

## 最佳实践

1. **用 `std::function` 注册 creator**：比继承 + 工厂子类更轻量，且支持 lambda
2. **注册放在翻译单元初始化期**：避免在并发路径上首次注册
3. **未知 type 要明确报错**：`throw` 比返回 `nullptr` 更早暴露调用 bug
4. **产品构造参数要保持一致**：所有 creator 接受相同签名，否则调用方难以泛化

## 总结

工厂方法把"决定具体类"从使用方挪到注册表，让新增产品只需一行 `registerProduct`——是消除 `if/else` 分支、保持代码对扩展开放的经典手段。
