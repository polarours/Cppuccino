# Prototype Pattern

## What Is the Prototype Pattern?

The Prototype Pattern is a creational design pattern that creates new objects
by copying existing ones instead of going through the `new` keyword. It is
useful when creating an object directly is expensive.

> **Core idea**: create new objects by cloning a prototype, avoiding the
> repeated cost of initialization.

## Why Do We Need It?

### Problem Scenario

Suppose we have a complex graph object that is expensive to create:

```cpp
// Problem: every creation has to initialize everything again
class ComplexGraph {
public:
    ComplexGraph() {
        // initialize the complex graph data
        loadFromFile("huge_graph.dat");
        computeLayout();
        buildRenderingTree();
    }
    
    ComplexGraph(const ComplexGraph& other) {
        // deep-copy all the data
        data_ = other.data_;
        layout_ = other.layout_;
        // ...
    }
};

// We need several similar objects
auto graph1 = std::make_unique<ComplexGraph>();
auto graph2 = std::make_unique<ComplexGraph>(*graph1);  // copy
auto graph3 = std::make_unique<ComplexGraph>(*graph1);  // copy
```

**Problems**:

1. Creation is expensive
2. Initialization is complicated
3. Many copies are needed

### Solution

Use the prototype pattern:

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

// usage
auto prototype = std::make_unique<ComplexGraph>();
prototype->loadFromFile("huge_graph.dat");

auto copy1 = prototype->clone();
auto copy2 = prototype->clone();
```

**Benefits**:

1. Avoids repeated initialization
2. The type to create is decided at runtime
3. Simplifies the creation process

## Pattern Structure

```
                    ┌─────────────┐
                    │  Prototype  │
                    │ (abstract)  │
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │     ConcretePrototype   │ ← concrete prototype
              │   (ComplexGraph)        │
              └────────────┬────────────┘
                           │ clone()
                           │
              ┌────────────▼────────────┐
              │      Client             │ ← client
              │  (uses clone to create) │
              └─────────────────────────┘
```

## Implementation

### 1. Define the Prototype Interface

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

### 2. Implement the Concrete Prototypes

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

### 3. Implement the Prototype Registry

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

### 4. Client Usage

```cpp
int main() {
    PrototypeRegistry registry;
    
    // register the prototypes
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
    
    // list the available prototypes
    registry.listTypes();
    
    // create a clone
    auto doc1 = registry.create("Text");
    doc1->setContent("Modified text content");
    std::cout << "\nCreated: " << doc1->getType() << "\n";
    std::cout << "Content: " << doc1->getContent() << "\n";
    
    auto doc2 = registry.create("Image");
    std::cout << "\nCreated: " << doc2->getType() << "\n";
    
    auto doc3 = registry.create("Spreadsheet");
    std::cout << "\nCreated: " << doc3->getType() << "\n";
    auto sheetClone = std::static_pointer_cast<SpreadsheetDocument>(doc3);
    std::cout << "Cell(0,0): " << sheetClone->getCell(0, 0) << "\n";
    
    // verify the deep copy
    auto textClone = std::static_pointer_cast<TextDocument>(doc1);
    std::cout << "\nFont size: " << textClone->getFontSize() << "\n";
    
    return 0;
}
```

## Modern C++ Implementations

### Simplifying Cloning with CRTP

```cpp
template<typename Derived>
class Cloneable {
public:
    std::shared_ptr<Derived> clone() const {
        return std::make_shared<Derived>(static_cast<const Derived&>(*this));
    }
};

class MyDocument : public Cloneable<MyDocument> {
    // gets the clone() method for free
};
```

### Using a Factory Function

```cpp
template<typename T>
std::shared_ptr<T> clonePrototype(const std::shared_ptr<T>& prototype) {
    return std::make_shared<T>(*prototype);
}

// usage
auto clone = clonePrototype<TextDocument>(textProto);
```

### Using a Custom Clone Strategy

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

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Factory** | A factory creates a new object; a prototype creates one by copying |
| **Builder** | A builder assembles step by step; a prototype creates by copying |
| **Singleton** | A singleton has one instance; a prototype can produce many clones |

## Best Practices

### 1. Implement Deep Copies

```cpp
class DeepCloneable {
public:
    virtual std::shared_ptr<DeepCloneable> clone() const = 0;
    
protected:
    // make sure the copy is a deep copy
    DeepCloneable(const DeepCloneable& other) 
        : data_(other.data_) {}  // deep copy
    
private:
    std::vector<int> data_;
};
```

### 2. Cache Clones in an Object Pool

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

### 3. Support Partial Cloning

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

## Common Traps

### Trap 1: The Shallow Copy Problem

```cpp
// Not recommended: a shallow copy can leave dangling pointers
class BadPrototype {
public:
    std::unique_ptr<BadPrototype> clone() const {
        auto clone = std::make_unique<BadPrototype>(*this);
        clone->data_ = data_;  // shallow copy of the pointer
        return clone;
    }
    
private:
    int* data_ = nullptr;  // needs a deep copy
};

// Recommended: deep copy or smart pointers
class GoodPrototype {
public:
    std::unique_ptr<GoodPrototype> clone() const {
        return std::make_unique<GoodPrototype>(*this);
    }
    
private:
    std::unique_ptr<int[]> data_;  // deep copy for free
};
```

### Trap 2: Reference Cycles

```cpp
// Danger: a reference cycle causes a memory leak
class Node {
public:
    std::shared_ptr<Node> clone() const {
        auto clone = std::make_shared<Node>(*this);
        if (parent_) {
            clone->parent_ = parent_->clone();  // reference cycle
        }
        return clone;
    }
    
private:
    std::shared_ptr<Node> parent_;
};
```

### Trap 3: Forgetting to Implement clone()

```cpp
// Not recommended: forgetting to implement clone()
class IncompletePrototype {
public:
    virtual std::shared_ptr<IncompletePrototype> clone() const {
        // forgot to implement it!
        return nullptr;
    }
};

// Recommended: use CRTP to make the implementation mandatory
template<typename Derived>
class Cloneable {
public:
    std::shared_ptr<Derived> clone() const {
        return std::make_shared<Derived>(static_cast<const Derived&>(*this));
    }
};
```

## Summary

The key points of the prototype pattern:

- **When to use it**: expensive creation, a need to create types dynamically,
  a need to cache objects
- **Main benefits**: avoids repeated initialization, supports runtime
  polymorphism, simplifies creation
- **Implementation advice**: deep copies, smart pointers, consider caching
- **Common traps**: shallow copies, reference cycles, an unimplemented
  `clone()`

> **Remember**: the key of the prototype pattern is *cloning* — create new
> objects by copying existing ones, so the repeated cost of initialization
> disappears.
