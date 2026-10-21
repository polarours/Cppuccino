# Static Initialization Order Fiasco

## 1. Introduction

In C++, objects with namespace scope (including global objects and `static` objects) are initialized during program startup. This mechanism makes it convenient to build infrastructure such as logging systems, configuration objects, and registries.

However, when those objects live in different translation units (Translation Unit, TU) and depend on each other during initialization, the behavior of the program is no longer deterministic. This phenomenon is commonly known as the **Static Initialization Order Fiasco (SIOF)**.

What this chapter is about is not “how to define a static object”, but rather:

> **Under what conditions does the initialization order of static objects stop being inferable, and what does that mean for engineering code?**

---

## 2. The Language Rules of Static Initialization Order

### 2.1 The Initialization Order the Standard Guarantees

The C++ standard’s guarantees about the initialization order of static objects are limited and precise:

- **Within a single translation unit**
  - Static objects are initialized in the order of their definitions
- **Across translation units**
  - The initialization order of static objects is **unspecified**

“Unspecified” here means:

- The compiler and the linker may choose any order
- The result may differ between build configurations
- The program must not make assumptions about that order

### 2.2 When Does the Problem Occur?

The problem does not lie in “using static objects” as such, but in this:

> **A static object accesses another static object from a different translation unit while it is being constructed.**

Once such a dependency exists, and the initialization order is unconstrained by the standard, the program enters an unpredictable state.

---

## 3. A Typical Example

### 3.1 Example Code

#### logger.h
```cpp
#pragma once
#include <string>

struct Logger {
    void log(const std::string& msg);
};

extern Logger global_logger;
```

#### logger.cpp

```cpp
#include "logger.h"
#include <iostream>

Logger global_logger;

void Logger::log(const std::string& msg) {
    std::cout << msg << std::endl;
}
```

#### config.h

```cpp
#pragma once
#include <string>

struct Config {
    std::string app_name;
};

extern Config global_config;
```

#### config.cpp

```cpp
#include "config.h"
#include "logger.h"

Config global_config{
    [] {
        global_logger.log("Initializing config");
        return "Cppuccino";
    }()
};
```

### 3.2 How the Problem Manifests

In the code above, the initialization of `global_config` depends on `global_logger`. Because they live in different translation units (`config.cpp` and `logger.cpp`), the C++ standard does **not** guarantee their initialization order.

- **Case A**: the linker initializes `logger.cpp` first, then `config.cpp`. The program runs normally — `global_logger` has already been constructed, so calling `log` on it is safe.
- **Case B**: the linker initializes `config.cpp` first. At that point `global_logger` has not been constructed yet, and its memory may hold an uninitialized region (or zero-initialized bytes). Calling `global_logger.log()` is **undefined behavior (Undefined Behavior, UB)**, which typically shows up as a segmentation fault (Segmentation Fault) or as subtle memory corruption.

These non-deterministic crashes caused by cross-translation-unit dependencies between static objects are the classic Static Initialization Order Fiasco (SIOF).

---

## 4. Solutions and Best Practices

To eliminate the uncertainty that static initialization order brings, the C++ community has developed several standard patterns.

### 4.1 Construct On First Use (Meyers' Singleton)

This is the most widely used and most strongly recommended solution, commonly known as **Meyers' Singleton** (proposed by Scott Meyers). Its core idea is to replace the namespace-scope static object with a function-local static object.

#### Refactored logger

```cpp
// logger.h
#pragma once
#include <string>

struct Logger {
    void log(const std::string& msg);
};

Logger& get_logger(); // replaces extern Logger global_logger;

// logger.cpp
#include "logger.h"
#include <iostream>

Logger& get_logger() {
    static Logger instance; // function-local static object
    return instance;
}

void Logger::log(const std::string& msg) {
    std::cout << msg << std::endl;
}
```

#### Refactored config

```cpp
// config.h
#pragma once
#include <string>

struct Config {
    std::string app_name;
};

Config& get_config(); // replaces extern Config global_config;

// config.cpp
#include "config.h"
#include "logger.h"

Config& get_config() {
    static Config instance{
        [] {
            get_logger().log("Initializing config");
            return "Cppuccino";
        }()
    };
    return instance;
}
```

**Why does it work?**

The C++ standard specifies that a function-local static object is initialized **when the program’s control flow first passes through its declaration**. So when `get_config()` calls `get_logger()`, a `Logger` that has not been initialized yet is initialized right there. This establishes a strict, call-graph-based initialization order that completely sidesteps the nondeterminism at the translation-unit level.

> **Note**: since C++11, the standard guarantees that the initialization of a function-local static object is **thread-safe** (usually called Magic Statics). So this pattern stays safe and reliable in a multithreaded environment.

### 4.2 Constant Initialization and `constexpr`

If a static object can be initialized at compile time, it undergoes **constant initialization**, which happens before any **dynamic initialization** (Dynamic Initialization, i.e. run-time initialization).

If we declare the object as `constexpr`, the compiler forces it to be initialized at compile time, which makes it completely immune to SIOF.

```cpp
struct Config {
    const char* app_name;
};

// Initialized at compile time, before all dynamic initialization
constexpr Config global_config{"Cppuccino"};
```

### 4.3 The `constinit` Keyword in C++20

`constexpr` requires the object to be initialized at compile time *and* to be a constant itself (not modifiable). In some scenarios we need a global object that is **initialized at compile time but modifiable at run time**.

C++20 introduced the `constinit` keyword, which forces a variable to be constant-initialized or zero-initialized without requiring the variable to have `const` qualification.

```cpp
// Force compile-time initialization to avoid SIOF
constinit int global_counter = 0;

void increment() {
    global_counter++; // can be modified at run time
}
```

If you try to give a `constinit` variable a dynamic initializer, the compiler reports an error, killing SIOF in the cradle at compile time.

### 4.4 The Nifty Counter (Schwarz Counter) Idiom

The stream objects in the standard library, such as `std::cout` and `std::cin`, face the same SIOF problem (a user may use `std::cout` inside the constructor of their own global object). The standard library solves this with the Nifty Counter idiom: it defines a static counter object in the header and uses that object’s constructor to control the initialization and destruction of the real global object.

Because the approach is complex and easy to get wrong, modern C++ development usually recommends **Meyers' Singleton** or `constinit` instead, and hand-writing a Nifty Counter in business code is rare.

---

## 5. Summary

The Static Initialization Order Fiasco (SIOF) is one of the most insidious traps in C++; at its core it is the **unspecified order of dynamic initialization across translation units**.

**Engineering guidelines:**

1. **Avoid global state as far as possible**: reduce the use of global objects and singletons at the architectural level.
2. **Use function-local statics**: if a global object is unavoidable, prefer the “construct on first use (Meyers' Singleton)” pattern.
3. **Use compile-time facilities**: use `constexpr` or C++20’s `constinit` wherever possible to turn dynamic initialization into constant initialization.
4. **Watch out for destruction order**: function-local statics are destroyed in the reverse order of their construction. If the objects also depend on each other during destruction, that can cause crashes as well (Static Deinitialization Order Fiasco).
