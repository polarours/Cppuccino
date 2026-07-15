#include "type_object.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_create_from_type() {
    type_object::CharacterTypeRegistry registry;
    registry.registerType(std::make_shared<type_object::CharacterType>("Warrior", 100, 20, 10));

    auto type = registry.getType("Warrior");
    type_object::Character hero(type, "Arthur");

    expect(hero.typeName() == "Warrior", "expected Warrior");
    expect(hero.maxHp() == 100, "expected 100 hp");
    expect(hero.attack() == 20, "expected 20 attack");
    expect(hero.isAlive(), "expected alive");
}

void test_damage_with_defense() {
    type_object::CharacterTypeRegistry registry;
    registry.registerType(std::make_shared<type_object::CharacterType>("Mage", 60, 30, 5));

    auto type = registry.getType("Mage");
    type_object::Character mage(type, "Gandalf");

    mage.takeDamage(15);
    expect(mage.currentHp() == 50, "expected 50 hp (15 - 5 defense = 10 damage)");
}

void test_min_damage() {
    type_object::CharacterTypeRegistry registry;
    registry.registerType(std::make_shared<type_object::CharacterType>("Tank", 200, 10, 50));

    auto type = registry.getType("Tank");
    type_object::Character tank(type, "Bulwark");

    tank.takeDamage(5);
    expect(tank.currentHp() == 199, "expected 199 hp (minimum 1 damage)");
}

void test_type_registry() {
    type_object::CharacterTypeRegistry registry;
    registry.registerType(std::make_shared<type_object::CharacterType>("Warrior", 100, 20, 10));
    registry.registerType(std::make_shared<type_object::CharacterType>("Mage", 60, 30, 5));

    expect(registry.count() == 2, "expected 2 types");
    expect(registry.getType("Warrior") != nullptr, "expected Warrior");
    expect(registry.getType("Rogue") == nullptr, "expected no Rogue");
}

} // namespace

int main() {
    try {
        std::cout << "Type Object Tests:\n";
        test_create_from_type();
        std::cout << "  test_create_from_type: PASS\n";

        test_damage_with_defense();
        std::cout << "  test_damage_with_defense: PASS\n";

        test_min_damage();
        std::cout << "  test_min_damage: PASS\n";

        test_type_registry();
        std::cout << "  test_type_registry: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "type_object_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ntype_object_tests passed" << std::endl;
    return 0;
}
