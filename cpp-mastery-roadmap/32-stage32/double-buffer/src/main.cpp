#include "double_buffer.hpp"

#include <iostream>

int main() {
    std::cout << "=== Double Buffer Demo ===\n\n";

    double_buffer::DoubleBuffer<int> buf(5);
    std::vector<int> data = {10, 20, 30, 40, 50};

    buf.write(data);
    buf.swap();

    for (std::size_t i = 0; i < 5; ++i) {
        std::cout << "Front[" << i << "] = " << buf.getFront(i) << "\n";
    }

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
