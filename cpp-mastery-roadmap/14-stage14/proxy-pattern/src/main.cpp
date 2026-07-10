#include "proxy_pattern.hpp"

#include <iostream>
#include <memory>

int main() {
    std::cout << "=== Proxy Pattern Demo ===\n\n";

    // Create proxy (lazy loading)
    auto proxy = std::make_shared<proxy_pattern::ImageProxy>("photo.jpg");

    std::cout << "Info before display: " << proxy->getInfo() << "\n";

    // First display triggers real image loading
    proxy->display();

    std::cout << "Info after display: " << proxy->getInfo() << "\n";

    // Second display uses cached image
    proxy->display();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
