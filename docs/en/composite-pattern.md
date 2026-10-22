# Composite Pattern

## What Is the Composite Pattern?

The Composite Pattern is a structural design pattern that assembles objects
into a tree structure to represent "part-whole" hierarchies. It lets clients
treat a single object and a composite object uniformly.

> **Core idea**: let clients handle single objects and composite objects
> alike, without having to distinguish between them.

## Why Do We Need It?

### Problem Scenario

Suppose we have a graphics editor that has to handle all kinds of shapes:

```cpp
// Problem: single shapes and shape groups must be handled separately
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

// The problem shows up when handling a list of shapes
void drawAll(const std::vector<Shape*>& shapes) {
    for (auto* shape : shapes) {
        shape->draw();  // must tell single shapes and groups apart
    }
}
```

**Problems**:

1. Single objects and composite objects must be handled separately
2. Client code becomes complicated
3. Recursive processing is hard

### Solution

Use the composite pattern:

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
    void draw() override { /* draws a single shape */ }
};
```

**Benefits**:

1. Uniform interface: clients never distinguish single objects from composite ones
2. Recursive processing: components can be combined at any depth
3. Easy to extend: new component types are trivial to add

## Pattern Structure

```
                    ┌─────────────┐
                    │  Component  │
                    │  (element)  │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Leaf       │ │  Leaf       │ │  Composite  │ ← composite
    │  (leaf)     │ │  (leaf)     │ │  (composite)│
    └─────────────┘ └─────────────┘ └──────┬──────┘
                                           │
                        ┌──────────────────┼──────────────────┐
                        │                  │                  │
                 ┌──────▼──────┐   ┌──────▼──────┐   ┌──────▼──────┐
                 │  Leaf       │   │  Leaf       │   │  Leaf       │
                 └─────────────┘   └─────────────┘   └─────────────┘
```

## Implementation

### 1. Define the Component Interface

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

    // composite operations
    virtual void add(FileSystemComponent* component) {}
    virtual void remove(FileSystemComponent* component) {}
    virtual FileSystemComponent* getChild(int index) { return nullptr; }

    virtual std::string getName() const = 0;
};
```

### 2. Implement the Leaf Node

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

### 3. Implement the Composite Node

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

### 4. Client Usage

```cpp
int main() {
    // create the file system structure
    Directory root("root");
    Directory docs("documents");
    Directory images("images");
    File readme("readme.txt", 1024);
    File photo("photo.jpg", 2048000);
    File logo("logo.png", 51200);

    // build the directory tree
    root.add(&docs);
    root.add(&images);
    docs.add(&readme);
    images.add(&photo);
    images.add(&logo);

    // display the structure
    std::cout << "=== File System ===\n";
    root.display();

    // collect the sizes
    std::cout << "\n=== Sizes ===\n";
    std::cout << "Root size: " << root.getSize() << " bytes\n";
    std::cout << "Docs size: " << docs.getSize() << " bytes\n";
    std::cout << "Images size: " << images.getSize() << " bytes\n";

    // reach the children
    std::cout << "\n=== Children ===\n";
    std::cout << "Root has " << root.getChildCount() << " children\n";
    std::cout << "Images has " << images.getChildCount() << " children\n";

    return 0;
}
```

## Modern C++ Implementations

### With Smart Pointers

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

### Simplifying with CRTP

```cpp
// the common base lets a leaf and a composite share one container
class Component {
public:
    virtual ~Component() = default;
    virtual void operation() = 0;
};

// CRTP: operation() dispatches to the derived type
template<typename Derived>
class ComponentBase : public Component {
public:
    void operation() override {
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
    void add(std::shared_ptr<Component> child) {
        children_.push_back(std::move(child));
    }

    void doOperation() {
        for (auto& child : children_) {
            child->operation();
        }
    }

private:
    std::vector<std::shared_ptr<Component>> children_;
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Decorator** | A decorator adds behavior; a composite manages a hierarchy |
| **Iterator** | An iterator traverses a collection; a composite represents a hierarchy |
| **Adapter** | An adapter converts an interface; a composite unifies an interface |

## Best Practices

### 1. Use a Transparent Interface

```cpp
// Recommended: every component supports add/remove
class TransparentComponent {
public:
    virtual void add(Component* c) {}  // leaves ignore it
    virtual void remove(Component* c) {}  // leaves ignore it
};

// Not recommended: throwing from a leaf
class OpaqueLeaf : public Component {
public:
    void add(Component* c) override {
        throw std::logic_error("Cannot add to leaf");
    }
};
```

### 2. Combine with the Visitor Pattern

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

### 3. Support Partial Operations

```cpp
class PartialComposite {
public:
    // supported on composite nodes only
    void addChild(std::unique_ptr<Component> child) {
        if (isComposite()) {
            children_.push_back(std::move(child));
        }
    }

    // check whether this is a composite node
    bool isComposite() const { return true; }
    bool isLeaf() const { return false; }

private:
    std::vector<std::unique_ptr<Component>> children_;
};
```

## Common Traps

### Trap 1: Reference Cycles

```cpp
// Danger: the components reference each other
class Node : public std::enable_shared_from_this<Node> {
public:
    void addChild(std::shared_ptr<Node> child) {
        children_.push_back(child);
        child->parent_ = shared_from_this();  // reference cycle!
    }

private:
    std::vector<std::shared_ptr<Node>> children_;
    std::shared_ptr<Node> parent_;  // should be a weak_ptr
};

// Recommended: use a weak reference
class SafeNode : public std::enable_shared_from_this<SafeNode> {
public:
    void addChild(std::shared_ptr<SafeNode> child) {
        children_.push_back(child);
        child->parent_ = std::weak_ptr<SafeNode>(shared_from_this());
    }

private:
    std::vector<std::shared_ptr<SafeNode>> children_;
    std::weak_ptr<SafeNode> parent_;  // avoids the reference cycle
};
```

### Trap 2: Memory Leaks

```cpp
// Not recommended: raw pointer management
class BadComposite {
public:
    void add(Component* component) {
        children_.push_back(component);  // who deletes it?
    }

private:
    std::vector<Component*> children_;  // may leak
};

// Recommended: smart pointer management
class GoodComposite {
public:
    void add(std::unique_ptr<Component> component) {
        children_.push_back(std::move(component));
    }

private:
    std::vector<std::unique_ptr<Component>> children_;
};
```

### Trap 3: Inconsistent Operations

```cpp
// Not recommended: leaves and composites behave differently
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

// Recommended: use a uniform interface
class Component {
public:
    virtual void operation() = 0;
};
```

## Example

A runnable version of this topic:

- [composite-demo.cpp](../../examples/composite-demo.cpp)

## Summary

The key points of the composite pattern:

- **When to use it**: tree structures, part-whole hierarchies, a need for
  uniform handling
- **Main benefits**: a uniform interface, recursive processing, easy extension
- **Implementation advice**: a transparent interface, smart pointers, no
  reference cycles
- **Common traps**: reference cycles, memory leaks, inconsistent operations

> **Remember**: the key of the composite pattern is *uniform handling* —
> clients process single objects and composite objects the same way, without
> caring about the difference.
