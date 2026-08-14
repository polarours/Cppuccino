# 迭代器模式 (Iterator Pattern)

## 什么是迭代器模式？

迭代器模式（Iterator Pattern）是一种行为型设计模式，它提供一种方法顺序访问一个聚合对象中各个元素，而又不暴露该对象的内部表示。

> **核心思想**：将遍历逻辑从集合中分离出来，使遍历算法可以独立于容器变化。

## 为什么需要迭代器模式？

### 问题场景

假设我们有多种数据结构需要遍历：

```cpp
// 问题：每个容器都有相同的遍历逻辑，但实现不同
std::vector<int> vec = {1, 2, 3, 4, 5};
for (size_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i] << " ";
}

std::list<int> lst = {1, 2, 3, 4, 5};
for (auto it = lst.begin(); it != lst.end(); ++it) {
    std::cout << *it << " ";
}

std::map<int, std::string> mp = {{1, "one"}, {2, "two"}};
for (auto it = mp.begin(); it != mp.end(); ++it) {
    std::cout << it->first << ": " << it->second << "\n";
}
```

**问题**：
1. 遍历逻辑重复
2. 客户端需要了解容器内部结构
3. 难以统一处理不同类型的容器

### 解决方案

使用迭代器模式：

```cpp
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool isDone() const = 0;
    virtual currentItem() const = 0;
};

class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual Iterator* createIterator() const = 0;
};

// 客户端统一使用迭代器
void printAll(Iterator* iterator) {
    for (iterator->first(); !iterator->isDone(); iterator->next()) {
        std::cout << iterator->currentItem() << " ";
    }
    std::cout << "\n";
}
```

**优点**：
1. 统一接口：不同容器使用相同的遍历方式
2. 解耦：客户端不依赖具体容器
3. 可扩展：可以添加新的遍历策略

## 模式结构

```
                    ┌─────────────┐
                    │  Iterator   │
                    │  (迭代器)    │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  ListIter   │ │ VectorIter  │ │  MapIter    │ ← 具体迭代器
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │      Aggregate          │ ← 聚合对象
              │   (Container)           │
              └────────────┬────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← 客户端
                    └─────────────────┘
```

## 代码实现

### 1. 定义迭代器接口

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>
#include <list>
#include <map>

class Iterator {
public:
    virtual ~Iterator() = default;
    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool isDone() const = 0;
    virtual std::string currentItem() const = 0;
};
```

### 2. 定义聚合接口

```cpp
class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual Iterator* createIterator() const = 0;
};
```

### 3. 实现具体迭代器

```cpp
// 列表迭代器
class ListIterator : public Iterator {
public:
    explicit ListIterator(const std::list<std::string>& list)
        : list_(list), current_(list_.begin()) {}
    
    void first() override {
        current_ = list_.begin();
    }
    
    void next() override {
        ++current_;
    }
    
    bool isDone() const override {
        return current_ == list_.end();
    }
    
    std::string currentItem() const override {
        return *current_;
    }

private:
    const std::list<std::string>& list_;
    std::list<std::string>::const_iterator current_;
};

// 向量迭代器
class VectorIterator : public Iterator {
public:
    explicit VectorIterator(const std::vector<std::string>& vec)
        : vec_(vec), index_(0) {}
    
    void first() override {
        index_ = 0;
    }
    
    void next() override {
        ++index_;
    }
    
    bool isDone() const override {
        return index_ >= vec_.size();
    }
    
    std::string currentItem() const override {
        return vec_[index_];
    }

private:
    const std::vector<std::string>& vec_;
    size_t index_;
};

// 地图迭代器
class MapIterator : public Iterator {
public:
    explicit MapIterator(const std::map<int, std::string>& map)
        : map_(map), current_(map_.begin()) {}
    
    void first() override {
        current_ = map_.begin();
    }
    
    void next() override {
        ++current_;
    }
    
    bool isDone() const override {
        return current_ == map_.end();
    }
    
    std::string currentItem() const override {
        return current_->second;
    }
    
    int currentKey() const {
        return current_->first;
    }

private:
    const std::map<int, std::string>& map_;
    std::map<int, std::string>::const_iterator current_;
};
```

### 4. 实现具体聚合

```cpp
// 字符串集合
class StringCollection : public Aggregate {
public:
    void add(const std::string& item) {
        items_.push_back(item);
    }
    
    Iterator* createIterator() const override {
        return new VectorIterator(items_);
    }

private:
    std::vector<std::string> items_;
};

// 书籍集合
class BookCollection : public Aggregate {
public:
    void addBook(int id, const std::string& title) {
        books_[id] = title;
    }
    
    Iterator* createIterator() const override {
        return new MapIterator(books_);
    }

private:
    std::map<int, std::string> books_;
};
```

### 5. 客户端使用

```cpp
void printAll(Aggregate* collection) {
    std::unique_ptr<Iterator> iterator(collection->createIterator());
    
    for (iterator->first(); !iterator->isDone(); iterator->next()) {
        std::cout << iterator->currentItem() << " ";
    }
    std::cout << "\n";
}

