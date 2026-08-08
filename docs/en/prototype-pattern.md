# 原型模式 (Prototype Pattern)

## 什么是原型模式？

原型模式（Prototype Pattern）是一种创建型设计模式，它通过复制现有的对象来创建新对象，而不是通过 new 关键字创建。当直接创建对象的代价较大时，可以使用原型模式。

> **核心思想**：通过克隆原型来创建新对象，避免重复的初始化开销。

## 为什么需要原型模式？

### 问题场景

假设我们有一个复杂的图形对象，创建开销很大：

```cpp
// 问题：每次创建都需要重新初始化
class ComplexGraph {
public:
    ComplexGraph() {
        // 初始化复杂的图形数据
        loadFromFile("huge_graph.dat");
        computeLayout();
        buildRenderingTree();
    }
    
    ComplexGraph(const ComplexGraph& other) {
        // 深拷贝所有数据
        data_ = other.data_;
        layout_ = other.layout_;
        // ...
    }
};

// 需要创建多个类似的对象
auto graph1 = std::make_unique<ComplexGraph>();
auto graph2 = std::make_unique<ComplexGraph>(*graph1);  // 拷贝
auto graph3 = std::make_unique<ComplexGraph>(*graph1);  // 拷贝
```

**问题**：
1. 创建成本高
2. 初始化复杂
3. 需要多次拷贝

### 解决方案

使用原型模式：

```cpp
class Prototype {
public:
    virtual ~Prototype() = default;
    virtual std::unique_ptr<Prototype> clone() const = 0;
};

class ComplexGraph : public Prototype {
public:
    std::unique_ptr<Prototype> clone() const override {
        return std::make_unique<ComplexGraph>(*this);
    }
};

// 使用
auto prototype = std::make_unique<ComplexGraph>();
prototype->loadFromFile("huge_graph.dat");

auto copy1 = prototype->clone();
auto copy2 = prototype->clone();
```

**优点**：
1. 避免重复初始化
2. 运行时决定创建类型
3. 简化创建过程

## 模式结构

```
                    ┌─────────────┐
                    │  Prototype  │
                    │  (原型)      │
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │     ConcretePrototype   │ ← 具体原型
              │   (ComplexGraph)        │
              └────────────┬────────────┘
                           │ clone()
                           │
              ┌────────────▼────────────┐
              │      Client             │ ← 客户端
              │  (使用 clone 创建新对象) │
              └─────────────────────────┘
```

## 代码实现

### 1. 定义原型接口

```cpp
#include <memory>
#include <string>
#include <map>
#include <iostream>

class Document : public std::enable_shared_from_this<Document> {
public:
    virtual ~Document() = default;
    virtual std::shared_ptr<Document> clone() const = 0;
    virtual std::string getType() const = 0;
    
    virtual void setContent(const std::string& content) {
        content_ = content;
    }
    
    virtual std::string getContent() const {
        return content_;
    }

protected:
    std::string content_;
};
```

### 2. 实现具体原型

```cpp
class TextDocument : public Document {
public:
    std::shared_ptr<Document> clone() const override {
        auto doc = std::make_shared<TextDocument>(*this);
        return doc;
    }
    
    std::string getType() const override {
        return "TextDocument";
    }
    
    void setFontSize(int size) { font_size_ = size; }
    int getFontSize() const { return font_size_; }

private:
    int font_size_ = 12;
};

class ImageDocument : public Document {
public:
    std::shared_ptr<Document> clone() const override {
        auto doc = std::make_shared<ImageDocument>(*this);
        return doc;
    }
    
    std::string getType() const override {
        return "ImageDocument";
    }
    
    void setWidth(int width) { width_ = width; }
    void setHeight(int height) { height_ = height; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    int width_ = 800;
    int height_ = 600;
};

class SpreadsheetDocument : public Document {
public:
    std::shared_ptr<Document> clone() const override {
        auto doc = std::make_shared<SpreadsheetDocument>(*this);
        return doc;
    }
    
    std::string getType() const override {
        return "SpreadsheetDocument";
    }
    
    void addCell(int row, int col, const std::string& value) {
        cells_[{row, col}] = value;
    }
    
    std::string getCell(int row, int col) const {
        auto it = cells_.find({row, col});
        return it != cells_.end() ? it->second : "";
    }

private:
    std::map<std::pair<int, int>, std::string> cells_;
};
```

