# CMake & Build Systems

## Why Do We Need a Build System?

A single `.cpp` file can be compiled with `g++ main.cpp -o app`, but a real C++
project has dozens of sources, header directories, third-party libraries, and a
test framework — a build system must automate it. CMake is the most popular
cross-platform build tool in the C++ world: you describe targets in
`CMakeLists.txt`, and CMake generates the platform-native build files (Unix
Makefiles, Ninja, Visual Studio solutions, …).

> **Core idea**: `CMakeLists.txt` declares *what* to build; CMake figures out
> *how* to build it.

## Minimal Project

```cmake
cmake_minimum_required(VERSION 3.16)         # minimum required CMake version
project(my_app LANGUAGES CXX)                # project name + languages

set(CMAKE_CXX_STANDARD 20)                   # C++ standard
set(CMAKE_CXX_STANDARD_REQUIRED ON)          # error out if the standard is unavailable

add_executable(my_app
    src/main.cpp
    src/calculator.cpp
)
target_include_directories(my_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
```

## Modern CMake: The Three Core Commands

| Command | Purpose |
|---|---|
| `target_include_directories()` | header search paths for a target |
| `target_compile_options()` | compile options for a target |
| `target_link_libraries()` | libraries a target links against |

```cmake
add_library(calc STATIC src/calc.cpp)
target_include_directories(calc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE calc)      # PRIVATE: calc is an implementation detail of app
```

`PUBLIC` / `PRIVATE` / `INTERFACE` control how requirements propagate:

- `PRIVATE`: applies only to the current target
- `PUBLIC`: applies to the current target *and* everything that depends on it
- `INTERFACE`: applies only to consumers (header-only libraries)

## Test Integration

```cmake
enable_testing()                             # turn CTest on

add_executable(calculator_tests
    tests/calculator_tests.cpp
)
target_link_libraries(calculator_tests PRIVATE calc)

add_test(NAME CalculatorTests COMMAND calculator_tests)  # register with CTest

# build and run the tests
# cmake -S . -B build -DBUILD_TESTING=ON
# cmake --build build
# ctest --test-dir build
```

## Common Variables and Generator Expressions

| Variable / expression | Meaning |
|---|---|
| `CMAKE_CURRENT_SOURCE_DIR` | directory of the current `CMakeLists.txt` |
| `$<TARGET_FILE:name>` | full path of a target's file (generator expression) |
| `$<BUILD_INTERFACE:...>` | path expanded at build time |
| `$<INSTALL_INTERFACE:...>` | path expanded at install time |

```cmake
target_include_directories(my_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
```

## find_package: Dependency Management

```cmake
# locate already-installed libraries
find_package(Threads REQUIRED)               # the Threads library
target_link_libraries(app PRIVATE Threads::Threads)

find_package(OpenSSL REQUIRED)               # OpenSSL
target_link_libraries(app PRIVATE OpenSSL::SSL OpenSSL::Crypto)
```

## Subdirectory Layout

```
project/
├── CMakeLists.txt          # top-level entry point
├── src/
│   └── CMakeLists.txt      # add_subdirectory(src)
├── include/
│   └── CMakeLists.txt      # install(TARGETS ... EXPORT ...)
├── tests/
│   └── CMakeLists.txt      # enable_testing() + add_test
└── CMakeLists.txt.in       # toolchain file template
```

Top-level `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_project)

add_subdirectory(src)
add_subdirectory(tests)
```

## Complete Example: A Multi-Target Project

```cmake
cmake_minimum_required(VERSION 3.16)
project(key_value_store)

find_package(Threads REQUIRED)               # kvs_server links Threads::Threads

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# library target
set(LIB_SOURCES
    src/store/key_value_store.cpp
    src/protocol/protocol.cpp
    src/server/server.cpp
)
add_library(kvs_core STATIC ${LIB_SOURCES})
target_include_directories(kvs_core PUBLIC src)

# executable target
add_executable(kvs_server src/main.cpp)
target_link_libraries(kvs_server PRIVATE kvs_core Threads::Threads)

# tests
enable_testing()
add_executable(kvs_tests tests/kvs_tests.cpp)
target_link_libraries(kvs_tests PRIVATE kvs_core)
add_test(NAME KVSTests COMMAND kvs_tests)
```

## Example

A project in this repository that uses the three core commands, a library
target, an executable and the test integration:

- [chat-server/CMakeLists.txt](../../cpp-mastery-roadmap/04-stage4/chat-server/CMakeLists.txt)

## Best Practices

1. **Always require `cmake_minimum_required(VERSION 3.16)` or newer**: 3.16 is
   the threshold for modern CMake
2. **Prefer `target_*` commands over global variables**:
   `include_directories()` / `link_directories()` are the old style and
   pollute everything
3. **`PRIVATE` is both the default and the safest choice**: reach for `PUBLIC`
   only with header-only libraries
4. **Put `enable_testing()` after `project()`** so `add_test()` registers
   correctly
5. **Manage external dependencies with `find_package`**: never hard-code
   library paths

## Summary

CMake is the standard step from "compiling one file" to "multi-target,
cross-platform, testable C++ projects". The core is understanding the
`target_*` trio (include directories, compile options, link libraries) and the
propagation semantics of `PUBLIC` / `PRIVATE` / `INTERFACE`.
