#include "cache_friendly.hpp"

#include <iostream>

int main() {
    std::cout << "=== Cache Friendly Demo ===\n\n";

    cache_friendly::AoSParticleSystem aos;
    for (int i = 0; i < 5; ++i) aos.add(i, i, i, 1.0f);

    cache_friendly::SoAParticleSystem soa;
    for (int i = 0; i < 5; ++i) soa.add(i, i, i, 1.0f);

    std::cout << "AoS mass: " << aos.totalMass() << "\n";
    std::cout << "SoA mass: " << soa.totalMass() << "\n";

    aos.updatePositions(0.1f);
    soa.updatePositions(0.1f);

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
