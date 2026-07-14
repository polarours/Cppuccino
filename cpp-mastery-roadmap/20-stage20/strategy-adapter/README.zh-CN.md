# 策略适配器

一个策略适配器，用于适配不兼容的接口。

## 学习目标

- 理解策略上下文中的适配器模式
- 练习接口适配
- 学习遗留代码集成
- 实现接口桥接

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/strategy_adapter_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
