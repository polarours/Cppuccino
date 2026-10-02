#include "bloom_filter.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_no_false_negatives() {
    bloom_filter::BloomFilter bf(4096, 4);
    for (int i = 0; i < 200; ++i)
        bf.add("key" + std::to_string(i));
    for (int i = 0; i < 200; ++i)
        expect(bf.contains("key" + std::to_string(i)),
               "inserted item must always be found: key" + std::to_string(i));
}

void test_empty_filter_rejects_most() {
    bloom_filter::BloomFilter bf(8192, 4);
    int hits = 0;
    for (int i = 0; i < 500; ++i)
        if (bf.contains("never-added-" + std::to_string(i))) ++hits;
    // With m=8192, k=4, FPR at n=0 ~ 0: a handful of accidental hits allowed
    expect(hits < 10, "empty filter should reject almost everything");
}

void test_false_positive_rate_within_bound() {
    bloom_filter::BloomFilter bf(10240, 7);
    for (int i = 0; i < 1000; ++i) bf.add("user" + std::to_string(i));
    int fp = 0;
    const int probes = 2000;
    for (int i = 0; i < probes; ++i)
        if (bf.contains("other" + std::to_string(i))) ++fp;
    double observed = static_cast<double>(fp) / probes;
    double predicted = bf.falsePositiveRate(1000);
    // Observed must be in the same ballpark as the formula (3x slack)
    expect(observed <= predicted * 3.0 + 0.01,
           "observed FPR too high: " + std::to_string(observed) +
           " vs predicted " + std::to_string(predicted));
}

void test_set_bits_grow() {
    bloom_filter::BloomFilter bf(1000, 3);
    auto before = bf.setBits();
    bf.add("x");
    auto after = bf.setBits();
    expect(after >= before && after <= before + 3, "add sets at most k bits");
    expect(bf.contains("x"), "added item found");
}

void test_dimensions() {
    bloom_filter::BloomFilter bf(100, 6);
    expect(bf.bitCount() == 100, "bit count");
    expect(bf.hashCount() == 6, "hash count");
    // packed storage: 100 bits -> 13 bytes internally; setBits <= 100
    expect(bf.setBits() <= 100, "bits bounded");
}

void test_deterministic() {
    bloom_filter::BloomFilter a(512, 3), b(512, 3);
    a.add("same"); b.add("same");
    expect(a.contains("same") == b.contains("same"), "deterministic across instances");
}

}  // namespace

int main() {
    try {
        test_no_false_negatives();
        test_empty_filter_rejects_most();
        test_false_positive_rate_within_bound();
        test_set_bits_grow();
        test_dimensions();
        test_deterministic();
        std::cout << "All bloom_filter tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