### 3. 实现原型管理器

```cpp
class PrototypeRegistry {
public:
    void registerPrototype(const std::string& type, 
                           std::shared_ptr<Document> prototype) {
        prototypes_[type] = prototype;
    }
    
    std::shared_ptr<Document> create(const std::string& type) const {
        auto it = prototypes_.find(type);
        if (it == prototypes_.end()) {
            throw std::invalid_argument("Unknown document type: " + type);
        }
        return it->second->clone();
    }
    
    size_t count() const { return prototypes_.size(); }
    
    void listTypes() const {
        std::cout << "Available prototypes:\n";
        for (const auto& [type, proto] : prototypes_) {
            std::cout << "  - " << type << "\n";
        }
    }

private:
    std::map<std::string, std::shared_ptr<Document>> prototypes_;
};
```

### 4. 客户端使用

```cpp
int main() {
    PrototypeRegistry registry;
    
    // 注册原型
    auto textProto = std::make_shared<TextDocument>();
    textProto->setContent("Original text content");
    textProto->setFontSize(14);
    registry.registerPrototype("Text", textProto);
    
    auto imageProto = std::make_shared<ImageDocument>();
    imageProto->setContent("Original image data");
    imageProto->setWidth(1920);
    imageProto->setHeight(1080);
    registry.registerPrototype("Image", imageProto);
    
    auto sheetProto = std::make_shared<SpreadsheetDocument>();
    sheetProto->setContent("Original spreadsheet");
    sheetProto->addCell(0, 0, "Hello");
    sheetProto->addCell(1, 1, "World");
    registry.registerPrototype("Spreadsheet", sheetProto);
    
    // 列出可用的原型
    registry.listTypes();
    
    // 创建克隆
    auto doc1 = registry.create("Text");
    doc1->setContent("Modified text content");
    std::cout << "\nCreated: " << doc1->getType() << "\n";
    std::cout << "Content: " << doc1->getContent() << "\n";
    
    auto doc2 = registry.create("Image");
    std::cout << "\nCreated: " << doc2->getType() << "\n";
    
    auto doc3 = registry.create("Spreadsheet");
    std::cout << "\nCreated: " << doc3->getType() << "\n";
    std::cout << "Cell(0,0): " << doc3->getCell(0, 0) << "\n";
    
    // 深拷贝验证
    auto textClone = std::static_pointer_cast<TextDocument>(doc1);
    std::cout << "\nFont size: " << textClone->getFontSize() << "\n";
    
    return 0;
}
```

## 现代 C++ 实现

### 使用 CRTP 简化克隆

```cpp
template<typename Derived>
class Cloneable {
public:
    std::shared_ptr<Derived> clone() const {
        return std::make_shared<Derived>(static_cast<const Derived&>(*this));
    }
};

class MyDocument : public Cloneable<MyDocument> {
    // 自动获得 clone() 方法
};
```

### 使用工厂函数

```cpp
template<typename T>
std::shared_ptr<T> clonePrototype(const std::shared_ptr<T>& prototype) {
    return std::make_shared<T>(*prototype);
}

// 使用
auto clone = clonePrototype<TextDocument>(textProto);
```

### 使用自定义克隆策略

