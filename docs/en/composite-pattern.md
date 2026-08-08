# 组合模式 (Composite Pattern)

## 什么是组合模式？

组合模式（Composite Pattern）是一种结构型设计模式，它将对象组合成树形结构以表示"部分-整体"的层次结构。组合模式使得客户端对单个对象和组合对象的使用具有一致性。

> **核心思想**：让客户端统一处理单个对象和组合对象，无需区分它们。

## 为什么需要组合模式？

### 问题场景

假设我们有一个图形编辑程序，需要处理各种形状：

```cpp
// 问题：需要分别处理单个形状和形状组
class Shape {
public:
    virtual void draw() = 0;
    virtual ~Shape() = default;
};

class Circle : public Shape {
public:
    void draw() override { /* ... */ }
};

class Rectangle : public Shape {
public:
    void draw() override { /* ... */ }
};

// 处理形状列表时的问题
void drawAll(const std::vector<Shape*>& shapes) {
    for (auto* shape : shapes) {
        shape->draw();  // 需要区分是单个形状还是组
    }
}
```

**问题**：
1. 需要分别处理单个对象和组合对象
2. 客户端代码复杂
3. 难以递归处理

### 解决方案

使用组合模式：

```cpp
class Component {
public:
    virtual void draw() = 0;
    virtual void add(Component* component) {}
    virtual void remove(Component* component) {}
    virtual Component* getChild(int index) { return nullptr; }
    virtual ~Component() = default;
};

class Composite : public Component {
public:
    void draw() override {
        for (auto* child : children_) {
            child->draw();
        }
    }
    
    void add(Component* component) override {
        children_.push_back(component);
    }
    
    void remove(Component* component) override {
        children_.erase(
            std::remove(children_.begin(), children_.end(), component),
            children_.end()
        );
    }

private:
    std::vector<Component*> children_;
};

class Leaf : public Component {
public:
    void draw() override { /* 绘制单个形状 */ }
};
```

**优点**：
1. 统一接口：客户端无需区分单个对象和组合对象
2. 递归处理：可以任意层次组合
3. 易于扩展：可以轻松添加新类型

## 模式结构

```
                    ┌─────────────┐
                    │  Component  │
                    │  (组件)      │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Leaf       │ │  Leaf       │ │  Composite  │ ← 组合对象
    │  (叶子)     │ │  (叶子)     │ │  (容器)     │
    └─────────────┘ └─────────────┘ └──────┬──────┘
                                           │
                        ┌──────────────────┼──────────────────┐
                        │                  │                  │
                 ┌──────▼──────┐   ┌──────▼──────┐   ┌──────▼──────┐
                 │  Leaf       │   │  Leaf       │   │  Leaf       │
                 └─────────────┘   └─────────────┘   └─────────────┘
```

## 代码实现

### 1. 定义组件接口

```cpp
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <algorithm>

class FileSystemComponent {
public:
    virtual ~FileSystemComponent() = default;
    virtual void display(int indent = 0) = 0;
    virtual size_t getSize() = 0;
    
    // 组合操作
    virtual void add(FileSystemComponent* component) {}
    virtual void remove(FileSystemComponent* component) {}
    virtual FileSystemComponent* getChild(int index) { return nullptr; }
    
    virtual std::string getName() const = 0;
};
```

### 2. 实现叶子节点

```cpp
class File : public FileSystemComponent {
public:
    File(const std::string& name, size_t size)
        : name_(name), size_(size) {}
    
    void display(int indent = 0) override {
        std::string prefix(indent * 2, ' ');
        std::cout << prefix << "[FILE] " << name_ << " (" << size_ << " bytes)\n";
    }
    
    size_t getSize() override { return size_; }
    std::string getName() const override { return name_; }

private:
    std::string name_;
    size_t size_;
};
```

### 3. 实现组合节点

```cpp
class Directory : public FileSystemComponent {
public:
    explicit Directory(const std::string& name) : name_(name) {}
    
    void add(FileSystemComponent* component) override {
        children_.push_back(component);
    }
    
    void remove(FileSystemComponent* component) override {
        children_.erase(
            std::remove(children_.begin(), children_.end(), component),
            children_.end()
        );
    }
    
    FileSystemComponent* getChild(int index) override {
        if (index >= 0 && index < static_cast<int>(children_.size())) {
            return children_[index];
        }
        return nullptr;
    }
    
    void display(int indent = 0) override {
        std::string prefix(indent * 2, ' ');
        std::cout << prefix << "[DIR] " << name_ << "\n";
        
        for (auto* child : children_) {
            child->display(indent + 1);
        }
    }
    
    size_t getSize() override {
        size_t total = 0;
        for (auto* child : children_) {
            total += child->getSize();
        }
        return total;
    }
    
    std::string getName() const override { return name_; }
    size_t getChildCount() const { return children_.size(); }

private:
    std::string name_;
    std::vector<FileSystemComponent*> children_;
};
```

### 4. 客户端使用

```cpp
int main() {
    // 创建文件系统结构
    Directory root("root");
    Directory docs("documents");
    Directory images("images");
    File readme("readme.txt", 1024);
    File photo("photo.jpg", 2048000);
    File logo("logo.png", 51200);
    
    // 构建目录树
    root.add(&docs);
    root.add(&images);
    docs.add(&readme);
    images.add(&photo);
    images.add(&logo);
    
    // 显示结构
    std::cout << "=== File System ===\n";
    root.display();
    
    // 获取大小
    std::cout << "\n=== Sizes ===\n";
    std::cout << "Root size: " << root.getSize() << " bytes\n";
    std::cout << "Docs size: " << docs.getSize() << " bytes\n";
    std::cout << "Images size: " << images.getSize() << " bytes\n";
    
    // 访问子元素
    std::cout << "\n=== Children ===\n";
    std::cout << "Root has " << root.getChildCount() << " children\n";
    std::cout << "Images has " << images.getChildCount() << " children\n";
    
    return 0;
}
```

