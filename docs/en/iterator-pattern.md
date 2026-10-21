# Iterator Pattern

## What Is the Iterator Pattern?

The Iterator Pattern is a behavioral design pattern that provides a way to
access the elements of an aggregate object sequentially without exposing the
object's internal representation.

> **Core idea**: separate the traversal logic from the collection, so that
> traversal algorithms can evolve independently of the container.

## Why Do We Need It?

### Problem Scenario

Suppose we have several data structures that all need to be traversed:

```cpp
// Problem: each container repeats the same traversal logic, but differently
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

**Problems**:

1. The traversal logic is duplicated
2. The client has to know the internal structure of the container
3. Different container types are hard to handle uniformly

### Solution

Use the iterator pattern:

```cpp
class Iterator {
public:
    virtual ~Iterator() = default;
    virtual void first() = 0;
    virtual void next() = 0;
    virtual bool isDone() const = 0;
    virtual std::string currentItem() const = 0;
};

class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual Iterator* createIterator() const = 0;
};

// the client uses the iterator uniformly
void printAll(Iterator* iterator) {
    for (iterator->first(); !iterator->isDone(); iterator->next()) {
        std::cout << iterator->currentItem() << " ";
    }
    std::cout << "\n";
}
```

**Benefits**:

1. Uniform interface: different containers are traversed in the same way
2. Decoupling: the client does not depend on a concrete container
3. Extensible: new traversal strategies can be added

## Pattern Structure

```
                    ┌─────────────┐
                    │  Iterator   │
                    │  (iterator) │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  ListIter   │ │ VectorIter  │ │  MapIter    │ ← concrete iterators
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │      Aggregate          │ ← the aggregate
              │   (Container)           │
              └────────────┬────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← the client
                    └─────────────────┘
```

## Implementation

### 1. Define the Iterator Interface

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

### 2. Define the Aggregate Interface

```cpp
class Aggregate {
public:
    virtual ~Aggregate() = default;
    virtual Iterator* createIterator() const = 0;
};
```

### 3. Implement the Concrete Iterators

```cpp
// list iterator
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

// vector iterator
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

// map iterator
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

### 4. Implement the Concrete Aggregates

```cpp
// a collection of strings
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

// a collection of books
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

### 5. Client Usage

```cpp
void printAll(Aggregate* collection) {
    std::unique_ptr<Iterator> iterator(collection->createIterator());
    
    for (iterator->first(); !iterator->isDone(); iterator->next()) {
        std::cout << iterator->currentItem() << " ";
    }
    std::cout << "\n";
}

int main() {
    // a collection of strings
    StringCollection collection;
    collection.add("Apple");
    collection.add("Banana");
    collection.add("Cherry");
    
    std::cout << "Fruits: ";
    printAll(&collection);
    
    // a collection of books
    BookCollection bookCollection;
    bookCollection.addBook(1, "C++ Primer");
    bookCollection.addBook(2, "Design Patterns");
    bookCollection.addBook(3, "Clean Code");
    
    std::cout << "Books: ";
    printAll(&bookCollection);
    
    return 0;
}
```

## Modern C++ Implementations

### With Range-Based for

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

// usage with range-based for
RangeIterable range;
for (int x : range) {
    std::cout << x << " ";
}
```

### With STL Iterators

```cpp
template<typename Container>
void printContainer(const Container& container) {
    for (auto it = container.begin(); it != container.end(); ++it) {
        std::cout << *it << " ";
    }
    std::cout << "\n";
}

// or use the C++11 range-based for
template<typename Container>
void printContainerModern(const Container& container) {
    for (const auto& item : container) {
        std::cout << item << " ";
    }
    std::cout << "\n";
}
```

### With a Custom Iterator Adapter

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

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Iterator vs STL** | STL iterators are stronger and support more operations |
| **Iterator vs range** | The C++11 range-based for is more concise |
| **Iterator vs Visitor** | The visitor accesses data; the iterator traverses data |

## Best Practices

### 1. Manage Iterators with Smart Pointers

```cpp
class SafeIterator {
public:
    explicit SafeIterator(const std::list<std::string>& list) : list_(list) {}

    std::unique_ptr<Iterator> createIterator() const {
        return std::make_unique<ListIterator>(list_);
    }

private:
    const std::list<std::string>& list_;
};
```

### 2. Support Bidirectional Iteration

```cpp
class BidirectionalIterator : public Iterator {
public:
    virtual void previous() = 0;
    virtual bool isFirst() const = 0;
    virtual bool isLast() const = 0;
};
```

### 3. Use Constant Iterators

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

## Common Traps

### Trap 1: Iterator Invalidation

```cpp
// Not recommended: the iterator may be invalidated
std::vector<int> vec = {1, 2, 3, 4, 5};
auto it = vec.begin();
vec.push_back(6);  // may reallocate — the iterator is invalidated!
std::cout << *it << "\n";  // undefined behavior

// Recommended: use an index, or reacquire the iterator
for (size_t i = 0; i < vec.size(); ++i) {
    std::cout << vec[i] << "\n";
}
```

### Trap 2: Memory Leaks

```cpp
// Not recommended: forgetting to delete the iterator
Iterator* iter = collection->createIterator();
iter->first();
while (!iter->isDone()) {
    std::cout << iter->currentItem() << " ";
    iter->next();
}
delete iter;  // must be deleted manually

// Recommended: use smart pointers
std::unique_ptr<Iterator> owned(collection->createIterator());
```

### Trap 3: Iterators Coupled to the Container

```cpp
// Not recommended: the iterator depends on the container's internals
class BadIterator {
public:
    explicit BadIterator(std::vector<int>& vec) : vec_(vec), index_(0) {}
    
    void next() { ++index_; }
    int current() { return vec_[index_]; }  // reaches into the internals directly
    
private:
    std::vector<int>& vec_;  // depends on a concrete container
    int index_;
};

// Recommended: the iterator is independent of the container
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

## Summary

The key points of the iterator pattern:

- **When to use it**: a uniform traversal interface is needed, the internals
  of the container must stay hidden, or several traversal strategies are
  required
- **Main benefits**: decoupling, a uniform interface, easy to extend
- **Implementation advice**: use smart pointers, support bidirectional
  iteration, avoid iterator invalidation
- **Common traps**: iterator invalidation, memory leaks, tight coupling

> **Remember**: the key of the iterator pattern is *"uniform traversal"* —
> provide one way to walk over collections of different types without
> exposing their internal structure.
