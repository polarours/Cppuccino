# 适配器模式

一个适配器模式实现，用于接口兼容性。

## 学习目标

- 理解适配器模式设计
- 练习接口兼容性
- 学习类适配器和对象适配器
- 实现遗留系统集成

## 非目标

- 构建生产级适配器框架
- 实现复杂的适配器层次结构
- 支持分布式适配器

## 功能

- MediaPlayer 目标接口
- VlcPlayer 和 Mp3Player 被适配者
- VlcAdapter 和 Mp3Adapter 适配器
- 所有播放器的通用接口
- 透明的适配器行为

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/adapter_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "adapter_pattern.hpp"

auto vlc = std::make_shared<adapter_pattern::VlcPlayer>();
auto adapter = std::make_shared<adapter_pattern::VlcAdapter>(vlc);

// 使用通用 MediaPlayer 接口
adapter->play("test.vlc");
adapter->stop();
```

## 建议的下一步

- 添加更多适配器类型
- 实现双向适配器
- 添加适配器缓存
- 实现适配器链
