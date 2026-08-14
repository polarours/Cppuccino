# 享元模式 (Flyweight Pattern)

## 什么是享元模式？

享元模式（Flyweight Pattern）是一种结构型设计模式，它通过共享技术来有效地支持大量细粒度的对象，避免对象间共享状态导致的内存开销。

> **核心思想**：将对象的状态分为内部状态和外部状态，内部状态共享，外部状态由客户端管理。

## 为什么需要享元模式？

### 问题场景

假设我们有一个文本编辑器，需要显示大量字符：

```cpp
// 问题：每个字符都是一个独立对象，内存开销大
class Character {
public:
    Character(char glyph, Font font) 
        : glyph_(glyph), font_(font) {}
    
    void draw(int x, int y) {
        // 使用 glyph_ 和 font_ 绘制字符
        // 同时需要位置 (x, y)
    }
    
private:
    char glyph_;      // 内部状态：可共享
    Font font_;       // 内部状态：可共享
    int x_;           // 外部状态：不可共享
    int y_;           // 外部状态：不可共享
};

// 创建 1000 个 'A' 字符
std::vector<Character> characters;
for (int i = 0; i < 1000; ++i) {
    characters.push_back(Character('A', Font("Arial", 12)));
}
// 内存开销：1000 个独立对象
```

**问题**：
1. 大量相同字符浪费内存
2. 内部状态重复存储
3. 难以维护

### 解决方案

使用享元模式：

```cpp
class Flyweight {
public:
    virtual ~Flyweight() = default;
    virtual void draw(int x, int y) = 0;
};

class CharacterFlyweight : public Flyweight {
public:
    CharacterFlyweight(char glyph, Font font) 
        : glyph_(glyph), font_(font) {}
    
    void draw(int x, int y) override {
        // 使用共享的 glyph_ 和 font_
        // 位置 (x, y) 由客户端传递
    }
    
private:
    char glyph_;  // 内部状态：共享
    Font font_;   // 内部状态：共享
};

class FlyweightFactory {
public:
    Flyweight* getCharacter(char glyph, Font font) {
        auto key = std::make_pair(glyph, font);
        if (flies_.find(key) == flies_.end()) {
            flies_[key] = new CharacterFlyweight(glyph, font);
        }
        return flies_[key];
    }

private:
    std::map<std::pair<char, Font>, Flyweight*> flies_;
};
```

**优点**：
1. 节省内存：相同字符共享对象
2. 提高效率：减少对象创建开销
3. 易于维护：集中管理共享对象

## 模式结构

```
                    ┌─────────────┐
                    │  Flyweight  │
                    │  (享元)      │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Char 'A'   │ │  Char 'B'   │ │  Char 'C'   │ ← 具体享元
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │    FlyweightFactory     │ ← 享元工厂
              └────────────┬────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← 客户端
                    └─────────────────┘
```

## 代码实现

### 1. 定义享元接口

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <map>
#include <unordered_map>

class Character {
public:
    virtual ~Character() = default;
    virtual void draw(int x, int y, const std::string& text) = 0;
    
    char getGlyph() const { return glyph_; }
    const std::string& getFont() const { return font_; }

protected:
    explicit Character(char glyph, const std::string& font)
        : glyph_(glyph), font_(font) {}
    
    char glyph_;      // 内部状态
    std::string font_; // 内部状态
};
```

### 2. 实现具体享元

```cpp
class LatinCharacter : public Character {
public:
    LatinCharacter(char glyph, const std::string& font)
        : Character(glyph, font) {}
    
    void draw(int x, int y, const std::string& text) override {
        std::cout << "[" << glyph_ << "] at (" << x << ", " << y 
                  << ") font=" << font_ << "\n";
    }
};

class DigitCharacter : public Character {
public:
    DigitCharacter(char glyph, const std::string& font)
        : Character(glyph, font) {}
    
    void draw(int x, int y, const std::string& text) override {
        std::cout << "[DIGIT:" << glyph_ << "] at (" << x << ", " << y 
                  << ") font=" << font_ << "\n";
    }
};
```

### 3. 实现享元工厂

```cpp
class CharacterFactory {
public:
    static CharacterFactory& getInstance() {
        static CharacterFactory factory;
        return factory;
    }
    
    Character* getCharacter(char glyph, const std::string& font = "Arial") {
        auto key = std::make_pair(glyph, font);
        auto it = characters_.find(key);
        if (it == characters_.end()) {
            // 创建新享元
            if (glyph >= '0' && glyph <= '9') {
                characters_[key] = std::make_unique<DigitCharacter>(glyph, font);
            } else {
                characters_[key] = std::make_unique<LatinCharacter>(glyph, font);
            }
        }
        return characters_[key].get();
    }
    
    size_t getFlyweightCount() const {
        return characters_.size();
    }

private:
    CharacterFactory() = default;
    
    std::unordered_map<std::pair<char, std::string>, std::unique_ptr<Character>> characters_;
};
```

### 4. 客户端使用

```cpp
class TextEditor {
public:
    void insertCharacter(char c, int x, int y) {
        Character* charFlyweight = CharacterFactory::getInstance().getCharacter(c);
        charFlyweight->draw(x, y, text_);
        text_.push_back(c);
        positions_.push_back({x, y});
    }
    
    void renderAll() const {
        for (size_t i = 0; i < text_.size(); ++i) {
            Character* charFlyweight = CharacterFactory::getInstance().getCharacter(text_[i]);
            charFlyweight->draw(positions_[i].first, positions_[i].second, text_);
        }
    }
    
