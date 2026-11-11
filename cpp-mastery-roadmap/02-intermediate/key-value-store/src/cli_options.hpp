#pragma once

#include <stdexcept>
#include <string>

namespace key_value_store {

// The port has to be a number, and it has to be a port.
//
// The number functions of the standard library stop at the first character
// they cannot read, so "-p 8080abc" was taken as 8080 and the rest of the
// text was dropped without a word. A text that is not a number at all
// ended the program with an exception from the standard library, whose
// message says nothing about which argument was wrong.
inline int parsePort(const std::string& text) {
    std::size_t pos = 0;
    int value = 0;
    try {
        value = std::stoi(text, &pos);
    } catch (const std::exception&) {
        throw std::runtime_error("not a port: " + text);
    }
    if (pos != text.size() || value < 1 || value > 65535) {
        throw std::runtime_error("not a port: " + text);
    }
    return value;
}

}  // namespace key_value_store
