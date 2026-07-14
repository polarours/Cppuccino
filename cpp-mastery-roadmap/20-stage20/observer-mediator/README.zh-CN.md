# 观察者中介者

观察者模式与中介者的组合，用于事件协调。

## 学习目标

- 理解观察者-中介者组合
- 练习事件协调
- 学习集中式事件处理
- 实现事件路由

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/observer_mediator_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
