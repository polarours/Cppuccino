# const 正确性陷阱

## 问题

C++ 中的 const 正确性有微妙的问题，特别是：
- 对临时对象的 const 引用
- const 成员函数和可变状态
- 移除 const

## 示例

```cpp
// const 引用延长临时对象的生命周期
const std::string& ref = std::string("temporary");
// ref 在这里是有效的！临时对象的生命周期与 ref 相同

// 但要小心指针
const char* ptr = std::string("temporary").c_str();
// 危险：ptr 可能指向已销毁的内存！
// c_str() 返回指针，不是引用
```

```cpp
class Cache {
public:
    int get(int key) const {  // const 成员函数
        if (auto it = cache_.find(key); it != cache_.end()) {
            return it->second;  // OK：读取
        }
        int value = compute(key);
        cache_[key] = value;    // 错误：在 const 函数中修改
        return value;
    }
private:
    mutable std::map<int, int> cache_;  // mutable 修复此问题
    int compute(int key) const;
};
```

```cpp
// 移除 const 是危险的
const int x = 42;
int* ptr = const_cast<int*>(&x);
*ptr = 100;  // 未定义行为（x 是 const 的）

// 只在以下情况有效：
// 1. 从 const 函数返回非 const
// 2. 移除原本非 const 对象的 const
```

## 预防措施

- 优先使用 const 引用而非 const 指针
- 对于缓存/记忆化使用 `mutable`
- 从不使用 const_cast 修改真正的 const 对象
- 使用 `-Wcast-qual` 构建以捕获不安全的转换

## 另请参阅

- [隐式转换](implicit-conversions.md)
- [未定义行为](../docs/en/undefined-behavior.md)
