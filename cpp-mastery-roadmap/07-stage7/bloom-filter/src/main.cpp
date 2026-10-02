#include "bloom_filter.hpp"

#include <iostream>

int main() {
    bloom_filter::BloomFilter bloom(1024, 5);
    bloom.add("alice@example.com");
    bloom.add("bob@example.com");

    std::cout << std::boolalpha;
    std::cout << "alice present? " << bloom.contains("alice@example.com") << "\n";
    std::cout << "mallory present? " << bloom.contains("mallory@example.com") << "\n";
    std::cout << "bits set: " << bloom.setBits() << " / " << bloom.bitCount() << "\n";
    std::cout << "est. FPR at n=2: " << bloom.falsePositiveRate(2) << "\n";
    return 0;
}
