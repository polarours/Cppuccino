# Proxy Pattern

## What Is the Proxy Pattern?

The Proxy Pattern is a structural design pattern that provides an object that
stands in for another one and controls access to it. The proxy can add extra
logic — lazy initialization, access control, logging, and so on.

> **Core idea**: control access to the real object through a proxy, and run
> additional logic before and after every access.

## Why Do We Need It?

### Problem Scenario

Suppose we have an image loading system where the images are huge and slow to
load:

```cpp
// Problem: the image is loaded on every access
class Image {
public:
    virtual void display() {
        loadFromFile("large_image.jpg");  // very slow!
        render();
    }

private:
    void loadFromFile(const std::string& path) {
        // simulate a slow operation
        std::this_thread::sleep_for(std::chrono::seconds(2));
    }

    void render() {
        std::cout << "Rendering image...\n";
    }
};

// Problem: the user stares at a blank screen
Image img;
img.display();  // takes two seconds
```

**Problems**:

1. Lazy loading hurts the user experience
2. Access cannot be controlled
3. Extra logic cannot be added

### Solution

Use the proxy pattern:

```cpp
class ImageProxy : public Image {
public:
    explicit ImageProxy(const std::string& path) : path_(path), loaded_(false) {}

    void display() override {
        if (!loaded_) {
            loadRealImage();  // lazy loading
        }
        realImage_->display();
    }

private:
    void loadRealImage() {
        realImage_ = std::make_unique<RealImage>(path_);
        loaded_ = true;
    }

    std::unique_ptr<RealImage> realImage_;
    std::string path_;
    bool loaded_;
};
```

**Benefits**:

1. Lazy loading improves performance
2. Extra control logic can be added
3. Transparent to the user

## Pattern Structure

```
                    ┌─────────────┐
                    │   Subject   │
                    │  (interface)│
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  RealObject │ │  Proxy      │ │  Virtual    │
    │  (real)     │ │  (proxy)    │ │  (virtual)  │
    └─────────────┘ └─────────────┘ └─────────────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← client
                    └─────────────────┘
```

## Implementation

### 1. Define the Subject Interface

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>

class DataSource {
public:
    virtual ~DataSource() = default;
    virtual std::string getData() = 0;
    virtual void setData(const std::string& data) = 0;
};
```

### 2. Implement the Real Object

```cpp
class RealDataSource : public DataSource {
public:
    explicit RealDataSource(const std::string& path) : path_(path) {}

    std::string getData() override {
        std::cout << "Loading data from " << path_ << "...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));  // simulate latency
        return "Data from " + path_;
    }

    void setData(const std::string& data) override {
        std::cout << "Saving data to " << path_ << "...\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        data_ = data;
    }

    std::string getPath() const { return path_; }

private:
    std::string path_;
    std::string data_;
};
```

### 3. Implement the Virtual Proxy (Lazy Loading)

```cpp
class LazyProxy : public DataSource {
public:
    explicit LazyProxy(const std::string& path)
        : path_(path), real_(nullptr) {}

    std::string getData() override {
        if (!real_) {
            real_ = std::make_unique<RealDataSource>(path_);
        }
        return real_->getData();
    }

    void setData(const std::string& data) override {
        if (!real_) {
            real_ = std::make_unique<RealDataSource>(path_);
        }
        real_->setData(data);
    }

    bool isLoaded() const { return real_ != nullptr; }

private:
    std::string path_;
    std::unique_ptr<RealDataSource> real_;
};
```

### 4. Implement the Protection Proxy (Access Control)

```cpp
class AccessProxy : public DataSource {
public:
    AccessProxy(std::unique_ptr<RealDataSource> real, const std::string& user)
        : real_(std::move(real)), user_(user) {}

    std::string getData() override {
        if (!canRead()) {
            throw std::runtime_error("Access denied for user: " + user_);
        }
        return real_->getData();
    }

    void setData(const std::string& data) override {
        if (!canWrite()) {
            throw std::runtime_error("Write denied for user: " + user_);
        }
        real_->setData(data);
    }

    bool canRead() const {
        return user_ == "admin" || user_ == "editor";
    }

    bool canWrite() const {
        return user_ == "admin";
    }

private:
    std::unique_ptr<RealDataSource> real_;
    std::string user_;
};
```

### 5. Implement the Cache Proxy

```cpp
class CacheProxy : public DataSource {
public:
    explicit CacheProxy(std::unique_ptr<RealDataSource> real)
        : real_(std::move(real)) {}

    std::string getData() override {
        if (cache_.empty()) {
            cache_ = real_->getData();
            std::cout << "[Cache] Data loaded and cached\n";
        } else {
            std::cout << "[Cache] Returning cached data\n";
        }
        return cache_;
    }

    void setData(const std::string& data) override {
        real_->setData(data);
        cache_.clear();  // invalidate the cache
        std::cout << "[Cache] Cache invalidated\n";
    }

    void clearCache() {
        cache_.clear();
        std::cout << "[Cache] Cache cleared\n";
    }

