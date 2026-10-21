# Template Method Pattern

## What Is the Template Method Pattern?

The Template Method Pattern is a behavioral design pattern that defines the
skeleton of an algorithm and defers some of its steps to subclasses. A template
method lets subclasses redefine particular steps of an algorithm without
changing the algorithm's structure.

> **Core idea**: define the algorithm's framework in the base class and let
> subclasses override individual steps without changing the algorithm's
> structure.

## Why Do We Need It?

### Problem Scenario

Suppose we have a data-processing system that has to support several formats
(CSV, JSON, XML):

```cpp
// Problem: every processing function repeats the same steps, only the details differ
void processCSV(const std::string& file) {
    openFile(file);
    validateData();  // CSV-specific validation
    transformData(); // CSV-specific transformation
    saveOutput();
}

void processJSON(const std::string& file) {
    openFile(file);
    validateData();  // JSON-specific validation
    transformData(); // JSON-specific transformation
    saveOutput();
}

void processXML(const std::string& file) {
    openFile(file);
    validateData();  // XML-specific validation
    transformData(); // XML-specific transformation
    saveOutput();
}
```

**Problems**:

1. Duplicated code: every function repeats the same steps
2. Hard to maintain: changing the algorithm's structure means changing every
   function
3. Error-prone: it is easy to forget one of the steps

### Solution

Use the template method pattern:

```cpp
class DataProcessor {
public:
    void process(const std::string& file) {
        openFile(file);           // concrete method
        if (validateData()) {     // hook method
            transformData();      // abstract method (implemented by subclasses)
            saveOutput();
        }
    }
    
protected:
    virtual void transformData() = 0;  // subclasses must implement it
    
private:
    void openFile(const std::string& file);
    bool validateData();
    void saveOutput();
};

class CSVProcessor : public DataProcessor {
protected:
    void transformData() override { /* CSV-specific transformation */ }
};

class JSONProcessor : public DataProcessor {
protected:
    void transformData() override { /* JSON-specific transformation */ }
};
```

**Benefits**:

1. Code reuse: the common logic is written once
2. Easy to maintain: changing the algorithm's structure only touches the base
   class
3. Enforced structure: subclasses must implement the specific steps

## Pattern Structure

```
                    ┌──────────────────┐
                    │  AbstractClass   │
                    │ (template method)│
                    └──────┬───────────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │ Primitive   │ │  Abstract   │ │   Hook     │ ← method types
    │ Operation   │ │ Operation   │ │  Method    │
    └─────────────┘ └────────────┘ └────────────┘
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ ConcreteA   │ │ ConcreteB   │ │  Optional   │ ← concrete implementations
    └─────────────┘ └────────────┘ └────────────┘
```

## Implementation

### 1. Define the Abstract Class

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>

class Game {
public:
    virtual ~Game() = default;
    
    // template method: defines the flow of a game
    void playOneGame() {
        initialize();
        startPlay();
        endPlay();
    }
    
protected:
    // hook methods: subclasses may customize them
    virtual void initialize() = 0;
    virtual void startPlay() = 0;
    virtual void endPlay() = 0;
    
    // concrete method: common logic
    void showScore() const {
        std::cout << "Final score: " << score_ << "\n";
    }
    
    void generateRandomNumber() {
        score_ = rand() % 100;
    }
    
private:
    int score_ = 0;
};
```

### 2. Implement the Concrete Games

```cpp
class Football : public Game {
protected:
    void initialize() override {
        std::cout << "Football Game Initialized!\n";
        std::cout << "Standards: Goal posts, 11 players per team\n";
    }
    
    void startPlay() override {
        std::cout << "Football Game Started!\n";
        std::cout << "Coin toss decided by captain. First team starts.\n";
    }
    
    void endPlay() override {
        std::cout << "Football Game Ended!\n";
        showScore();
        std::cout << "The Game was tied\n";
    }
};

class Cricket : public Game {
protected:
    void initialize() override {
        std::cout << "Cricket Game Initialized!\n";
        std::cout << "Standards: 2 innings, 4 overs per innings, 10 players per team\n";
    }
    
    void startPlay() override {
        std::cout << "Cricket Game Started!\n";
        std::cout << "Play starts with coin toss, batting team decided.\n";
    }
    
    void endPlay() override {
        std::cout << "Cricket Game Ended!\n";
        showScore();
        std::cout << "The Game was tied\n";
    }
};
```

### 3. A Template with Hook Methods

```cpp
class OrderProcessor {
public:
    void processOrder(const std::string& order) {
        validateOrder(order);
        if (shouldApplyDiscount(order)) {  // hook method
            applyDiscount(order);
        }
        calculateTotal(order);
        generateInvoice(order);
        sendConfirmation(order);
    }
    
protected:
    virtual bool shouldApplyDiscount(const std::string& order) {
        return false;  // no discount by default
    }
    
    virtual void applyDiscount(const std::string& order) {
        std::cout << "Applying discount to: " << order << "\n";
    }
    
private:
    void validateOrder(const std::string& order);
    void calculateTotal(const std::string& order);
    void generateInvoice(const std::string& order);
    void sendConfirmation(const std::string& order);
};

class VIPOrderProcessor : public OrderProcessor {
protected:
    bool shouldApplyDiscount(const std::string& order) override {
        return order.find("VIP") != std::string::npos;
    }
    
