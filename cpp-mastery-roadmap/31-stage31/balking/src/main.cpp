#include "balking.hpp"

#include <iostream>

int main() {
    std::cout << "=== Balking Pattern Demo ===\n\n";

    balking_pattern::Document doc;
    doc.save();
    std::cout << "Save before open: " << std::boolalpha << doc.isSaved() << "\n";

    doc.open();
    doc.save();
    std::cout << "Save after open: " << doc.isSaved() << "\n";

    balking_pattern::Sensor sensor(100);
    std::cout << "Read 50: " << sensor.read(50) << "\n";
    std::cout << "Read 150: " << sensor.read(150) << "\n";

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
