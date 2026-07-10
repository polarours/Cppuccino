# 代理模式

一个代理模式实现，用于受控访问。

## 学习目标

- 理解代理模式设计
- 练习延迟加载
- 学习访问控制
- 实现虚拟代理

## 非目标

- 构建生产级代理框架
- 实现复杂的访问控制
- 支持分布式代理

## 功能

- 带 display 的 Image 接口
- RealImage 具体主题
- ImageProxy 延迟加载代理
- 按需对象创建
- 访问日志

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/proxy_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "proxy_pattern.hpp"

auto proxy = std::make_shared<proxy_pattern::ImageProxy>("photo.jpg");
proxy->display();  // 首次调用加载真实图像
proxy->display();  // 使用缓存图像
```

## 建议的下一步

- 添加更多代理类型
- 实现代理缓存
- 添加代理访问控制
- 实现分布式代理
