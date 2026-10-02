#include "bloom_filter.hpp"

#include <cmath>

namespace bloom_filter {

namespace {

// FNV-1a 64-bit
std::uint64_t fnv1a(const std::string& s) {
    std::uint64_t h = 1469598103934665603ULL;
    for (unsigned char c : s) {
        h ^= c;
        h *= 1099511628211ULL;
    }
    return h;
}

}  // namespace

BloomFilter::BloomFilter(std::size_t bits, std::size_t hashes)
    : m_(bits), k_(hashes), bits_((bits + 7) / 8, 0) {}

std::pair<std::size_t, std::size_t> BloomFilter::baseHashes(const std::string& s) const {
    // Two independent-ish hashes from different seeds
    std::uint64_t h1 = fnv1a(s);
    std::uint64_t h2 = fnv1a(std::string(s.rbegin(), s.rend())) | 1ULL;  // odd
    return {static_cast<std::size_t>(h1 % m_),
            static_cast<std::size_t>(h2 % m_)};
}

void BloomFilter::setBit(std::size_t idx) {
    bits_[idx / 8] |= static_cast<std::uint8_t>(1u << (idx % 8));
}

bool BloomFilter::testBit(std::size_t idx) const {
    return (bits_[idx / 8] >> (idx % 8)) & 1u;
}

void BloomFilter::add(const std::string& item) {
    auto [h1, h2] = baseHashes(item);
    for (std::size_t i = 0; i < k_; ++i) {
        setBit((h1 + i * h2) % m_);
    }
}

bool BloomFilter::contains(const std::string& item) const {
    auto [h1, h2] = baseHashes(item);
    for (std::size_t i = 0; i < k_; ++i) {
        if (!testBit((h1 + i * h2) % m_)) return false;
    }
    return true;
}

double BloomFilter::falsePositiveRate(std::size_t inserted) const {
    if (m_ == 0) return 1.0;
    double exponent = -static_cast<double>(k_) *
                      static_cast<double>(inserted) / static_cast<double>(m_);
    double base = 1.0 - std::exp(exponent);
    return std::pow(base, static_cast<double>(k_));
}

std::size_t BloomFilter::setBits() const {
    std::size_t count = 0;
    for (std::uint8_t b : bits_) {
        while (b) { count += b & 1u; b >>= 1; }
    }
    return count;
}

}  // namespace bloom_filter
