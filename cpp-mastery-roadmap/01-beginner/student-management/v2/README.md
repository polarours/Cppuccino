# Student Management System v2

An improved version of the student management system, demonstrating common code quality improvements.

## Improvements Over v1

### 1. Consistent I/O

v1 mixed `std::scanf` and `std::cin`, which is error-prone. v2 uses only C++ I/O streams.

### 2. Input Validation

v2 validates user input and handles invalid data gracefully:

```cpp
int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Invalid input. Please enter a number.\n";
        clearInput();
    }
}
```

### 3. Separation of UI and Domain Logic

v1 mixed UI code with business logic. v2 separates concerns:

- `Student` - data class
- `StudentManager` - business logic
- `main.cpp` - UI only

### 4. Proper Error Handling

v1 had no error handling. v2 returns `bool` or `std::optional` to indicate success/failure:

```cpp
bool addStudent(Student student);
std::optional<Student> findStudent(int id) const;
```

### 5. Modern C++ Idioms

- Use `const` references for getters
- Use `std::move` for efficient string passing
- Use `std::optional` instead of raw pointers
- Use `friend` for stream operators

### 6. File Format

v1 used a simple text format without structure. v2 includes a count header:

```
2
1
Alice
20
CS
3.8
alice@example.com
1234567
2
Bob
21
Math
3.9
bob@example.com
7654321
```

## Build

```bash
cmake -S . -B build
cmake --build build
```

## Run

```bash
./build/student_management_v2
```

## Test

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## Suggested Next Steps

- Add unit tests for edge cases (negative IDs, empty names, etc.)
- Add support for updating individual fields
- Add search by name or major
- Add data validation (age range, grade range, email format)
