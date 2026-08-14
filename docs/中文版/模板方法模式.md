# 模板方法模式 (Template Method Pattern)

## 什么是模板方法模式？

模板方法模式（Template Method Pattern）是一种行为型设计模式，它定义了一个算法的骨架，而将一些步骤延迟到子类中。模板方法使得子类可以不改变一个算法的结构即可重定义该算法的某些特定步骤。

> **核心思想**：在父类中定义算法框架，允许子类重写某些步骤而不改变算法结构。

## 为什么需要模板方法模式？

### 问题场景

假设我们有一个数据处理系统，需要支持多种格式（CSV、JSON、XML）：

```cpp
// 问题：每个处理函数都有相同的步骤，但细节不同
void processCSV(const std::string& file) {
    openFile(file);
    validateData();  // CSV 特定的验证
    transformData(); // CSV 特定的转换
    saveOutput();
}

void processJSON(const std::string& file) {
    openFile(file);
    validateData();  // JSON 特定的验证
    transformData(); // JSON 特定的转换
    saveOutput();
}

void processXML(const std::string& file) {
    openFile(file);
    validateData();  // XML 特定的验证
    transformData(); // XML 特定的转换
    saveOutput();
}
```

**问题**：
1. 重复代码：每个函数都有相同的步骤
2. 难以维护：修改算法结构需要修改所有函数
3. 容易出错：可能忘记某个步骤

### 解决方案

使用模板方法模式：

```cpp
class DataProcessor {
public:
    void process(const std::string& file) {
        openFile(file);           // 具体方法
        if (validateData()) {     // 钩子方法
            transformData();      // 抽象方法（子类实现）
            saveOutput();
        }
    }
    
protected:
    virtual void transformData() = 0;  // 子类必须实现
    
private:
    void openFile(const std::string& file);
    bool validateData();
    void saveOutput();
};

class CSVProcessor : public DataProcessor {
protected:
    void transformData() override { /* CSV 特定转换 */ }
};

class JSONProcessor : public DataProcessor {
protected:
    void transformData() override { /* JSON 特定转换 */ }
};
```

**优点**：
1. 代码复用：公共逻辑只写一次
2. 易于维护：修改算法结构只改父类
3. 强制结构：子类必须实现特定步骤

## 模式结构

```
                    ┌─────────────┐
                    │  AbstractClass │
                    │ (模板方法)    │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │ Primitive   │ │  Abstract   │ │   Hook     │ ← 方法类型
    │ Operation   │ │ Operation   │ │  Method    │
    └─────────────┘ └─────────────┘ └────────────┘
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │ ConcreteA   │ │ ConcreteB   │ │  Optional   │ ← 具体实现
    └─────────────┘ └─────────────┘ └─────────────┘
```

## 代码实现

### 1. 定义抽象类

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>

class Game {
public:
    virtual ~Game() = default;
    
    // 模板方法：定义游戏流程
    void playOneGame() {
        initialize();
        startPlay();
        endPlay();
    }
    
protected:
    // 钩子方法：允许子类自定义
    virtual void initialize() = 0;
    virtual void startPlay() = 0;
    virtual void endPlay() = 0;
    
