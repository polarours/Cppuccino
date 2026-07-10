# 组合模式

一个组合模式实现，用于树形结构。

## 学习目标

- 理解组合模式设计
- 练习树形结构实现
- 学习叶子和组合的统一接口
- 实现递归操作

## 非目标

- 构建生产级文件系统
- 实现复杂的树操作
- 支持分布式组合

## 功能

- FileSystemComponent 接口
- File 叶子类
- Directory 组合类
- 嵌套结构支持
- 大小计算

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/composite_pattern_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "composite_pattern.hpp"

auto root = std::make_shared<composite_pattern::Directory>("root");
root->add(std::make_shared<composite_pattern::File>("file.txt", 100));

std::cout << "Size: " << root->getSize() << "\n";
root->print();
```

## 建议的下一步

- 添加更多组件类型
- 实现树遍历算法
- 添加树操作（搜索、过滤）
- 实现树序列化
