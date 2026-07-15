#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <iostream>

namespace type_object {

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
    int hp_;
    int attack_;
    int defense_;
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

    std::vector<std::string> typeNames() const {
        std::vector<std::string> names;
        for (const auto& [name, _] : types_) {
            names.push_back(name);
        }
        return names;
    }

    std::size_t count() const { return types_.size(); }

private:
    std::map<std::string, std::shared_ptr<CharacterType>> types_;
};

} // namespace type_object
