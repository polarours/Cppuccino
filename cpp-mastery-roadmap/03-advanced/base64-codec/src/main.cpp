#include "base64_codec.hpp"

#include <iostream>

int main() {
    const std::string original = "Hello, Base64!";
    auto enc = base64::encode(original);
    auto dec = base64::decode(enc);

    std::cout << "original: " << original << "\n";
    std::cout << "encoded:  " << enc << "\n";
    std::cout << "decoded:  " << dec << "\n";
    std::cout << "roundtrip ok: " << std::boolalpha << (dec == original) << "\n";

    // RFC 4648 test vectors
    std::cout << "\"f\"   -> " << base64::encode("f") << " (expected Zg==)\n";
    std::cout << "\"fo\"  -> " << base64::encode("fo") << " (expected Zm8=)\n";
    std::cout << "\"foo\" -> " << base64::encode("foo") << " (expected Zm9v)\n";
    return 0;
}
