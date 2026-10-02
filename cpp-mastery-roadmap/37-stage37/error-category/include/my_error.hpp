#pragma once

#include <string>
#include <system_error>

namespace my_error {

// Domain-specific error codes integrated with std::error_code, so they
// compose with std::expected<T, std::error_code>, future::get_exception
// checks, and any API that speaks error_code.
enum class Errc {
    ok = 0,
    not_found = 1,
    invalid_argument = 2,
    timeout = 3,
    permission_denied = 4,
    resource_exhausted = 5,
    io_error = 6,
};

// The custom category (singleton).
const std::error_category& category() noexcept;

inline std::error_code make_error_code(Errc e) noexcept {
    return std::error_code(static_cast<int>(e), category());
}

inline std::error_condition make_error_condition(Errc e) noexcept {
    // Map our codes onto standard conditions where a natural equivalent
    // exists, so is_error_condition_equality works both ways.
    using sc = std::errc;
    switch (e) {
        case Errc::ok: return std::error_condition(0, std::system_category());
        case Errc::not_found: return sc::no_such_file_or_directory;
        case Errc::invalid_argument: return sc::invalid_argument;
        case Errc::timeout: return sc::timed_out;
        case Errc::permission_denied: return sc::permission_denied;
        case Errc::resource_exhausted: return sc::no_space_on_device;
        case Errc::io_error: return sc::io_error;
    }
    // Unmapped/out-of-range: fall back to a self-referential condition
    // (compares equal to its own code, never matches a standard one).
    return std::error_condition(static_cast<int>(e), category());
}

}  // namespace my_error

// Enable implicit conversion Errc -> std::error_code (the standard hook).
namespace std {
template <>
struct is_error_code_enum<my_error::Errc> : true_type {};
}  // namespace std
