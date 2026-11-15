# CMake 模式

程序化 CMakeLists.txt 生成。

生成出来的文件是要能用的：测试链接的是项目里的库，宏用
`add_compile_definitions` 写出来，整个工程能配置、能构建、测试也能过。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
