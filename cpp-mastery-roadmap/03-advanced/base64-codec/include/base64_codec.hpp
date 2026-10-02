#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace base64 {

// RFC 4648 base64: A-Z a-z 0-9 + / with '=' padding.
std::string encode(const std::vector<std::uint8_t>& data);
std::string encode(const std::string& text);

// Throws std::invalid_argument on invalid length or alphabet characters.
std::vector<std::uint8_t> decodeBytes(const std::string& encoded);

// Convenience: decode to raw bytes then to string.
std::string decode(const std::string& encoded);

bool isValid(const std::string& encoded);  // no throw

}  // namespace base64
