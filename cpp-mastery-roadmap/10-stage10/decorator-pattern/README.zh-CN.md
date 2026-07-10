# 装饰器模式

一个装饰器模式实现，用于动态添加行为。

## 学习目标

- 理解装饰器模式设计
- 练习动态行为组合
- 学习接口保持
- 实现分层行为

## 非目标

- 构建生产级装饰器框架
- 实现复杂的装饰器链
- 支持分布式装饰器

## 功能

- DataSource 组件接口
- 文件数据源具体组件
- 加密、压缩、日志装饰器
- 可组合的装饰器
- 透明的行为添加

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/decorator_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "decorator_pattern.hpp"

auto source = std::make_shared<decorator_pattern::FileDataSource>("data.txt");
auto encrypted = std::make_shared<decorator_pattern::EncryptionDecorator>(source);
auto compressed = std::make_shared<decorator_pattern::CompressionDecorator>(encrypted);

// 带加密和压缩读取
std::string data = compressed->readData();
```

## 建议的下一步

- 添加更多装饰器类型
- 实现装饰器链的性能优化
- 添加装饰器缓存
- 实现装饰器序列化
