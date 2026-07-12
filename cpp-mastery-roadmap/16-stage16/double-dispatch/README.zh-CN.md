# 双重分发模式

一个双重分发模式实现，用于多方法多态。

## 学习目标

- 理解双重分发模式
- 练习多方法多态
- 学习访问者模式扩展
- 实现游戏逻辑

## 非目标

- 构建生产级游戏引擎
- 实现复杂的游戏规则
- 支持分布式游戏

## 功能

- 带 accept 的 Element 接口
- Rock、Paper、Scissors 元素
- Game 访问者用于结果
- 双重分发函数
- 游戏逻辑

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/double_dispatch_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "double_dispatch.hpp"

double_dispatch::Rock rock;
double_dispatch::Paper paper;

std::string result = double_dispatch::playGame(rock, paper);
```

## 建议的下一步

- 添加更多游戏元素
- 实现得分系统
- 添加游戏历史
- 实现网络对战
