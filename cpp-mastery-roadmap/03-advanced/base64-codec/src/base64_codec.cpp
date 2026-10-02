#include "base64_codec.hpp"

#include <stdexcept>

namespace base64 {

namespace {

const char kAlphabet[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int decodeChar(char c) {
    if (c >= 'A' && c <= 'Z') return c - 'A';
    if (c >= 'a' && c <= 'z') return c - 'a' + 26;
    if (c >= '0' && c <= '9') return c - '0' + 52;
    if (c == '+') return 62;
    if (c == '/') return 63;
    return -1;
}

}  // namespace

std::string encode(const std::vector<std::uint8_t>& data) {
    std::string out;
    out.reserve(((data.size() + 2) / 3) * 4);
    std::size_t i = 0;
    while (i + 3 <= data.size()) {
        std::uint32_t n = (data[i] << 16) | (data[i + 1] << 8) | data[i + 2];
        out += kAlphabet[(n >> 18) & 63];
        out += kAlphabet[(n >> 12) & 63];
        out += kAlphabet[(n >> 6) & 63];
        out += kAlphabet[n & 63];
        i += 3;
    }
    std::size_t rem = data.size() - i;
    if (rem == 1) {
        std::uint32_t n = data[i] << 16;
        out += kAlphabet[(n >> 18) & 63];
        out += kAlphabet[(n >> 12) & 63];
        out += "==";
    } else if (rem == 2) {
        std::uint32_t n = (data[i] << 16) | (data[i + 1] << 8);
        out += kAlphabet[(n >> 18) & 63];
        out += kAlphabet[(n >> 12) & 63];
        out += kAlphabet[(n >> 6) & 63];
        out += '=';
    }
    return out;
}

std::string encode(const std::string& text) {
    return encode(std::vector<std::uint8_t>(text.begin(), text.end()));
}

std::vector<std::uint8_t> decodeBytes(const std::string& encoded) {
    if (encoded.size() % 4 != 0)
        throw std::invalid_argument("base64 length must be a multiple of 4");

    // '=' may only appear as the final 1-2 characters of the whole string,
    // and everything after the first '=' must also be '='.
    auto firstPad = encoded.find('=');
    std::size_t pad = 0;
    if (firstPad != std::string::npos) {
        if (firstPad + 2 < encoded.size())
            throw std::invalid_argument("'=' must be at the end of the input");
        for (std::size_t i = firstPad; i < encoded.size(); ++i)
            if (encoded[i] != '=')
                throw std::invalid_argument("data after '=' padding");
        pad = encoded.size() - firstPad;
    }
    std::vector<std::uint8_t> out;
    out.reserve((encoded.size() / 4) * 3 - pad);

    for (std::size_t i = 0; i < encoded.size(); i += 4) {
        int vals[4];
        for (int j = 0; j < 4; ++j) {
            char c = encoded[i + j];
            if (c == '=') {
                // '=' only allowed in the final quantum
                if (i + 4 != encoded.size())
                    throw std::invalid_argument("'=' in middle of input");
                vals[j] = 0;
            } else {
                vals[j] = decodeChar(c);
                if (vals[j] < 0)
                    throw std::invalid_argument(std::string("invalid character: ") + c);
            }
        }
        std::uint32_t n = (vals[0] << 18) | (vals[1] << 12) | (vals[2] << 6) | vals[3];
        out.push_back((n >> 16) & 0xFF);
        if (encoded[i + 2] != '=') out.push_back((n >> 8) & 0xFF);
        if (encoded[i + 3] != '=') out.push_back(n & 0xFF);
    }
    return out;
}

std::string decode(const std::string& encoded) {
    auto bytes = decodeBytes(encoded);
    return std::string(bytes.begin(), bytes.end());
}

bool isValid(const std::string& encoded) {
    try {
        decodeBytes(encoded);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

}  // namespace base64