    void applyDiscount(const std::string& order) override {
        std::cout << "Applying VIP 20% discount to: " << order << "\n";
    }
};
```

## Modern C++ Implementations

### Simplifying with Lambdas

```cpp
class LambdaProcessor {
public:
    void process() {
        if (init_) init_();
        if (execute_) execute_();
        if (cleanup_) cleanup_();
    }
    
    void setInit(std::function<void()> init) { init_ = std::move(init); }
    void setExecute(std::function<void()> execute) { execute_ = std::move(execute); }
    void setCleanup(std::function<void()> cleanup) { cleanup_ = std::move(cleanup); }

private:
    std::function<void()> init_;
    std::function<void()> execute_;
    std::function<void()> cleanup_;
};

// Usage
LambdaProcessor processor;
processor.setInit([]() { std::cout << "Initializing...\n"; });
processor.setExecute([]() { std::cout << "Executing...\n"; });
processor.setCleanup([]() { std::cout << "Cleaning up...\n"; });
processor.process();
```

### Using CRTP

```cpp
template<typename Derived>
class GameTemplate {
public:
    void playOneGame() {
        static_cast<Derived*>(this)->initialize();
        static_cast<Derived*>(this)->startPlay();
        static_cast<Derived*>(this)->endPlay();
    }
};

class FootballGame : public GameTemplate<FootballGame> {
public:
    void initialize() { /* ... */ }
    void startPlay() { /* ... */ }
    void endPlay() { /* ... */ }
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Strategy** | Strategy replaces the whole algorithm; template method extends part of the steps |
| **Factory Method** | Factory method creates objects; template method defines the flow of the algorithm |
| **State** | State changes the behavior based on the state; template method fixes the structure of the algorithm |

## Best Practices

### 1. Use Access Control

```cpp
class Processor {
public:
    // template method: public interface
    void process() {
        step1();
        step2();
        step3();
    }
    
protected:
    // hook method: subclasses may override it
    virtual void step2() { /* default implementation */ }
    
private:
    // concrete method: not visible to subclasses
    void step1() { /* ... */ }
    void step3() { /* ... */ }
};
```

### 2. Avoid Virtual Calls in the Template Method

```cpp
// Not recommended: calling a virtual function in the template method can cause problems
class BadProcessor {
public:
    void process() {
        init();
        execute();  // virtual call
        cleanup();
    }
    
protected:
    virtual void execute() = 0;
};

// Recommended: use the final keyword to prevent overriding
class GoodProcessor {
public:
    void process() final {  // subclasses cannot override it
        init();
        execute();
        cleanup();
    }
    
protected:
    virtual void execute() = 0;
};
```

### 3. Provide Defaults with Hook Methods

```cpp
class ReportGenerator {
public:
    void generate() {
        prepareData();
        if (shouldIncludeCharts()) {  // hook
            generateCharts();
        }
        if (shouldIncludeSummary()) {  // hook
            generateSummary();
        }
        exportReport();
    }
    
protected:
    virtual bool shouldIncludeCharts() { return true; }
    virtual bool shouldIncludeSummary() { return true; }
    
private:
    void prepareData();
    void generateCharts();
    void generateSummary();
    void exportReport();
};
```

## Common Traps

### Trap 1: Too Many Virtual Functions

```cpp
// Not recommended: too many virtual functions, hard to maintain
class OverEngineered {
public:
    virtual void step1() = 0;
    virtual void step2() = 0;
    virtual void step3() = 0;
    virtual void step4() = 0;
    virtual void step5() = 0;
    // ... 20+ virtual functions
};

// Recommended: make only the necessary steps virtual
class Minimal {
public:
    void process() {
        prepare();
        if (needCustomStep()) customStep();  // only this step needs to be virtual
        finalize();
    }
    
protected:
    virtual void customStep() { /* default implementation */ }
};
```

### Trap 2: Breaking Encapsulation

```cpp
// Not recommended: subclasses can reach the private methods
class LeakyBase {
public:
    void process() {
        step1();
        step2();
    }
    
protected:
    // should not be exposed to subclasses
    void step1() { /* ... */ }
};

// Recommended: use protected methods to expose only the necessary interface
class EncapsulatedBase {
public:
    void process() final {
        doStep1();
        doStep2();
    }
    
protected:
    virtual void doStep2() { /* subclasses can override it */ }
    
private:
    void doStep1() { /* subclasses cannot access it */ }
};
```

### Trap 3: Virtual Call Overhead

```cpp
// Not recommended: lots of virtual functions in performance-critical code
class SlowProcessor {
public:
    void process() {
        for (int i = 0; i < 1000000; ++i) {
            tinyStep();  // virtual call overhead
        }
    }
    
protected:
    virtual void tinyStep() { /* ... */ }
};

// Recommended: use CRTP to avoid virtual calls
template<typename Derived>
class FastProcessor {
public:
    void process() {
        for (int i = 0; i < 1000000; ++i) {
            static_cast<Derived*>(this)->tinyStep();  // static binding
        }
    }
};
```

## Summary

The key points of the template method pattern:

- **When to use it**: a fixed algorithm structure, some steps vary, code reuse
  is needed
- **Main benefits**: code reuse, easy to maintain, enforced structure
- **Implementation advice**: use access control, avoid too many virtual
  functions, use hook methods
- **Common traps**: too many virtual functions, broken encapsulation, virtual
  call overhead

> **Remember**: the key of the template method pattern is *defining the
> skeleton* — put the algorithm's framework in the base class and let the
> subclasses decide how the individual steps are implemented.
