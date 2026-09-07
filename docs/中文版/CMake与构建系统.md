# CMake 与构建系统 (CMake & Build Systems)

## 为什么需要构建系统？

单个 `.cpp` 文件可以直接 `g++ main.cpp -o app`，但一个 C++ 项目有几十个源文件、头文件目录、依赖库、测试框架——必须用构建系统自动化。CMake 是 C++ 领域最流行的跨平台构建工具，用 `CMakeLists.txt` 描述构建目标，生成平台原生构建文件（Unix Makefile、Ninja、Visual Studio 等）。

> **核心思想**：`CMakeLists.txt` 声明"要构建什么"，CMake 生成"怎么构建它"。

## 最小项目

```cmake
cmake_minimum_required(VERSION 3.16)
project(my_app LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(my_app
    src/main.cpp
    src/calculator.cpp
)
target_include_directories(my_app PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
```

## Modern CMake：三个核心命令

| 命令 | 作用 |
|---|---|
| `target_include_directories()` | 设置目标的头文件搜索路径 |
| `target_compile_options()` | 设置目标的编译选项 |
| `target_link_libraries()` | 设置目标链接的库 |

```cmake
add_library(calc STATIC src/calc.cpp)
target_include_directories(calc PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(app src/main.cpp)
target_link_libraries(app PRIVATE calc)
```

`PUBLIC` / `PRIVATE` / `INTERFACE` 决定依赖传播范围：
- `PRIVATE`：只在当前目标有效
- `PUBLIC`：当前目标和依赖它的目标都有效
- `INTERFACE`：只在依赖目标有效（纯头文件库）

## 测试集成

```cmake
enable_testing()

add_executable(calculator_tests tests/calculator_tests.cpp)
target_link_libraries(calculator_tests PRIVATE calc)
add_test(NAME CalculatorTests COMMAND calculator_tests)

# cmake -S . -B build -DBUILD_TESTING=ON
# cmake --build build
# ctest --test-dir build
```

## 常用变量与生成器表达式

| 变量/表达式 | 含义 |
|---|---|
| `CMAKE_CURRENT_SOURCE_DIR` | 当前 `CMakeLists.txt` 所在目录 |
| `$<TARGET_FILE:name>` | 目标文件的完整路径（生成器表达式） |
| `$<BUILD_INTERFACE:...>` | 构建时展开的路径 |
| `$<INSTALL_INTERFACE:...>` | 安装时展开的路径 |

```cmake
target_include_directories(my_lib PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
```

## find_package：依赖管理

```cmake
find_package(Threads REQUIRED)
target_link_libraries(app PRIVATE Threads::Threads)

find_package(OpenSSL REQUIRED)
target_link_libraries(app PRIVATE OpenSSL::SSL OpenSSL::Crypto)
```

## 子目录结构

```
project/
├── CMakeLists.txt          # 顶层入口
├── src/CMakeLists.txt      # add_subdirectory(src)
├── include/CMakeLists.txt  # install(TARGETS ...)
└── tests/CMakeLists.txt    # enable_testing() + add_test
```

## 完整示例：多目标项目

```cmake
cmake_minimum_required(VERSION 3.16)
project(key_value_store)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_SOURCES
    src/store/key_value_store.cpp
    src/protocol/protocol.cpp
)
add_library(kvs_core STATIC ${LIB_SOURCES})
target_include_directories(kvs_core PUBLIC src)

add_executable(kvs_server src/main.cpp)
target_link_libraries(kvs_server PRIVATE kvs_core Threads::Threads)

enable_testing()
add_executable(kvs_tests tests/kvs_tests.cpp)
target_link_libraries(kvs_tests PRIVATE kvs_core)
add_test(NAME KVSTests COMMAND kvs_tests)
```

## 最佳实践

1. **始终指定 `cmake_minimum_required(VERSION 3.16)` 以上**：3.16 是现代 CMake 的门槛
2. **用 `target_*` 命令代替全局变量**：`include_directories()` / `link_directories()` 是旧风格，全局污染
3. **`PRIVATE` 是默认值也是最安全的选择**：除非是纯头文件库，否则不用 `PUBLIC`
4. **`enable_testing()` 放在 `project()` 之后**：`add_test()` 才能正确注册
5. **用 `find_package` 管理外部依赖**：不要硬编码库路径

## 总结

CMake 是 C++ 项目从"单文件编译"走向"多目标、跨平台、可测试项目"的标准工具。核心是理解 `target_*` 三件套和 `PUBLIC/PRIVATE/INTERFACE` 的传播语义。
