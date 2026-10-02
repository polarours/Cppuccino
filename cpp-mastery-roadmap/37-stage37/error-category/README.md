# Custom Error Category

A domain error enum wired into `std::error_code` via
`is_error_code_enum`, with `make_error_condition` mapping onto standard
conditions (timed_out, permission_denied, ...).

## Learning Goals

- The three-piece kit: enum + category singleton + is_error_code_enum trait
- error_code vs error_condition: values compare across categories
- Why message() must handle unknown codes (forward compatibility)
- Composing with std::system_error / std::expected<T, error_code>

## Non-Goals

- Hierarchical categories
- POSIX errno mirroring
- Exception-heavy APIs

## Build / Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
