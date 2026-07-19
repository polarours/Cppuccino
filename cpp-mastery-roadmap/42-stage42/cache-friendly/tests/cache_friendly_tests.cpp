#include "cache_friendly.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_aos_basic() {
    cache_friendly::AoSParticleSystem system;
    system.add(0, 0, 0, 1.0f);
    system.add(1, 2, 3, 2.0f);

    expect(system.size() == 2, "expected 2 particles");
    expect(system.totalMass() == 3.0f, "expected mass 3.0");
}

void test_soa_basic() {
    cache_friendly::SoAParticleSystem system;
    system.add(0, 0, 0, 1.0f);
    system.add(1, 2, 3, 2.0f);

    expect(system.size() == 2, "expected 2 particles");
    expect(system.totalMass() == 3.0f, "expected mass 3.0");
}

void test_aos_update() {
    cache_friendly::AoSParticleSystem system;
    system.add(0, 0, 0, 1.0f);

    system.updatePositions(1.0f);

    expect(system.size() == 1, "expected 1 particle");
}

void test_soa_update() {
    cache_friendly::SoAParticleSystem system;
    system.add(0, 0, 0, 1.0f);

    system.updatePositions(1.0f);

    expect(system.size() == 1, "expected 1 particle");
}

void test_soa_resize() {
    cache_friendly::SoAParticleSystem system;
    system.add(1, 2, 3, 1.0f);
    system.add(4, 5, 6, 2.0f);

    expect(system.size() == 2, "expected 2");
    expect(system.totalMass() == 3.0f, "expected 3.0");
}

} // namespace

int main() {
    try {
        std::cout << "Cache Friendly Tests:\n";
        test_aos_basic();
        std::cout << "  test_aos_basic: PASS\n";

        test_soa_basic();
        std::cout << "  test_soa_basic: PASS\n";

        test_aos_update();
        std::cout << "  test_aos_update: PASS\n";

        test_soa_update();
        std::cout << "  test_soa_update: PASS\n";

        test_soa_resize();
        std::cout << "  test_soa_resize: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "cache_friendly_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\ncache_friendly_tests passed" << std::endl;
    return 0;
}
