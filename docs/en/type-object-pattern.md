# 类型对象模式 (Type Object Pattern)

## 什么是类型对象模式？

类型对象模式（Type Object Pattern）通过把"类型"建模为一个独立对象，让"实例"持有一个指向类型对象的引用，从而在**不引入新类**的前提下创建大量相似但参数不同的种类。

> **核心思想**：分类信息从类（编译期）下沉到对象（运行期），把 N 个类替换为 1 个类 + N 个类型对象。

## 适用场景

- 需要大量相似子类（RPG 怪物、棋子类、UI 主题……）
- 分类需要运行时创建或从配置/数据中加载
- 分类之间的关系（HP 相加、技能继承）需要在数据层而非继承层表达

## 核心概念

- **类型对象（Type Object）**：描述一类事物的属性（HP/攻击/防御/技能列表）
- **实例对象**：持有一个 `shared_ptr<Type>` 的对象，其行为委派给类型对象
- **类型注册表**：用名字索引类型对象，支持查表构造实例

## 与继承/多态的关系

| | 继承 | Type Object |
|---|---|---|
| 扩展点 | 新类（编译期） | 新数据条目（运行期） |
| 内存开销 | 每个类一份 vtable | 每个类型对象一份数据 |
| 关系表达 | 继承树 | 类型对象引用 / 组合 |
| 动态创建 | 不友好 | 自然支持 |

## 代码实现

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

完整可运行示例见 [type-object 项目](../../cpp-mastery-roadmap/31-stage31/type-object)。

## 使用示例

```cpp
CharacterTypeRegistry registry;
registry.registerType(std::make_shared<CharacterType>("Goblin", 30, 8, 2));
registry.registerType(std::make_shared<CharacterType>("Dragon", 500, 50, 30));

auto dragon = registry.getType("Dragon");
Character hero(dragon, "Aldric");
hero.takeDamage(60);    // 实际扣血 = max(1, 60 - 30) = 30
assert(hero.currentHp() == 470);
```

## 最佳实践

1. **类型对象不可变**：让 `CharacterType` 成员 `const`，所有变化通过 `registerType` 走新对象，避免实例缓存与类型对象错位
2. **共享类型对象**：同一类型的所有实例共享同一个 `CharacterType`，节省内存；用 `shared_ptr` 表达所有权
3. **配合工厂方法**：把"按名字建实例"封装成 `registry.create("Dragon", "Aldric")`，调用方不直接拿原始指针
4. **可序列化**：把类型数据从代码移到 JSON/数据库后，可以用同一份代码处理 mod 与关卡数据

## 总结

Type Object 把类的多样性平移到数据层，是处理"分类多、变化频繁、需运行时加载"问题的标准武器——也是 ECS、依赖注入容器、游戏引擎 entity 系统的底层模式之一。
