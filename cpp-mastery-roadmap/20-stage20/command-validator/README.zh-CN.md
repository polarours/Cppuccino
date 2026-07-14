# 命令验证器

一个命令验证系统。

## 学习目标

- 理解命令验证
- 练习验证规则
- 学习错误报告
- 实现验证链

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/command_validator_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
