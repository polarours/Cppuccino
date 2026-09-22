# Student Management System v3

An improved version of the student management system, demonstrating the **borrowing ownership model** via `std::string_view` parameters.

## Improvements Over v2

### 1. `std::string_view` Borrowing Parameters

v2 passed string parameters by value, then moved them into the object. v3 borrows the input with `std::string_view` and makes its own copy internally:

```cpp
// v2: pass by value, then move
Student(int id, std::string name, int age, std::string major,
        double grade, std::string email, int phoneNumber);

// v3: borrow at the boundary, own internally
Student(int id, std::string_view name, int age, std::string_view major,
        double grade, std::string_view email, int phoneNumber);
```

Why this matters:

- **Caller side**: no temporary `std::string` construction on hot call paths — the signature makes the borrow explicit.
- **Owner side**: `Student` still owns its own `std::string` members. The object never holds a view, so there is no dangling risk.
- **API boundary**: the class documents its ownership model at the signature level — borrow at the boundary, own internally.

### 2. Setters Borrow Too

```cpp
void setName(std::string_view name) { name_ = name; }  // copy from view
```

The member stays a `std::string` (owning); the setter avoids taking a by-value `std::string` just to move it.

### 3. Everything From v2 Is Kept

Input validation, separated UI, `bool`/`std::optional` error handling, and the count-header file format from v2 are all unchanged.

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/student_management_v3
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## See Also

- [v1](../v1/): baseline with the problems
- [v2](../v2/): validation, error handling, UI separation
