#include "base64_codec.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool cond, const std::string& msg) {
    if (!cond) throw std::runtime_error(msg);
}

void test_rfc4648_vectors() {
    expect(base64::encode("") == "", "empty");
    expect(base64::encode("f") == "Zg==", "f -> Zg==");
    expect(base64::encode("fo") == "Zm8=", "fo -> Zm8=");
    expect(base64::encode("foo") == "Zm9v", "foo -> Zm9v");
    expect(base64::encode("foob") == "Zm9vYg==", "foob");
    expect(base64::encode("fooba") == "Zm9vYmE=", "fooba");
    expect(base64::encode("foobar") == "Zm9vYmFy", "foobar");
}

void test_decode_vectors() {
    expect(base64::decode("Zm9vYmFy") == "foobar", "decode foobar");
    expect(base64::decode("Zg==") == "f", "decode f");
    expect(base64::decode("Zm8=") == "fo", "decode fo");
    expect(base64::decode("") == "", "decode empty");
}

void test_roundtrip_all_byte_values() {
    std::string all;
    for (int i = 0; i < 256; ++i) all.push_back(static_cast<char>(i));
    for (std::size_t n : {std::size_t(1), std::size_t(2), std::size_t(3),
                          std::size_t(16), std::size_t(255), std::size_t(256)}) {
        std::string input = all.substr(0, n);
        expect(base64::decode(base64::encode(input)) == input,
               "roundtrip length " + std::to_string(n));
    }
}

void test_plus_slash_alphabet() {
    // 0xFB 0xFF 0xBF -> "+/+/" style: exercises + and /
    std::vector<std::uint8_t> data{0xFF, 0xEF, 0xBE};
    auto enc = base64::encode(data);
    expect(enc.find('+') != std::string::npos ||
           enc.find('/') != std::string::npos,
           "non-alphanumeric bytes produce + or /");
    expect(base64::decodeBytes(enc) == data, "binary roundtrip");
}

void test_invalid_inputs_throw() {
    expect(!base64::isValid("abc"), "length not multiple of 4");
    expect(!base64::isValid("ab!="), "invalid char before padding");
    expect(!base64::isValid("a!bc"), "'!' not in alphabet");
    expect(!base64::isValid("Zm9v=Zm9"), "'=' in middle");
    bool threw = false;
    try { base64::decodeBytes("!!"); } catch (const std::invalid_argument&) { threw = true; }
    expect(threw, "decodeBytes throws invalid_argument");
}

void test_valid_helper() {
    expect(base64::isValid("Zm9vYmFy"), "valid string");
    expect(base64::isValid(""), "empty is valid");
    expect(!base64::isValid("Zg="), "short padding invalid");
}

}  // namespace

int main() {
    try {
        test_rfc4648_vectors();
        test_decode_vectors();
        test_roundtrip_all_byte_values();
        test_plus_slash_alphabet();
        test_invalid_inputs_throw();
        test_valid_helper();
        std::cout << "All base64_codec tests passed\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "FAILED: " << e.what() << "\n";
        return 1;
    }
}