    // 具体方法：公共逻辑
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

### 2. 实现具体游戏

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

### 3. 带钩子方法的模板

```cpp
class OrderProcessor {
public:
    void processOrder(const std::string& order) {
        validateOrder(order);
        if (shouldApplyDiscount(order)) {  // 钩子方法
            applyDiscount(order);
        }
        calculateTotal(order);
        generateInvoice(order);
        sendConfirmation(order);
    }
    
protected:
    virtual bool shouldApplyDiscount(const std::string& order) {
        return false;  // 默认不应用折扣
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

## 现代 C++ 实现

### 使用 lambda 简化

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

// 使用
LambdaProcessor processor;
processor.setInit([]() { std::cout << "Initializing...\n"; });
processor.setExecute([]() { std::cout << "Executing...\n"; });
processor.setCleanup([]() { std::cout << "Cleaning up...\n"; });
processor.process();
```

### 使用 CRTP

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

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **策略模式** | 策略模式替换整个算法；模板方法扩展部分步骤 |
| **工厂方法** | 工厂方法创建对象；模板方法定义算法流程 |
| **状态模式** | 状态模式根据状态改变行为；模板方法固定算法结构 |

## 最佳实践

### 1. 使用访问控制

```cpp
class Processor {
public:
    // 模板方法：公共接口
    void process() {
        step1();
        step2();
        step3();
    }
    
protected:
    // 钩子方法：子类可重写
    virtual void step2() { /* 默认实现 */ }
    
private:
    // 具体方法：子类不能访问
    void step1() { /* ... */ }
    void step3() { /* ... */ }
};
```

### 2. 避免在模板方法中使用虚函数

```cpp
// 不推荐：在模板方法中调用虚函数可能导致问题
class BadProcessor {
public:
    void process() {
        init();
        execute();  // 虚函数调用
        cleanup();
    }
    
protected:
    virtual void execute() = 0;
};

// 推荐：使用 final 关键字防止重写
class GoodProcessor {
public:
    void process() final {  // 禁止子类重写
        init();
        execute();
        cleanup();
    }
    
protected:
    virtual void execute() = 0;
};
```

### 3. 使用钩子方法提供默认行为

```cpp
class ReportGenerator {
public:
    void generate() {
        prepareData();
        if (shouldIncludeCharts()) {  // 钩子
            generateCharts();
        }
        if (shouldIncludeSummary()) {  // 钩子
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

## 常见陷阱

### 陷阱 1：过多的虚函数

```cpp
// 不推荐：太多虚函数，难以维护
class OverEngineered {
public:
    virtual void step1() = 0;
    virtual void step2() = 0;
    virtual void step3() = 0;
    virtual void step4() = 0;
    virtual void step5() = 0;
    // ... 20+ 个虚函数
};

// 推荐：只将必要的步骤设为虚函数
class Minimal {
public:
    void process() {
        prepare();
        if (needCustomStep()) customStep();  // 只有这一步需要虚函数
        finalize();
    }
    
protected:
    virtual void customStep() { /* 默认实现 */ }
};
```

### 陷阱 2：破坏封装性

```cpp
// 不推荐：子类可以访问私有方法
class LeakyBase {
public:
    void process() {
        step1();
        step2();
    }
    
protected:
    // 不应该暴露给子类
    void step1() { /* ... */ }
};

// 推荐：使用保护方法只暴露必要的接口
class EncapsulatedBase {
public:
    void process() final {
        doStep1();
        doStep2();
    }
    
protected:
    virtual void doStep2() { /* 子类可以重写 */ }
    
private:
    void doStep1() { /* 子类不能访问 */ }
};
```

### 陷阱 3：虚函数调用开销

```cpp
// 不推荐：在性能敏感代码中使用大量虚函数
class SlowProcessor {
public:
    void process() {
        for (int i = 0; i < 1000000; ++i) {
            tinyStep();  // 虚函数调用开销
        }
    }
    
protected:
    virtual void tinyStep() { /* ... */ }
};

// 推荐：使用 CRTP 避免虚函数调用
template<typename Derived>
class FastProcessor {
public:
    void process() {
        for (int i = 0; i < 1000000; ++i) {
            static_cast<Derived*>(this)->tinyStep();  // 静态绑定
        }
    }
};
```

## 总结

模板方法模式的核心要点：

- **适用场景**：算法结构固定、部分步骤可变、需要代码复用
- **核心优点**：代码复用、易于维护、强制结构
- **实现要点**：使用访问控制、避免过度虚函数、使用钩子方法
- **常见陷阱**：过多虚函数、破坏封装、虚函数调用开销

> **记住**：模板方法模式的关键是"定义骨架"——在父类中定义算法框架，让子类决定具体步骤的实现。