## 现代 C++ 实现

### 使用智能指针

```cpp
class Component {
public:
    virtual ~Component() = default;
    virtual void operation() = 0;
};

class Leaf : public Component {
public:
    void operation() override {
        std::cout << "Leaf operation\n";
    }
};

class Composite : public Component {
public:
    void add(std::unique_ptr<Component> component) {
        children_.push_back(std::move(component));
    }
    
    void operation() override {
        for (auto& child : children_) {
            child->operation();
        }
    }

private:
    std::vector<std::unique_ptr<Component>> children_;
};
```

### 使用 CRTP 简化实现

```cpp
template<typename Derived>
class ComponentBase {
public:
    void operation() {
        static_cast<Derived*>(this)->doOperation();
    }
};

class Leaf : public ComponentBase<Leaf> {
public:
    void doOperation() {
        std::cout << "Leaf operation\n";
    }
};

class Composite : public ComponentBase<Composite> {
public:
    void add(std::shared_ptr<ComponentBase<Composite>> child) {
        children_.push_back(std::move(child));
    }
    
    void doOperation() {
        for (auto& child : children_) {
            child->operation();
        }
    }

private:
    std::vector<std::shared_ptr<ComponentBase<Composite>>> children_;
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **装饰器模式** | 装饰器增强功能；组合管理层次结构 |
| **迭代器模式** | 迭代器遍历集合；组合表示层次结构 |
| **适配器模式** | 适配器转换接口；组合统一接口 |

## 最佳实践

### 1. 使用透明接口

```cpp
// 推荐：所有组件都支持 add/remove
class TransparentComponent {
public:
    virtual void add(Component* c) {}  // 叶子节点忽略
    virtual void remove(Component* c) {}  // 叶子节点忽略
};

// 不推荐：在叶子节点抛出异常
class OpaqueLeaf : public Component {
public:
    void add(Component* c) override {
        throw std::logic_error("Cannot add to leaf");
    }
};
```

### 2. 使用访问者模式

```cpp
class Visitor {
public:
    virtual void visit(File* file) = 0;
    virtual void visit(Directory* dir) = 0;
    virtual ~Visitor() = default;
};

class SizeCalculator : public Visitor {
public:
    void visit(File* file) override {
        totalSize += file->getSize();
    }
    
    void visit(Directory* dir) override {
        for (auto* child : dir->getChildren()) {
            child->accept(this);
        }
    }
    
    size_t getTotalSize() const { return totalSize; }

private:
    size_t totalSize = 0;
};
```

### 3. 支持部分操作

```cpp
class PartialComposite {
public:
    // 只在组合节点支持
    void addChild(std::unique_ptr<Component> child) {
        if (isComposite()) {
            children_.push_back(std::move(child));
        }
    }
    
    // 检查是否是组合节点
    bool isComposite() const { return true; }
    bool isLeaf() const { return false; }

private:
    std::vector<std::unique_ptr<Component>> children_;
};
```

## 常见陷阱

### 陷阱 1：循环引用

```cpp
// 危险：组件互相引用
class Node {
public:
    void addChild(std::shared_ptr<Node> child) {
        children_.push_back(child);
        child->parent_ = shared_from_this();  // 循环引用！
    }

private:
    std::vector<std::shared_ptr<Node>> children_;
    std::shared_ptr<Node> parent_;  // 应该用 weak_ptr
};

// 推荐：使用弱引用
class SafeNode {
public:
    void addChild(std::shared_ptr<SafeNode> child) {
        children_.push_back(child);
        child->parent_ = std::weak_ptr<SafeNode>(shared_from_this());
    }

private:
    std::vector<std::shared_ptr<SafeNode>> children_;
    std::weak_ptr<SafeNode> parent_;  // 避免循环引用
};
```

### 陷阱 2：内存泄漏

```cpp
// 不推荐：裸指针管理
class BadComposite {
public:
    void add(Component* component) {
        children_.push_back(component);  // 谁负责 delete？
    }

private:
    std::vector<Component*> children_;  // 可能泄漏
};

// 推荐：智能指针管理
class GoodComposite {
public:
    void add(std::unique_ptr<Component> component) {
        children_.push_back(std::move(component));
    }

private:
    std::vector<std::unique_ptr<Component>> children_;
};
```

### 陷阱 3：操作不一致

```cpp
// 不推荐：叶子节点和组合节点行为不一致
class Leaf {
public:
    void operation() { /* ... */ }
};

class Composite {
public:
    void operation() { 
        for (auto* child : children_) {
            child->operation();
        }
    }
};

// 推荐：使用统一的接口
class Component {
public:
    virtual void operation() = 0;
};
```

## 总结

组合模式的核心要点：

- **适用场景**：树形结构、部分-整体层次、需要统一处理
- **核心优点**：统一接口、递归处理、易于扩展
- **实现要点**：透明接口、使用智能指针、避免循环引用
- **常见陷阱**：循环引用、内存泄漏、操作不一致

> **记住**：组合模式的关键是"统一处理"——让客户端可以统一地处理单个对象和组合对象，无需关心它们的区别。
