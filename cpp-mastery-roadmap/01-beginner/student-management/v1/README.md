# Student Management System v1

The baseline version of the student management system. This is a first attempt: it works, but contains the typical problems that motivate v2 and v3.

## Features

- Add / remove / find / update students
- Save to and load from a text file
- Interactive menu-driven console UI

## Known Drawbacks (Motivating v2/v3)

- **Mixed I/O**: uses `std::scanf` together with `std::cin`/`std::getline` — mixing C and C++ I/O on the same stream is error-prone.
- **No input validation**: malformed numeric input corrupts the stream state.
- **UI mixed with domain logic**: `StudentManager::displayAllStudents()` and `updateStudent()` read from `std::cin` directly.
- **No error handling**: `addStudent` returns `void`; `getStudent` returns a raw `Student*` that can dangle or be null.

## Build

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## Run

```bash
./build/student_management_v1
```

## Test

```bash
ctest --test-dir build --output-on-failure
```

## See Also

- [v2](../v2/): input validation, error handling, separated UI
- [v3](../v3/): `std::string_view` borrowing parameters
