#include "my_error.hpp"

namespace my_error {

namespace {

class MyCategory final : public std::error_category {
public:
    const char* name() const noexcept override { return "my_error"; }

    std::string message(int code) const override {
        switch (static_cast<Errc>(code)) {
            case Errc::ok: return "success";
            case Errc::not_found: return "resource not found";
            case Errc::invalid_argument: return "invalid argument";
            case Errc::timeout: return "operation timed out";
            case Errc::permission_denied: return "permission denied";
            case Errc::resource_exhausted: return "resource exhausted";
            case Errc::io_error: return "I/O error";
        }
        return "unknown my_error code " + std::to_string(code);
    }

    // THE mapping hook: `ec == std::errc::timed_out` compares
    // default_error_condition(ec.value()) against the standard condition.
    // Without this override the comparison always fails.
    std::error_condition default_error_condition(int code) const noexcept override {
        switch (static_cast<Errc>(code)) {
            case Errc::ok:
                // 0 must match a default-constructed error_code(): both
                // sides normalize to (0, generic_category) here.
                return std::error_condition(0, std::generic_category());
            case Errc::not_found: return std::errc::no_such_file_or_directory;
            case Errc::invalid_argument: return std::errc::invalid_argument;
            case Errc::timeout: return std::errc::timed_out;
            case Errc::permission_denied: return std::errc::permission_denied;
            case Errc::resource_exhausted: return std::errc::no_space_on_device;
            case Errc::io_error: return std::errc::io_error;
        }
        // Out-of-range: self-referential, never matches standard conditions
        return std::error_condition(code, *this);
    }
};

}  // namespace

const std::error_category& category() noexcept {
    static MyCategory instance;
    return instance;
}

}  // namespace my_error
