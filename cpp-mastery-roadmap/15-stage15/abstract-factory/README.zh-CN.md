# 抽象工厂模式

一个抽象工厂模式实现，用于创建相关对象族。

## 学习目标

- 理解抽象工厂模式
- 练习族对象创建
- 学习平台特定工厂
- 实现一致的产品族

## 非目标

- 构建生产级 GUI 框架
- 实现复杂的控件层次结构
- 支持分布式工厂

## 功能

- Button 和 Checkbox 抽象产品
- Windows、Linux、Mac 具体产品
- GUIFactory 抽象工厂
- 平台特定的具体工厂
- 一致的产品族

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/abstract_factory_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "abstract_factory.hpp"

auto factory = std::make_unique<abstract_factory::LinuxFactory>();
auto button = factory->createButton();
auto checkbox = factory->createCheckbox();

button->render();    // [Linux Button]
checkbox->render();  // [Linux Checkbox]
```

## 建议的下一步

- 添加更多产品类型
- 实现工厂注册表
- 添加工厂缓存
- 实现工厂组合
