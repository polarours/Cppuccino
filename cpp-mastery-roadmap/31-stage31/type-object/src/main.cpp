#include "type_object.hpp"

#include <iostream>

int main() {
    std::cout << "=== Type Object Demo ===\n\n";

    type_object::CharacterTypeRegistry registry;
    registry.registerType(std::make_shared<type_object::CharacterType>("Warrior", 100, 20, 10));
    registry.registerType(std::make_shared<type_object::CharacterType>("Mage", 60, 30, 5));

    auto warriorType = registry.getType("Warrior");
    type_object::Character hero(warriorType, "Arthur");
    std::cout << hero.name() << " (" << hero.typeName() << ") HP: " << hero.currentHp() << "\n";

    hero.takeDamage(15);
    std::cout << "After 15 dmg: HP " << hero.currentHp() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
