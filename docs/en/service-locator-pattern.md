# Service Locator Pattern

## What Is the Service Locator Pattern?

The service locator pattern is a registry pattern: a set of globally available
service instances is registered in one object, and business code looks them up
by name instead of explicitly injecting dependencies at every use site.

> **Core idea**: one centralized lookup table — `registerService(name,
> instance)` / `getService<T>(name)` — decouples "who needs" from "who
> provides".

## How It Relates to Dependency Injection (DI)

| | Service Locator | Dependency Injection |
|---|---|---|
| Service source | Caller actively calls `locator.get<X>()` | Passed in by a container/externally at construction |
| Dependency visibility | Hidden (inside the locator) | Explicit (constructor parameter list) |
| Testability | Weaker (must replace the locator globally) | Stronger (pass in a mock directly) |
| Fit | Frameworks/plugin systems needing runtime registration | Application-layer business objects |

The two are not mutually exclusive: applications often use both — DI injects
"business services" while the locator exposes "cross-cutting capabilities"
(logging, configuration, plugins).

## Core Concepts

- **Registration**: `registerService(name, shared_ptr<void>)`, keyed by a
  string name
- **Typed retrieval**: `getService<T>(name)` restores the type with
  `static_pointer_cast<T>`
- **Lifetime**: held by `shared_ptr`; the locator does not construct services
- **Optional presence**: `hasService(name)` allows runtime probing

## Problem Scenario

```cpp
// Problem: business code news up the concrete service — hard to swap implementations
class PdfExporter { /* ... */ };

class ReportGenerator {
    PdfExporter exporter;   // hardcoded concrete type
};
```

### Solution

```cpp
// Solution: depend on an interface; the concrete type is resolved by name
#include <memory>

struct IExporter { virtual ~IExporter() = default; };

class ReportGenerator {
public:
    explicit ReportGenerator(std::shared_ptr<IExporter> exporter)
        : exporter_(std::move(exporter)) {}

private:
    std::shared_ptr<IExporter> exporter_;  // swapped through the locator
};
```

## Implementation

```cpp
#include <map>
#include <memory>
#include <string>

class ServiceLocator {
public:
    void registerService(const std::string& name, std::shared_ptr<void> service) {
        services_[name] = std::move(service);
    }

    std::shared_ptr<void> getRaw(const std::string& name) const {
        auto it = services_.find(name);
        return (it != services_.end()) ? it->second : nullptr;
    }

    template <typename T>
    std::shared_ptr<T> getService(const std::string& name) const {
        return std::static_pointer_cast<T>(getRaw(name));
    }

    bool hasService(const std::string& name) const {
        return services_.find(name) != services_.end();
    }

    void removeService(const std::string& name) { services_.erase(name); }
    std::size_t serviceCount() const { return services_.size(); }

private:
    std::map<std::string, std::shared_ptr<void>> services_;
};
```

For the full runnable example, see the
[service-locator project](../../cpp-mastery-roadmap/30-stage30/service-locator).

## Usage Example

```cpp
struct ILogger { virtual ~ILogger() = default; virtual void log(const std::string&) = 0; };
struct ConsoleLogger : ILogger {
    void log(const std::string& m) override { /* ... */ }
};

ServiceLocator locator;
locator.registerService("logger", std::make_shared<ConsoleLogger>());

auto logger = locator.getService<ILogger>("logger");
logger->log("hello");
```

## Best Practices

1. **Avoid the global singleton**: pass the locator instance explicitly as a
   parameter where possible — tests will thank you
2. **Type mismatches fail silently**: `getService<T>`'s `static_pointer_cast`
   performs no runtime type check; callers must keep the name + type
   combination correct
3. **Look up capabilities, inject instances**: putting multi-instance objects
   (e.g. database connection-pool slots) into the locator invites races —
   singletons are its comfort zone
4. **Destruction order**: the locator holds `shared_ptr`s; services held by the
   locator must not hold the locator back, or you get a cycle

## Example

A runnable version of this topic:

- [service-locator-demo.cpp](../../examples/service-locator-demo.cpp)

## Summary

The service locator provides runtime lookup and is a natural home for plugin
systems and cross-cutting concerns (logging/config/feature flags); but keep its
surface small — core business dependencies should prefer explicit DI, or
testability degrades quickly.
