#include "abstract_factory.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Abstract Factory Demo ===\n\n";

    // Create Windows UI
    auto winFactory = std::make_unique<abstract_factory::WindowsFactory>();
    auto winButton = winFactory->createButton();
    auto winCheckbox = winFactory->createCheckbox();

    std::cout << "Windows UI:\n";
    winButton->render();
    winCheckbox->render();

    // Create Linux UI
    auto linuxFactory = std::make_unique<abstract_factory::LinuxFactory>();
    auto linuxButton = linuxFactory->createButton();
    auto linuxCheckbox = linuxFactory->createCheckbox();

    std::cout << "\nLinux UI:\n";
    linuxButton->render();
    linuxCheckbox->render();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