int main() {
    // 使用字符串集合
    StringCollection collection;
    collection.add("Apple");
    collection.add("Banana");
    collection.add("Cherry");
    
    std::cout << "Fruits: ";
    printAll(&collection);
    
    // 使用书籍集合
    BookCollection bookCollection;
    bookCollection.addBook(1, "C++ Primer");
    bookCollection.addBook(2, "Design Patterns");
    bookCollection.addBook(3, "Clean Code");
    
    std::cout << "Books: ";
    printAll(&bookCollection);
    
    return 0;
}
```

## 现代 C++ 实现

### 使用 range-based for

```cpp
class RangeIterable {
public:
    class Iterator {
    public:
        Iterator(int start, int end) : current_(start), end_(end) {}
        
        int operator*() const { return current_; }
        Iterator& operator++() { ++current_; return *this; }
        bool operator!=(const Iterator& other) const { return current_ != other.current_; }
        
    private:
        int current_;
        int end_;
    };
    
    Iterator begin() const { return Iterator(0, 10); }
    Iterator end() const { return Iterator(10, 10); }
};

// 使用 range-based for
RangeIterable range;
for (int x : range) {
    std::cout << x << " ";
}
```

### 使用 STL 迭代器

```cpp
template<typename Container>
void printContainer(const Container& container) {
    for (auto it = container.begin(); it != container.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

// 或使用 C++11 range-based for
template<typename Container>
void printContainerModern(const Container& container) {
    for (const auto& item : container) {
        std::cout << item << " ";
    }
    std::cout << "\n";
}
```

### 使用自定义迭代器适配器

```cpp
template<typename InputIt, typename TransformFn>
class TransformIterator : public Iterator {
public:
    TransformIterator(InputIt begin, InputIt end, TransformFn transform)
        : begin_(begin), end_(end), current_(begin), transform_(std::move(transform)) {}
    
    void first() override { current_ = begin_; }
    void next() override { ++current_; }
    bool isDone() const override { return current_ == end_; }
    std::string currentItem() const override { return transform_(*current_); }

private:
    InputIt begin_;
    InputIt end_;
    InputIt current_;
    TransformFn transform_;
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **迭代器 vs STL** | STL 迭代器更强大，支持更多操作 |
| **迭代器 vs range** | C++11 range-based for 更简洁 |
| **迭代器 vs 访问者** | 访问者访问数据，迭代器遍历数据 |

## 最佳实践

### 1. 使用智能指针管理迭代器

```cpp
class SafeIterator {
public:
    std::unique_ptr<Iterator> createIterator() const {
        return std::make_unique<ListIterator>(list_);
    }
};
```

### 2. 支持双向迭代

```cpp
class BidirectionalIterator : public Iterator {
public:
    virtual void previous() = 0;
    virtual bool isFirst() const = 0;
    virtual bool isLast() const = 0;
};
```

### 3. 使用常量迭代器

```cpp
class ConstIterator : public Iterator {
public:
    const ItemType& currentItem() const override {
        return *current_;
    }
    
protected:
    ConstIterator(const ItemType* begin, const ItemType* end)
        : begin_(begin), end_(end), current_(begin) {}
    
private:
    const ItemType* begin_;
    const ItemType* end_;
    const ItemType* current_;
};
```

## 常见陷阱

### 陷阱 1：迭代器失效

```cpp
// 不推荐：迭代器可能失效
std::vector<int> vec = {1, 2, 3, 4, 5};
auto it = vec.begin();
vec.push_back(6);  // 可能导致重新分配，迭代器失效！
std::cout << *it << "\n";  // 未定义行为

// 推荐：使用索引或重新获取迭代器
for (size_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i] << "\n";
}
```

### 陷阱 2：内存泄漏

```cpp
// 不推荐：忘记删除迭代器
Iterator* iter = collection->createIterator();
iter->first();
while (!iter->isDone()) {
    std::cout << iter->currentItem() << " ";
    iter->next();
}
delete iter;  // 必须手动删除

// 推荐：使用智能指针
auto iter = std::make_unique<Iterator>(collection->createIterator());
```

### 陷阱 3：迭代器与容器耦合

```cpp
// 不推荐：迭代器依赖容器内部结构
class BadIterator {
public:
    explicit BadIterator(std::vector<int>& vec) : vec_(vec), index_(0) {}
    
    void next() { ++index_; }
    int current() { return vec_[index_]; }  // 直接访问内部
    
private:
    std::vector<int>& vec_;  // 依赖具体容器
    int index_;
};

// 推荐：迭代器独立于容器
class GoodIterator {
public:
    explicit GoodIterator(const ItemType* begin, const ItemType* end)
        : current_(begin), end_(end) {}
    
    void next() { ++current_; }
    ItemType current() const { return *current_; }
    
private:
    const ItemType* current_;
    const ItemType* end_;
};
```

## 总结

迭代器模式的核心要点：

- **适用场景**：需要统一遍历接口、隐藏容器内部结构、支持多种遍历策略
- **核心优点**：解耦、统一接口、易于扩展
- **实现要点**：使用智能指针、支持双向迭代、避免迭代器失效
- **常见陷阱**：迭代器失效、内存泄漏、过度耦合

> **记住**：迭代器模式的关键是"统一遍历"——提供一种统一的方式遍历不同类型的集合，而无需暴露其内部结构。
