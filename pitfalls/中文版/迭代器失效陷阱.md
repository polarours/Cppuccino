# 迭代器失效陷阱

## 问题描述

迭代器失效是 C++ 中最常见的陷阱之一。当容器被修改时，已经获取的迭代器可能变得无效，继续使用该迭代器会导致未定义行为。

## 常见场景

### 1. vector 迭代器失效

```cpp
#include <vector>
#include <iostream>

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    
    auto it = v.begin();
    
    // 危险：insert 可能导致重新分配
    v.insert(v.begin(), 0);
    
    // 未定义行为：it 可能已失效
    std::cout << *it << "\n";  // BUG!
    
    return 0;
}
```

**原因**：`vector::insert` 在空间不足时会重新分配内存，所有迭代器、指针和引用都会失效。

### 2. map/set 迭代器失效

```cpp
#include <map>
#include <iostream>

int main() {
    std::map<int, std::string> m = {{1, "one"}, {2, "two"}};
    
    auto it = m.begin();
    
    // 危险：erase 后 it 失效
    m.erase(it++);  // 正确：先递增再使用
    
    // 危险：直接 erase
    it = m.begin();
    m.erase(it);
    std::cout << *it << "\n";  // BUG! it 已失效
}
```

**原因**：`std::map::erase(it)` 会使 `it` 失效，但返回的是下一个有效迭代器（C++11 起）。

## 安全做法

### vector 安全操作

```cpp
#include <vector>

std::vector<int> v = {1, 2, 3};

// 方式1：使用索引
for (size_t i = 0; i < v.size(); ++i) {
    // 安全使用 v[i]
}

// 方式2：erase 时更新迭代器
for (auto it = v.begin(); it != v.end(); ) {
    if (shouldErase(*it)) {
        it = v.erase(it);  // erase 返回下一个有效迭代器
    } else {
        ++it;
    }
}

// 方式3：erase-remove 惯用法
v.erase(std::remove_if(v.begin(), v.end(), pred), v.end());
```

### map/set 安全操作

```cpp
#include <map>

std::map<int, std::string> m = {{1, "one"}, {2, "two"}};

// erase 时更新迭代器
for (auto it = m.begin(); it != m.end(); ) {
    if (shouldErase(it->first)) {
        it = m.erase(it);  // C++11 起有效
    } else {
        ++it;
    }
}
```

## 容器行为总结

| 操作 | vector | string | deque | list | forward_list | map/set | unordered_map/set |
|------|--------|--------|-------|------|-------------|---------|-------------------|
| insert (非尾部) | 全部失效 | 全部失效 | 全部失效 | 不变 | 不变 | 不变 | 不变 |
| insert (尾部) | 全部失效 | 全部失效 | 不变 | 不变 | 不变 | 不变 | 不变 |
| erase (指定位置) | 该位置后失效 | 该位置后失效 | 该位置后失效 | 不变 | 不变 | 返回下一个 | 返回下一个 |
| clear | 全部失效 | 全部失效 | 全部失效 | 不变 | 不变 | 不变 | 不变 |
| resize | 全部失效 | 全部失效 | 不变 | 不变 | 不变 | 不变 | 不变 |

## 最佳实践

1. **避免在循环中修改容器大小**：先收集要删除的元素，再统一删除
2. **使用 erase-remove 惯用法**：`v.erase(std::remove(...), v.end())`
3. **erase 时记住返回值**：`it = container.erase(it)`
4. **优先使用索引遍历**：对于 vector，索引比迭代器更安全
5. **理解容器的迭代器失效规则**：不同容器的行为不同

## 检测工具

- **AddressSanitizer**: `-fsanitize=address`
- **Valgrind**: `valgrind --tool=memcheck ./program`

## 总结

> **迭代器失效是 C++ 中最隐蔽的 Bug 来源之一。始终理解容器的迭代器失效规则，并在修改容器后重新获取迭代器。**