    size_t getCharacterCount() const { return text_.size(); }
    size_t getFlyweightCount() const { 
        return CharacterFactory::getInstance().getFlyweightCount(); 
    }

private:
    std::string text_;
    std::vector<std::pair<int, int>> positions_;
};

int main() {
    TextEditor editor;
    
    // 插入大量字符
    std::string text = "Hello World! This is a flyweight example.";
    int x = 0, y = 0;
    
    for (char c : text) {
        editor.insertCharacter(c, x, y);
        x += 10;
        if (x > 500) {
            x = 0;
            y += 20;
        }
    }
    
    std::cout << "Text length: " << editor.getCharacterCount() << "\n";
    std::cout << "Flyweights: " << editor.getFlyweightCount() << "\n";
    std::cout << "Saved: " << editor.getCharacterCount() - editor.getFlyweightCount() 
              << " objects\n\n";
    
    std::cout << "=== Rendering ===\n";
    editor.renderAll();
    
    return 0;
}
```

## 内部状态与外部状态

### 内部状态（Intrinsic State）

- 在对象内共享，不随环境改变
- 可以存储在享元对象中
- 示例：字符的 glyph、字体、颜色

### 外部状态（Extrinsic State）

- 随环境改变而改变
- 不能共享，由客户端管理
- 示例：字符的位置、大小、旋转角度

```cpp
class ExtrinsicState {
public:
    ExtrinsicState(int x, int y, int size)
        : x_(x), y_(y), size_(size) {}
    
    int x() const { return x_; }
    int y() const { return y_; }
    int size() const { return size_; }

private:
    int x_, y_, size_;
};

// 客户端管理外部状态
std::vector<std::pair<Character*, ExtrinsicState>> renderList;
```

## 现代 C++ 实现

### 使用智能指针

```cpp
class ModernFlyweightFactory {
public:
    template<typename... Args>
    auto getFlyweight(Args&&... args) 
        -> std::shared_ptr<Flyweight> {
        
        auto key = std::make_tuple(std::forward<Args>(args)...);
        
        auto it = flyweights_.find(key);
        if (it == flyweights_.end()) {
            flyweights_[key] = std::make_shared<ConcreteFlyweight>(
                std::forward<Args>(args)...
            );
        }
        return flyweights_[key];
    }

private:
    std::unordered_map<std::tuple<>, std::shared_ptr<Flyweight>> flyweights_;
};
```

### 使用对象池优化

```cpp
template<typename T>
class ObjectPool {
public:
    T* acquire() {
        if (!pool_.empty()) {
            auto obj = std::move(pool_.back());
            pool_.pop_back();
            return obj.get();
        }
        return new T();
    }
    
    void release(T* obj) {
        pool_.emplace_back(std::unique_ptr<T>(obj));
    }
    
    size_t size() const { return pool_.size(); }

private:
    std::vector<std::unique_ptr<T>> pool_;
};
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **单例模式** | 单例只有一个实例；享元有多个共享实例 |
| **对象池** | 对象池复用对象；享元共享状态 |
| **缓存** | 缓存存储结果；享元共享对象 |

## 最佳实践

### 1. 选择合适的共享粒度

```cpp
// 过于细粒度：每个字符一个享元
class CharFlyweight { ... };  // 1000 个字符 -> 1000 个享元

// 合适粒度：每个字体+大小一个享元
class FontFlyweight { ... };  // 10 种字体 -> 10 个享元
```

### 2. 考虑线程安全

```cpp
class ThreadSafeFactory {
public:
    Character* getCharacter(char glyph, const std::string& font) {
        std::lock_guard<std::mutex> lock(mutex_);
        // ...
    }

private:
    std::mutex mutex_;
};
```

### 3. 支持状态变更

```cpp
class UpdatableFlyweight {
public:
    void updateState(const State& newState) {
        // 通知所有使用此享元的客户端
        for (auto* observer : observers_) {
            observer->onStateChange(newState);
        }
    }

private:
    std::vector<Observer*> observers_;
};
```

## 常见陷阱

### 陷阱 1：共享状态不一致

```cpp
// 不推荐：共享状态被修改
class BadFlyweight {
public:
    void modifyState() {
        state_ = newState;  // 影响所有共享此享元的对象
    }
    
private:
    State state_;
};

// 推荐：内部状态不可变
class GoodFlyweight {
public:
    // 不提供修改内部状态的方法
};
```

### 陷阱 2：外部状态管理复杂

```cpp
// 不推荐：外部状态分散管理
std::map<int, int> positions;  // 位置
std::map<int, int> sizes;      // 大小
std::map<int, std::string> fonts;  // 字体

// 推荐：封装外部状态
struct CharacterState {
    int x, y;
    int size;
    std::string font;
};
std::map<int, CharacterState> states;
```

### 陷阱 3：过度使用享元

```cpp
// 不推荐：简单对象也使用享元
class SimplePoint : public Flyweight {
    // 过于简单，不值得共享
};

// 推荐：只在有大量重复对象时使用
class FontCharacter : public Flyweight {
    // 大量字符共享字体，值得使用享元
};
```

## 总结

享元模式的核心要点：

- **适用场景**：大量细粒度对象、内部状态可共享、内存敏感
- **核心优点**：节省内存、提高效率、易于维护
- **实现要点**：区分内外状态、线程安全、合适共享粒度
- **常见陷阱**：状态不一致、外部状态复杂、过度使用

> **记住**：享元模式的关键是"共享状态"——将可共享的状态提取到共享对象中，让大量对象共用同一份状态。
