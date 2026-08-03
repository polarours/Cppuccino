#include <iostream>
#include <string>
#include <bitset>
#include <cstdint>
#include <limits>
#include <cmath>

// C++20 std::bit - bit manipulation utilities
// Demonstrates the new bit manipulation functions in <bit>

int main() {
    std::cout << "=== C++20 std::bit Demo ===\n\n";

    uint8_t value = 0b10101010;

    // bit_width - number of bits required to represent a value
    std::cout << "1. bit_width:\n";
    std::cout << "   bit_width(0) = " << std::bit_width(0u) << "\n";
    std::cout << "   bit_width(1) = " << std::bit_width(1u) << "\n";
    std::cout << "   bit_width(8) = " << std::bit_width(8u) << " (1000 binary)\n";
    std::cout << "   bit_width(255) = " << std::bit_width(255u) << " (11111111 binary)\n";

    // bit_ceil - smallest power of 2 >= value
    std::cout << "\n2. bit_ceil:\n";
    std::cout << "   bit_ceil(0) = " << std::bit_ceil(0u) << "\n";
    std::cout << "   bit_ceil(5) = " << std::bit_ceil(5u) << " (8)\n";
    std::cout << "   bit_ceil(8) = " << std::bit_ceil(8u) << " (8)\n";
    std::cout << "   bit_ceil(9) = " << std::bit_ceil(9u) << " (16)\n";

    // bit_floor - largest power of 2 <= value
    std::cout << "\n3. bit_floor:\n";
    std::cout << "   bit_floor(0) = " << std::bit_floor(0u) << "\n";
    std::cout << "   bit_floor(5) = " << std::bit_floor(5u) << " (4)\n";
    std::cout << "   bit_floor(8) = " << std::bit_floor(8u) << " (8)\n";
    std::cout << "   bit_floor(9) = " << std::bit_floor(9u) << " (8)\n";

    // bit_cast - reinterpret cast between same-sized types
    std::cout << "\n4. bit_cast:\n";
    float f = 3.14f;
    std::uint32_t bits = std::bit_cast<std::uint32_t>(f);
    std::cout << "   bit_cast<float to uint32>(3.14) = " << bits << "\n";
    
    // Reverse: convert bits back to float
    float back = std::bit_cast<float>(bits);
    std::cout << "   bit_cast<uint32 to float>(" << bits << ") = " << back << "\n";

    // popcount - count set bits
    std::cout << "\n5. popcount:\n";
    std::cout << "   popcount(0b10101010) = " << std::popcount(value) << " bits set\n";
    std::cout << "   popcount(0xFF) = " << std::popcount(0xFFu) << " bits set\n";
    std::cout << "   popcount(0) = " << std::popcount(0u) << " bits set\n";

    // countl/ countr - count leading/trailing zeros
    std::cout << "\n6. Count leading/trailing zeros:\n";
    std::cout << "   countl_zero(0x80) = " << std::countl_zero(0x80u) << "\n";
    std::cout << "   countr_zero(0x80) = " << std::countr_zero(0x80u) << "\n";
    std::cout << "   countl_zero(0xFF) = " << std::countl_zero(0xFFu) << "\n";
    std::cout << "   countr_zero(0xFF) = " << std::countr_zero(0xFFu) << "\n";

    // rotl/ rotr - rotate bits
    std::cout << "\n7. Rotate operations:\n";
    uint8_t rotated = std::rotl(value, 4);
    std::cout << "   rotl(0b10101010, 4) = 0b" 
              << std::bitset<8>(rotated) << "\n";
    
    rotated = std::rotr(value, 4);
    std::cout << "   rotr(0b10101010, 4) = 0b" 
              << std::bitset<8>(rotated) << "\n";

    // has_single_bit - check if power of 2
    std::cout << "\n8. has_single_bit (power of 2 check):\n";
    std::cout << "   has_single_bit(0) = " << std::boolalpha 
              << std::has_single_bit(0u) << "\n";
    std::cout << "   has_single_bit(4) = " << std::has_single_bit(4u) << "\n";
    std::cout << "   has_single_bit(5) = " << std::has_single_bit(5u) << "\n";
    std::cout << "   has_single_bit(8) = " << std::has_single_bit(8u) << "\n";

    // is_power_of_two (deprecated in C++23, use has_single_bit)
    std::cout << "   is_power_of_two(8) = " << std::boolalpha 
              << std::has_single_bit(8u) << "\n";

    return 0;
}
