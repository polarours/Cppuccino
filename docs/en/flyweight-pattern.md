# Flyweight Pattern

## What Is the Flyweight Pattern?

The Flyweight Pattern is a structural design pattern that supports a large
number of fine-grained objects efficiently by sharing them, so that shared
state is not duplicated in memory over and over.

> **Core idea**: split an object's state into an intrinsic part and an
> extrinsic part — the intrinsic state is shared, the extrinsic state is managed
> by the client.

## Why Do We Need It?

### Problem Scenario

Suppose we have a text editor that has to display a large number of characters:

```cpp
// Problem: every character is a separate object, so the memory overhead is large
class Character {
public:
    Character(char glyph, Font font) 
        : glyph_(glyph), font_(font) {}
    
    void draw(int x, int y) {
        // draw the character with glyph_ and font_
        // the position (x, y) is needed as well
    }
    
private:
    char glyph_;      // intrinsic state: shareable
    Font font_;       // intrinsic state: shareable
    int x_;           // extrinsic state: not shareable
    int y_;           // extrinsic state: not shareable
};

// create 1000 'A' characters
std::vector<Character> characters;
for (int i = 0; i < 1000; ++i) {
    characters.push_back(Character('A', Font("Arial", 12)));
}
// memory overhead: 1000 separate objects
```

**Problems**:

1. Many identical characters waste memory
2. The intrinsic state is stored again and again
3. Hard to maintain

### Solution

Use the flyweight pattern:

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
        // use the shared glyph_ and font_
        // the position (x, y) is passed in by the client
    }
    
private:
    char glyph_;  // intrinsic state: shared
    Font font_;   // intrinsic state: shared
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

**Benefits**:

1. Saves memory: identical characters share one object
2. Improves efficiency: fewer objects have to be created
3. Easy to maintain: the shared objects are managed in one place

## Pattern Structure

```
                    ┌─────────────┐
                    │  Flyweight  │
                    │  (shared)   │
                    └──────┬──────┘
                           │
           ┌───────────────┼───────────────┐
           │               │               │
    ┌──────▼──────┐ ┌──────▼──────┐ ┌──────▼──────┐
    │  Char 'A'   │ │  Char 'B'   │ │  Char 'C'   │ ← concrete flyweights
    └──────┬──────┘ └──────┬──────┘ └──────┬──────┘
           │               │               │
           └───────────────┼───────────────┘
                           │
              ┌────────────▼────────────┐
              │    FlyweightFactory     │ ← flyweight factory
              └────────────┬────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← client
                    └─────────────────┘
```

## Implementation

### 1. Define the Flyweight Interface

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <map>
#include <vector>

class Character {
public:
    virtual ~Character() = default;
    virtual void draw(int x, int y, const std::string& text) = 0;
    
    char getGlyph() const { return glyph_; }
    const std::string& getFont() const { return font_; }

protected:
    explicit Character(char glyph, const std::string& font)
        : glyph_(glyph), font_(font) {}
    
    char glyph_;      // intrinsic state
    std::string font_; // intrinsic state
};
```

### 2. Implement the Concrete Flyweights

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

### 3. Implement the Flyweight Factory

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
            // create a new flyweight
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
    
    std::map<std::pair<char, std::string>, std::unique_ptr<Character>> characters_;
};
```

### 4. Client Usage

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
    
    // insert a large number of characters
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

## Intrinsic State and Extrinsic State

### Intrinsic State

- Shared inside the object, does not change with the environment
- Can be stored in the flyweight object
- Examples: a character's glyph, font, color

### Extrinsic State

- Changes when the environment changes
- Cannot be shared, the client manages it
- Examples: a character's position, size, rotation angle

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

// the client manages the extrinsic state
std::vector<std::pair<Character*, ExtrinsicState>> renderList;
```

## Modern C++ Implementations

### Using Smart Pointers

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

### Optimizing with an Object Pool

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

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Singleton** | A singleton has one instance; a flyweight has many shared instances |
| **Object pool** | An object pool reuses objects; a flyweight shares state |
| **Cache** | A cache stores results; a flyweight shares objects |

## Best Practices

### 1. Choose the Right Sharing Granularity

```cpp
// Too fine-grained: one flyweight per character
class CharFlyweight { ... };  // 1000 characters -> 1000 flyweights

// The right granularity: one flyweight per font + size
class FontFlyweight { ... };  // 10 fonts -> 10 flyweights
```

### 2. Consider Thread Safety

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

### 3. Support State Changes

```cpp
class UpdatableFlyweight {
public:
    void updateState(const State& newState) {
        // notify every client that uses this flyweight
        for (auto* observer : observers_) {
            observer->onStateChange(newState);
        }
    }

private:
    std::vector<Observer*> observers_;
};
```

## Common Traps

### Trap 1: Inconsistent Shared State

```cpp
// Not recommended: the shared state gets modified
class BadFlyweight {
public:
    void modifyState() {
        state_ = newState;  // affects every object that shares this flyweight
    }
    
private:
    State state_;
};

// Recommended: the intrinsic state is immutable
class GoodFlyweight {
public:
    // no method for modifying the intrinsic state
};
```

### Trap 2: Complex Extrinsic State Management

```cpp
// Not recommended: the extrinsic state is scattered around
std::map<int, int> positions;  // positions
std::map<int, int> sizes;      // sizes
std::map<int, std::string> fonts;  // fonts

// Recommended: encapsulate the extrinsic state
struct CharacterState {
    int x, y;
    int size;
    std::string font;
};
std::map<int, CharacterState> states;
```

### Trap 3: Overusing the Flyweight

```cpp
// Not recommended: even simple objects become flyweights
class SimplePoint : public Flyweight {
    // too simple to be worth sharing
};

// Recommended: use a flyweight only when there are many repeated objects
class FontCharacter : public Flyweight {
    // many characters share a font, so a flyweight pays off
};
```

## Example

A runnable version of this topic:

- [flyweight-pattern-demo.cpp](../../examples/flyweight-pattern-demo.cpp)

## Summary

The key points of the flyweight pattern:

- **When to use it**: many fine-grained objects, shareable intrinsic state,
  memory sensitivity
- **Main benefits**: saves memory, improves efficiency, easy to maintain
- **Implementation advice**: separate intrinsic and extrinsic state, thread
  safety, the right sharing granularity
- **Common traps**: inconsistent state, complex extrinsic state, overuse

> **Remember**: the key of the flyweight pattern is *sharing state* — pull the
> shareable state out into shared objects, so that many objects use one and the
> same copy of it.