    size_t cacheSize() const { return cache_.empty() ? 0 : 1; }

private:
    std::unique_ptr<RealDataSource> real_;
    std::string cache_;
};
```

### 6. Client Usage

```cpp
int main() {
    std::cout << "=== Lazy Proxy Demo ===\n";
    {
        LazyProxy proxy("data.txt");
        std::cout << "Is loaded: " << proxy.isLoaded() << "\n";
        std::cout << "Data: " << proxy.getData() << "\n";
        std::cout << "Is loaded: " << proxy.isLoaded() << "\n";
    }

    std::cout << "\n=== Access Proxy Demo ===\n";
    {
        auto real = std::make_unique<RealDataSource>("data.txt");
        AccessProxy adminProxy(std::move(real), "admin");
        AccessProxy readerProxy(std::make_unique<RealDataSource>("data.txt"), "reader");

        try {
            std::cout << "Admin data: " << adminProxy.getData() << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }

        try {
            std::cout << "Reader data: " << readerProxy.getData() << "\n";
        } catch (const std::exception& e) {
            std::cout << "Error: " << e.what() << "\n";
        }
    }

    std::cout << "\n=== Cache Proxy Demo ===\n";
    {
        auto real = std::make_unique<RealDataSource>("data.txt");
        CacheProxy proxy(std::move(real));

        std::cout << "First call: " << proxy.getData() << "\n";
        std::cout << "Second call: " << proxy.getData() << "\n";
        proxy.setData("new data");
        std::cout << "After update: " << proxy.getData() << "\n";
    }

    return 0;
}
```

## Modern C++ Implementations

### With a Template Proxy

```cpp
template<typename T>
class Proxy : public T {
public:
    using T::T;  // inherit constructors

    void logAccess(const std::string& method) const {
        std::cout << "[Proxy] Calling " << method << "\n";
    }
};
```

### With a Decorator-Style Proxy

```cpp
template<typename T>
class LoggingProxy : public T {
public:
    using T::T;

    template<typename... Args>
    auto invoke(const std::string& method, Args&&... args)
        -> decltype(std::declval<T&>().someMethod(std::forward<Args>(args)...)) {

        std::cout << "[Logging] Calling " << method << "\n";
        auto result = std::invoke(&T::someMethod, *this, std::forward<Args>(args)...);
        std::cout << "[Logging] " << method << " returned\n";
        return result;
    }
};
```

### Managing with Smart Pointers

```cpp
class SmartProxy {
public:
    template<typename T, typename... Args>
    static std::shared_ptr<T> create(Args&&... args) {
        auto real = std::make_shared<T>(std::forward<Args>(args)...);
        return std::make_shared<LoggingProxy<T>>(real);
    }
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Decorator** | A decorator adds behavior; a proxy controls access |
| **Adapter** | An adapter converts an interface; a proxy controls access |
| **Facade** | A facade simplifies an interface; a proxy controls access |

## Best Practices

### 1. Prefer Composition over Inheritance

```cpp
// Recommended: use composition
class Proxy {
public:
    explicit Proxy(std::shared_ptr<RealObject> real)
        : real_(std::move(real)) {}

private:
    std::shared_ptr<RealObject> real_;
};

// Not recommended: use inheritance (it pins down the class hierarchy)
class Proxy : public RealObject { ... };
```

### 2. Use Weak References to Avoid Cycles

```cpp
class ObserverProxy {
public:
    void observe(std::weak_ptr<Subject> subject) {
        subject_ = subject;
    }

    void update() {
        if (auto subject = subject_.lock()) {
            subject->notify();
        }
    }

private:
    std::weak_ptr<Subject> subject_;
};
```

### 3. Support Multi-Layer Proxies

```cpp
class MultiProxy {
public:
    void addProxy(std::shared_ptr<Proxy> proxy) {
        proxies_.push_back(std::move(proxy));
    }

    std::string process() {
        std::string result = "original";
        for (const auto& proxy : proxies_) {
            result = proxy->intercept(result);
        }
        return result;
    }

private:
    std::vector<std::shared_ptr<Proxy>> proxies_;
};
```

## Common Traps

### Trap 1: Proxy Leaks

```cpp
// Not recommended: forgetting to release the proxy
void process() {
    Proxy* proxy = new Proxy(real);
    proxy->process();
    // forgot delete proxy;
}

// Recommended: use a smart pointer
void process() {
    auto proxy = std::make_unique<Proxy>(real);
    proxy->process();
    // released automatically
}
```

### Trap 2: Proxy Chains Getting Too Long

```cpp
// Not recommended: nesting proxies too deeply
auto proxy1 = std::make_shared<Proxy1>(
    std::make_shared<Proxy2>(
        std::make_shared<Proxy3>(
            std::make_shared<RealObject>()
        )
    )
);

// Recommended: use a proxy factory to keep them under control
class ProxyFactory {
public:
    static std::shared_ptr<RealObject> create() {
        auto real = std::make_shared<RealObject>();
        return std::make_shared<LoggingProxy>(
            std::make_shared<CacheProxy>(
                std::make_shared<AccessProxy>(real, "admin")
            )
        );
    }
};
```

### Trap 3: Thread Safety

```cpp
// Not recommended: not thread safe
class ThreadUnsafeProxy {
public:
    void process() {
        if (!real_) {
            real_ = createReal();  // possible race between threads
        }
        real_->process();
    }

private:
    RealObject* real_ = nullptr;
};

// Recommended: thread-safe lazy initialization
class ThreadSafeProxy {
public:
    void process() {
        std::call_once(flag_, [this]() {
            real_ = createReal();
        });
        real_->process();
    }

private:
    std::once_flag flag_;
    std::unique_ptr<RealObject> real_;
};
```

## Summary

The key points of the proxy pattern:

- **When to use it**: lazy loading, access control, caching, logging
- **Main benefits**: controlled access, extra logic, transparent to users
- **Implementation advice**: smart pointers, multi-layer proxies, thread safety
- **Common traps**: proxy leaks, over-long chains, thread safety

> **Remember**: the key of the proxy pattern is *controlling access* — a proxy
> guards access to the real object and adds extra logic without modifying the
> original class.
