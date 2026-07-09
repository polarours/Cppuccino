# 内存分配器

一个简单的固定块内存分配器，带有 STL 分配器接口。

## 学习目标

- 理解底层内存分配
- 练习 RAII 进行内存管理
- 了解 STL 分配器的工作原理
- 实现空闲链表进行内存复用

## 非目标

- 构建生产级分配器
- 支持可变大小分配
- 实现垃圾回收

## 功能

- 固定块内存池
- O(1) 分配和释放
- 空闲链表复用
- STL 兼容分配器接口
- 内存所有权跟踪

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/allocator_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "simple_allocator.hpp"

// 创建分配器：64字节块，10个块
simple_allocator::SimpleAllocator allocator(64, 10);

// 分配和使用
void* ptr = allocator.allocate(32);
// ... 使用 ptr ...
allocator.deallocate(ptr);

// 与 STL 容器一起使用
simple_allocator::StlAllocator<int> alloc(allocator);
std::vector<int, simple_allocator::StlAllocator<int>> vec(alloc);
vec.push_back(42);
```

## 建议的下一步

- 添加可变大小分配支持
- 实现伙伴分配器算法
- 添加内存对齐支持
- 添加分配统计和调试
