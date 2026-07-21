# Macro Pitfalls

## The Problem

C preprocessor macros are text substitution, not code. This leads to
subtle bugs with operator precedence, multiple evaluation, and scope.

## Examples

```cpp
// Classic: operator precedence
#define SQUARE(x) x * x

int a = 3;
int result = SQUARE(a + 1);  // a + 1 * a + 1 = 3 + 3 + 1 = 7, not 16!

// Fix: wrap in parentheses
#define SQUARE(x) ((x) * (x))
```

```cpp
// Multiple evaluation
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int x = 5;
int result = MAX(x++, 10);  // x incremented twice! UB if x changes sign
```

```cpp
// Scope issues
#define DECLARE_INT(name) int name = 0;

void func() {
    DECLARE_INT(temp)  // Expands to: int temp = 0;
    DECLARE_INT(temp)  // ERROR: redefinition!
}
```

```cpp
// String literal issues
#define LOG(msg) std::cout << msg

LOG("Hello " "World");     // OK: string concatenation
LOG(Hello World);           // ERROR: undeclared identifiers
LOG("Hello " + name);      // ERROR: + doesn't work with "Hello "
```

## Prevention

- Always wrap macro parameters in parentheses
- Consider inline functions or templates instead of macros
- Use `do { ... } while(0)` for multi-statement macros
- Prefix macros with project name to avoid collisions
- Use `#undef` after use if macros are temporary
- Prefer `constexpr` functions over function-like macros

## See Also

- [Implicit Conversions](implicit-conversions.md)
- [Signed Integer Overflow](signed-integer-overflow-assumptions.md)
