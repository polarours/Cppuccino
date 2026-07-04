# Static Initialization Order Fiasco

## 1. Introduction

The Static Initialization Order Fiasco is a subtle and dangerous problem in C++ where the initialization order of static objects across different translation units (source files) is undefined. This can lead to accessing uninitialized objects, causing crashes or subtle bugs.

## 2. The Problem

### 2.1 Basic Example

```cpp
// file1.cpp
#include <iostream>
extern int y;
int x = y + 1;  // BAD: y may not be initialized yet
void printX() { std::cout << "x = " << x << '\n'; }

// file2.cpp
int y = 42;
```

The C++ standard does not define the order in which `x` and `y` are initialized. If `x` is initialized before `y`, it will use an uninitialized value.

### 2.2 Real-World Example

```cpp
// logger.cpp
#include <fstream>
std::ofstream logFile("app.log");

// main.cpp
extern std::ofstream logFile;
void log(const std::string& msg) {
    logFile << msg << '\n';  // BAD: logFile may not be initialized
}
```

## 3. Why It Happens

- Static objects in different translation units have **static storage duration**
- The C++ standard allows implementations to initialize them in any order
- Even within a translation unit, the order is defined, but across units it is not
- This is especially problematic for global objects and namespace-scope objects

## 4. Solutions

### 4.1 Construct on First Use (Meyers' Singleton)

The most common and recommended solution:

```cpp
Logger& getLogger() {
    static Logger instance;  // Initialized on first call
    return instance;
}

void log(const std::string& msg) {
    getLogger() << msg << '\n';  // Safe: instance is initialized before use
}
```

**Why it works:**
- The static local variable is initialized when the function is first called
- Function-local statics are guaranteed to be initialized before their first use
- Thread-safe in C++11 and later

### 4.2 Avoid Static Objects

Use dynamic allocation or stack objects instead:

```cpp
// Instead of global static:
// std::ofstream globalLog("app.log");

// Use a pointer that can be initialized when needed:
std::ofstream* logFile = nullptr;

void initLog() {
    static std::ofstream file("app.log");
    logFile = &file;
}
```

### 4.3 Explicit Initialization Order

If you must have multiple static objects, document and enforce the initialization order:

```cpp
// config.h
extern int configValue;

// config.cpp
int configValue = 42;

// main.cpp
extern int configValue;  // Use after config.cpp is initialized
```

This is fragile and hard to maintain.

### 4.4 Use std::call_once

For lazy initialization with thread safety:

```cpp
#include <mutex>

std::once_flag initFlag;
std::unique_ptr<Logger> globalLogger;

void initLogger() {
    std::call_once(initFlag, []() {
        globalLogger = std::make_unique<Logger>("app.log");
    });
}

Logger& getLogger() {
    initLogger();
    return *globalLogger;
}
```

## 5. Thread Safety Considerations

### 5.1 C++11 Static Initialization

In C++11 and later, function-local statics are thread-safe:

```cpp
Logger& getLogger() {
    static Logger instance;  // Thread-safe initialization
    return instance;
}
```

The compiler generates code to ensure only one thread initializes the static.

### 5.2 Before C++11

Before C++11, static initialization was not thread-safe. You had to use:

```cpp
// Double-checked locking (broken in C++03)
Logger* instance = nullptr;
std::mutex mtx;

Logger& getLogger() {
    if (!instance) {  // First check (no lock)
        std::lock_guard<std::mutex> lock(mtx);
        if (!instance) {  // Second check (with lock)
            instance = new Logger("app.log");
        }
    }
    return *instance;
}
```

This pattern is broken in C++03 due to memory ordering issues. Use `std::call_once` instead.

## 6. Detection

### 6.1 Runtime Detection

If you suspect initialization order issues, add logging:

```cpp
int x = []() {
    std::cout << "x initialized\n";
    return y + 1;
}();
```

### 6.2 Static Analysis

Some static analysis tools can detect potential initialization order issues.

## 7. Best Practices

1. **Prefer function-local statics** — They are guaranteed to be initialized before first use
2. **Avoid global state** — Minimize the number of static objects
3. **Use dependency injection** — Pass dependencies explicitly instead of relying on globals
4. **Document initialization order** — If you must have static dependencies, document them
5. **Use `std::call_once`** — For lazy, thread-safe initialization

## 8. Summary

- Static initialization order across translation units is undefined
- This can lead to accessing uninitialized objects
- Use the "construct on first use" idiom to avoid the problem
- Function-local statics are thread-safe in C++11 and later
- Minimize global state and use dependency injection when possible
