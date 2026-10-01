// examples/type-object-demo.cpp
// Demonstrates the Type Object Pattern: represent "types" as data objects
// instead of hard-coding a subclass per type (game entities).
// Compile: g++ -std=c++20 -Wall -Wextra -o type-object-demo type-object-demo.cpp

#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace type_object {

// The "type" is a shared immutable object, not a class
struct EntityType {
    std::string name;
    int baseHealth;
    int baseAttack;
    std::string faction;
};

class Entity {
public:
    Entity(std::string id, std::shared_ptr<const EntityType> type)
        : id_(std::move(id)), type_(std::move(type)),
          health_(type_->baseHealth), attack_(type_->baseAttack) {}

    void takeDamage(int dmg) {
        health_ -= dmg;
        if (health_ < 0) health_ = 0;
    }

    void buff(int attackBonus) { attack_ += attackBonus; }

    std::string describe() const {
        return id_ + " (" + type_->name + ", " + type_->faction + ") hp="
             + std::to_string(health_) + " atk=" + std::to_string(attack_);
    }

    bool alive() const { return health_ > 0; }
    const std::shared_ptr<const EntityType>& type() const { return type_; }

private:
    std::string id_;
    std::shared_ptr<const EntityType> type_;
    int health_;
    int attack_;
};

// New types are just new data - no new class, no recompilation of entity logic
class EntityTypeCatalog {
public:
    static std::shared_ptr<const EntityType> make(const std::string& name, int hp,
                                                  int atk, const std::string& faction) {
        return std::make_shared<const EntityType>(EntityType{name, hp, atk, faction});
    }
};

} // namespace type_object

int main() {
    using namespace type_object;
    std::cout << "=== Type Object Pattern Demo ===\n\n";

    // Define "types" by handing out data objects
    auto goblin = EntityTypeCatalog::make("Goblin", 30, 8, "horde");
    auto knight = EntityTypeCatalog::make("Knight", 100, 15, "realm");
    auto dragon = EntityTypeCatalog::make("Dragon", 500, 60, "wild");

    std::vector<Entity> units;
    units.emplace_back("gob-1", goblin);
    units.emplace_back("gob-2", goblin);
    units.emplace_back("knt-1", knight);
    units.emplace_back("drg-1", dragon);

    std::cout << "Spawned units:\n";
    for (const auto& u : units) std::cout << "  " << u.describe() << "\n";

    std::cout << "\nBattle: dragon fights two goblins\n";
    units[0].takeDamage(60);   // goblin 1 dies
    units[1].takeDamage(25);   // goblin 2 survives
    units[3].takeDamage(45);   // dragon scratched

    for (const auto& u : units)
        std::cout << "  " << u.describe()
                  << (u.alive() ? "" : "  [dead]") << "\n";

    // Adding a faction/type later requires only data, not a new subclass
    auto phoenix = EntityTypeCatalog::make("Phoenix", 200, 40, "wild");
    units.emplace_back("phx-1", phoenix);
    units.back().buff(10);
    std::cout << "\nLate-added type:\n  " << units.back().describe() << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
