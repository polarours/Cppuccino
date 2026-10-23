# 智能指针

自定义实现的 unique_ptr、shared_ptr 和 weak_ptr。

## 学习目标

- 理解引用计数机制
- 练习 RAII 进行内存管理
- 了解智能指针的工作原理
- 实现移动语义和所有权转移

## 非目标

- 构建生产级智能指针
- 支持自定义删除器或分配器

## 功能

- UniquePtr：独占所有权，支持移动语义
- SharedPtr：共享所有权，带引用计数
- WeakPtr：非拥有引用，用于打破循环

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/smart_ptr_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "smart_ptr.hpp"

// 独占所有权
smart_ptr::UniquePtr<int> uptr(new int(42));
auto moved = std::move(uptr);  // uptr 现在为空

// 共享所有权
smart_ptr::SharedPtr<int> sptr(new int(100));
auto copy = sptr;  // 两者共享所有权
// use_count() == 2

// 弱引用
smart_ptr::WeakPtr<int> wptr = sptr;
if (auto locked = wptr.lock()) {
    // 使用锁定的指针
}
```

## 控制块

共享指针和弱指针共用一个控制块，控制块里有两个计数：

- `strong_count` 数的是 `SharedPtr`。
- `weak_count` 数的是 `WeakPtr`，**外加一个**。这多出来的引用，只要强计数
  还没归零就一直被持有。

控制块随最后一个弱引用一起消失，绝不随最后一个强引用一起消失。`WeakPtr`
还得能问"对象还在吗"，所以只要还有 `WeakPtr` 在，控制块就不能没了。

而多出来的那个弱引用，正好把另一个方向也变安全。最后一个 `SharedPtr` 消失
时对象会被销毁，而它的析构函数可能顺手释放自己的弱引用 —— 比如一个指回
自己的节点。控制块必须比那个析构函数活得久，所以这份额外的弱引用要等析构
跑完才放下。

## 建议的下一步

- 添加自定义删除器支持
- 实现 make_unique 和 make_shared
- 添加 weak_ptr::lock() 线程安全
- 添加别名构造函数