```cpp
class CustomCloneable {
public:
    using CloneFunc = std::function<std::shared_ptr<CustomCloneable>()>;
    
    void registerClone(const std::string& type, CloneFunc cloneFunc) {
        cloneFuncs_[type] = std::move(cloneFunc);
    }
    
    std::shared_ptr<CustomCloneable> clone(const std::string& type) const {
        auto it = cloneFuncs_.find(type);
        if (it == cloneFuncs_.end()) {
            throw std::invalid_argument("Unknown type: " + type);
        }
        return it->second();
    }

private:
    std::map<std::string, CloneFunc> cloneFuncs_;
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **工厂模式** | 工厂创建新对象；原型通过克隆创建 |
| **建造者模式** | 建造者逐步构建；原型通过复制创建 |
| **单例模式** | 单例只有一个实例；原型可以创建多个克隆 |

## 最佳实践

### 1. 实现深拷贝

```cpp
class DeepCloneable {
public:
    virtual std::shared_ptr<DeepCloneable> clone() const = 0;
    
protected:
    // 确保拷贝是深拷贝
    DeepCloneable(const DeepCloneable& other) 
        : data_(other.data_) {}  // 深拷贝
    
private:
    std::vector<int> data_;
};
```

### 2. 使用对象池缓存克隆

```cpp
class CloneCache {
public:
    std::shared_ptr<Document> getOrCreate(const std::string& type, 
                                           std::function<std::shared_ptr<Document>()> factory) {
        auto it = cache_.find(type);
        if (it != cache_.end() && !it->second.expired()) {
            return it->second.lock();
        }
        
        auto doc = factory();
        cache_[type] = std::weak_ptr<Document>(doc);
        return doc;
    }

private:
    std::map<std::string, std::weak_ptr<Document>> cache_;
};
```

### 3. 支持部分克隆

```cpp
class PartialCloneable {
public:
    virtual std::shared_ptr<PartialCloneable> clone() const = 0;
    virtual void modify(const std::string& key, const std::string& value) = 0;
};

class ModifiedClone : public PartialCloneable {
public:
    std::shared_ptr<PartialCloneable> clone() const override {
        auto clone = std::make_shared<ModifiedClone>(*this);
        for (const auto& [key, value] : modifications_) {
            clone->modify(key, value);
        }
        return clone;
    }
    
    void modify(const std::string& key, const std::string& value) override {
        modifications_[key] = value;
    }

private:
    std::map<std::string, std::string> modifications_;
};
```

## 常见陷阱

### 陷阱 1：浅拷贝问题

```cpp
// 不推荐：浅拷贝可能导致悬垂指针
class BadPrototype {
public:
    std::unique_ptr<BadPrototype> clone() const {
        auto clone = std::make_unique<BadPrototype>(*this);
        clone->data_ = data_;  // 浅拷贝指针
        return clone;
    }
    
private:
    int* data_ = nullptr;  // 需要深拷贝
};

// 推荐：深拷贝或智能指针
class GoodPrototype {
public:
    std::unique_ptr<GoodPrototype> clone() const {
        return std::make_unique<GoodPrototype>(*this);
    }
    
private:
    std::unique_ptr<int[]> data_;  // 自动深拷贝
};
```

### 陷阱 2：循环引用

```cpp
// 危险：循环引用导致内存泄漏
class Node {
public:
    std::shared_ptr<Node> clone() const {
        auto clone = std::make_shared<Node>(*this);
        if (parent_) {
            clone->parent_ = parent_->clone();  // 循环引用
        }
        return clone;
    }
    
private:
    std::shared_ptr<Node> parent_;
};
```

### 陷阱 3：忘记实现 clone()

```cpp
// 不推荐：忘记实现 clone()
class IncompletePrototype {
public:
    virtual std::shared_ptr<IncompletePrototype> clone() const {
        // 忘记实现！
        return nullptr;
    }
};

// 推荐：使用 CRTP 强制实现
template<typename Derived>
class Cloneable {
public:
    std::shared_ptr<Derived> clone() const {
        return std::make_shared<Derived>(static_cast<const Derived&>(*this));
    }
};
```

## 总结

原型模式的核心要点：

- **适用场景**：创建成本高、需要动态创建类型、需要缓存对象
- **核心优点**：避免重复初始化、支持运行时多态、简化创建
- **实现要点**：深拷贝、使用智能指针、考虑缓存
- **常见陷阱**：浅拷贝、循环引用、未实现 clone()

> **记住**：原型模式的关键是"克隆"——通过复制现有对象来创建新对象，避免重复的初始化开销。
