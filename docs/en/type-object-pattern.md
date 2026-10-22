# Type Object Pattern

## What Is the Type Object Pattern?

The type object pattern models "type" as a separate object and lets each
"instance" hold a reference to its type object — creating large numbers of
similar-but-parameterized kinds **without introducing new classes**.

> **Core idea**: move classification from classes (compile time) down to
> objects (runtime): replace N classes with 1 class + N type objects.

## When It Fits

- Many similar subclasses are needed (RPG monsters, chess piece kinds, UI
  themes…)
- The classification must be created at runtime or loaded from config/data
- Relationships between kinds (summed HP, inherited skills) belong in the data
  layer, not the inheritance layer

## Core Concepts

- **Type object**: describes the properties of a kind (HP/attack/defense/skill
  list)
- **Instance object**: holds a `shared_ptr<Type>` and delegates its behavior to
  the type object
- **Type registry**: indexes type objects by name and supports table-driven
  instance construction

## Type Object vs Inheritance

| | Inheritance | Type Object |
|---|---|---|
| Extension point | New class (compile time) | New data entry (runtime) |
| Memory cost | One vtable per class | One data record per type object |
| Expressing relationships | Inheritance tree | Type-object references / composition |
| Dynamic creation | Awkward | Natural |

## Implementation

```cpp
#include <map>
#include <memory>
#include <string>
#include <vector>

class CharacterType {
public:
    CharacterType(std::string name, int hp, int attack, int defense)
        : name_(std::move(name)), hp_(hp), attack_(attack), defense_(defense) {}

    const std::string& name() const { return name_; }
    int hp() const { return hp_; }
    int attack() const { return attack_; }
    int defense() const { return defense_; }

private:
    std::string name_;
    int hp_, attack_, defense_;
};

class Character {
public:
    Character(std::shared_ptr<CharacterType> type, std::string name)
        : type_(std::move(type)), name_(std::move(name)), currentHp_(type_->hp()) {}

    const std::string& name() const { return name_; }
    const std::string& typeName() const { return type_->name(); }
    int maxHp() const { return type_->hp(); }
    int currentHp() const { return currentHp_; }
    int attack() const { return type_->attack(); }
    int defense() const { return type_->defense(); }
    bool isAlive() const { return currentHp_ > 0; }

    void takeDamage(int damage) {
        int actual = damage - type_->defense();
        if (actual < 1) actual = 1;
        currentHp_ -= actual;
        if (currentHp_ < 0) currentHp_ = 0;
    }

private:
    std::shared_ptr<CharacterType> type_;
    std::string name_;
    int currentHp_;
};

class CharacterTypeRegistry {
public:
    void registerType(std::shared_ptr<CharacterType> type) {
        types_[type->name()] = std::move(type);
    }

    std::shared_ptr<CharacterType> getType(const std::string& name) const {
        auto it = types_.find(name);
        return (it != types_.end()) ? it->second : nullptr;
    }

    std::size_t count() const { return types_.size(); }

private:
    std::map<std::string, std::shared_ptr<CharacterType>> types_;
};
```

For the full runnable example, see the
[type-object project](../../cpp-mastery-roadmap/31-stage31/type-object).

## Usage Example

```cpp
#include <cassert>

CharacterTypeRegistry registry;
registry.registerType(std::make_shared<CharacterType>("Goblin", 30, 8, 2));
registry.registerType(std::make_shared<CharacterType>("Dragon", 500, 50, 30));

auto dragon = registry.getType("Dragon");
Character hero(dragon, "Aldric");
hero.takeDamage(60);    // actual damage = max(1, 60 - 30) = 30
assert(hero.currentHp() == 470);
```

## Best Practices

1. **Type objects are immutable**: make `CharacterType` members `const`; all
   changes go through `registerType` with a new object, so cached instances
   never disagree with their type
2. **Share type objects**: every instance of a kind shares one `CharacterType`
   — express ownership with `shared_ptr` and save memory
3. **Combine with Factory Method**: wrap name-based construction as
   `registry.create("Dragon", "Aldric")` so callers never touch raw pointers
4. **Serializable**: once type data moves from code into JSON/databases, the
   same code serves mods and level data alike

## Example

A runnable version of this topic:

- [type-object-demo.cpp](../../examples/type-object-demo.cpp)

## Summary

Type Object shifts the diversity of classes into the data layer — the standard
weapon for "many kinds, frequent change, runtime loading", and one of the
underlying patterns of ECS, dependency-injection containers, and game-engine
entity systems.
