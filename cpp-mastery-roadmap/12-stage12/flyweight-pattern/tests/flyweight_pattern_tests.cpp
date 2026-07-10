#include "flyweight_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_font_factory() {
    flyweight_pattern::FontFactory factory;

    auto font1 = factory.getFont("Arial", 12, false);
    auto font2 = factory.getFont("Arial", 12, false);

    expect(font1.get() == font2.get(), "expected same font instance");
    expect(factory.size() == 1, "expected 1 font in factory");
}

void test_different_fonts() {
    flyweight_pattern::FontFactory factory;

    auto font1 = factory.getFont("Arial", 12, false);
    auto font2 = factory.getFont("Arial", 14, false);
    auto font3 = factory.getFont("Arial", 12, true);

    expect(font1.get() != font2.get(), "expected different fonts");
    expect(font1.get() != font3.get(), "expected different fonts");
    expect(factory.size() == 3, "expected 3 fonts");
}

void test_text_character() {
    flyweight_pattern::FontFactory factory;
    auto font = factory.getFont("Arial", 12, false);

    flyweight_pattern::TextCharacter ch('A', font);
    expect(ch.getCharacter() == 'A', "expected character A");
    expect(ch.getFont().get() == font.get(), "expected same font");
}

void test_shared_state() {
    flyweight_pattern::FontFactory factory;

    auto font1 = factory.getFont("Arial", 12, false);
    auto font2 = factory.getFont("Arial", 12, false);

    // Both characters share the same font
    flyweight_pattern::TextCharacter ch1('A', font1);
    flyweight_pattern::TextCharacter ch2('B', font2);

    expect(ch1.getFont().get() == ch2.getFont().get(), "expected shared font");
}

} // namespace

int main() {
    try {
        std::cout << "Flyweight Pattern Tests:\n";
        test_font_factory();
        std::cout << "  test_font_factory: PASS\n";

        test_different_fonts();
        std::cout << "  test_different_fonts: PASS\n";

        test_text_character();
        std::cout << "  test_text_character: PASS\n";

        test_shared_state();
        std::cout << "  test_shared_state: PASS\n";

    } catch (const std::string& exception) {
        std::cerr << "flyweight_pattern_tests failed: " << exception << std::endl;
        return 1;
    }

    std::cout << "\nflyweight_pattern_tests passed" << std::endl;
    return 0;
}