# 类型擦除

手写 std::function 风格包装（AnyAction）与类型擦除的可打印值
（AnyPrintable）：Concept 接口 + 模板 Holder/Model。

## 学习目标

- Concept/Holder 分工：外层虚接口，内层模板实现
- 为什么克隆 holder 就能得到值语义的拷贝
- 多态包装的五法则
- 与虚基类方案对比：接口更小、调试更难

## 非目标

- 小缓冲优化（SBO）
- 无 RTTI 变体
- 完整 std::function 异常保证

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
