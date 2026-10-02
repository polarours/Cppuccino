#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace bloom_filter {

// Space-probabilistic set: add() never false-negatives, contains() may
// false-positive. k hash functions derived from two base hashes
// (double hashing: h_i = h1 + i * h2).
class BloomFilter {
public:
    // bits: bit array size; hashes: number of hash functions (>= 1)
    BloomFilter(std::size_t bits, std::size_t hashes);

    void add(const std::string& item);
    bool contains(const std::string& item) const;

    // Estimated false-positive rate: (1 - e^(-k*n/m))^k
    double falsePositiveRate(std::size_t inserted) const;

    std::size_t bitCount() const { return m_; }
    std::size_t hashCount() const { return k_; }
    std::size_t setBits() const;   // how many bits are currently 1

private:
    std::pair<std::size_t, std::size_t> baseHashes(const std::string& s) const;
    void setBit(std::size_t idx);
    bool testBit(std::size_t idx) const;

    std::size_t m_;          // number of bits
    std::size_t k_;          // number of hash functions
    std::vector<std::uint8_t> bits_;  // packed bits
};

}  // namespace bloom_filter
