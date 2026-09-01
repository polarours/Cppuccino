# 内存池与分配器 (Memory Pool & Allocators)

## 什么是内存池？

内存池（Memory Pool）是一种预分配固定大小内存块的机制，避免频繁调用 `new/delete` 带来的开销和碎片。池的分配和释放是 O(1) 的。

> **核心思想**：一次分配一大块（"池"），内部用链表或栈管理空闲块，按需取用。

## 何时需要内存池

- 高频分配/释放同类对象（网络包、游戏实体、树节点）
- `new/delete` 成为性能瓶颈
- 需要保证内存分配实时性（不允许系统调用延迟）

## 三种分配器对比

| | Memory Pool | Arena (Bump Allocator) | Slab Allocator |
|---|---|---|---|
| 块大小 | 固定 | 固定 | 固定 |
| 释放粒度 | 单块归还 | 只能全reset | 单块归还 |
| 碎片 | 无 | 无 | 极低 |
| 适用场景 | 同类对象大量创建/销毁 | 短生命周期批次处理 | 多种尺寸规格 |

## 代码实现：Memory Pool

```cpp
#include <cstddef>
#include <vector>
#include <cstdint>

class MemoryPool {
public:
    explicit MemoryPool(std::size_t blockSize, std::size_t blockCount)
        : blockSize_(blockSize) {
        memory_.resize(blockCount * blockSize);
        for (std::size_t i = 0; i < blockCount; ++i) {
            freeList_.push_back(&memory_[i * blockSize]);
        }
    }

    void* allocate() {
        if (freeList_.empty()) return nullptr;
        void* ptr = freeList_.back();
        freeList_.pop_back();
        return ptr;
    }

    void deallocate(void* ptr) {
        if (!ptr) return;
        freeList_.push_back(ptr);  // 简单归还链表
    }

    std::size_t freeBlocks() const { return freeList_.size(); }
    std::size_t usedBlocks() const {
        return (memory_.size() / blockSize_) - freeList_.size();
    }

private:
    std::size_t blockSize_;
    std::vector<std::byte> memory_;
    std::vector<void*> freeList_;   // 空闲块链表（用 vector 模拟）
};
```

## 代码实现：Arena（Bump Allocator）

```cpp
#include <cstddef>
#include <vector>
#include <cstdint>

class Arena {
public:
    explicit Arena(std::size_t size) : memory_(size), offset_(0) {}

    void* allocate(std::size_t size, std::size_t alignment = alignof(std::max_align_t)) {
        std::size_t aligned = (offset_ + alignment - 1) & ~(alignment - 1);
        if (aligned + size > memory_.size()) return nullptr;
        void* ptr = &memory_[aligned];
        offset_ = aligned + size;
        return ptr;
    }

    void reset() { offset_ = 0; }  // 一次性释放全部——无需逐块追踪

    std::size_t used() const { return offset_; }

private:
    std::vector<std::byte> memory_;
    std::size_t offset_;   // bump 指针
};
```

## 代码实现：Slab Allocator

```cpp
#include <cstddef>
#include <vector>
#include <map>

class SlabAllocator {
public:
    explicit SlabAllocator(std::size_t slabSize = 4096) : slabSize_(slabSize) {
        currentSlab_.resize(slabSize);
    }

    void* allocate(std::size_t size) {
        std::size_t blockSize = nextPowerOf2(size);
        if (blockSize < 16) blockSize = 16;

        auto& freeList = freeLists_[blockSize];
        if (!freeList.empty()) {
            void* ptr = freeList.back();
            freeList.pop_back();
            return ptr;
        }

        if (slabOffset_ + blockSize > slabSize_) {
            slabs_.push_back(std::move(currentSlab_));
            currentSlab_.resize(slabSize_);
            slabOffset_ = 0;
        }

        void* ptr = &currentSlab_[slabOffset_];
        slabOffset_ += blockSize;
        return ptr;
    }

    void reset() {
        freeLists_.clear();
        slabs_.clear();
        currentSlab_.resize(slabSize_);
        slabOffset_ = 0;
    }

private:
    static std::size_t nextPowerOf2(std::size_t n) {
        std::size_t p = 1;
        while (p < n) p <<= 1;
        return p;
    }

    std::size_t slabSize_;
    std::size_t slabOffset_ = 0;
    std::vector<std::byte> currentSlab_;
    std::vector<std::vector<std::byte>> slabs_;
    std::map<std::size_t, std::vector<void*>> freeLists_;
};
```

完整可运行示例见 [memory-pool 项目](../../cpp-mastery-roadmap/35-stage35/memory-pool)、[arena-allocator 项目](../../cpp-mastery-roadmap/35-stage35/arena-allocator)、[slab-allocator 项目](../../cpp-mastery-roadmap/35-stage35/slab-allocator)。

## 最佳实践

1. **Pool 只管理同类对象**：不同尺寸/生命周期的对象不要混用同一个池
2. **Arena 的 reset 必须在正确时机**：所有使用该 Arena 分配的对象都失效后才能 reset——通常配合批次处理或单帧
3. **分配结果要做空检查**：三个分配器在耗尽时均返回 `nullptr`（Pool）或 `nullptr`（Arena），调用方必须处理
4. **对齐是隐式约束**：`Arena::allocate` 接受对齐参数，默认 `alignof(std::max_align_t)`；小于自然对齐的请求按 8 或 16 字节对齐

## 与标准分配器的对比

| | new/delete | std::allocator | 内存池 |
|---|---|---|---|
| 分配速度 | 慢（系统调用） | 中 | 快（O(1) 链表） |
| 碎片 | 有 | 中 | 无 |
| 适用规模 | 少量/一次性 | 标准容器 | 大量同类对象 |

## 总结

内存池通过预分配+内部管理，把分配成本从"系统调用"降为"指针弹栈"，是游戏引擎、网络服务器、高频交易等场景的标配基础设施。Arena 的 bump-only 模式尤其适合"用完即弃"的批次处理。
