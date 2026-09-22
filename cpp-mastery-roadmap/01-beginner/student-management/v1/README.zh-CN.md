# 学生管理系统 v1

学生管理系统的基线版本。这是第一次实现：能跑，但包含了推动 v2、v3 改进的典型问题。

## 功能

- 添加 / 删除 / 查找 / 更新学生
- 保存到文本文件 / 从文件加载
- 交互式菜单驱动的控制台界面

## 已知不足（v2/v3 的动机）

- **混用 I/O**：`std::scanf` 与 `std::cin`/`std::getline` 混用在同一流上，容易出错。
- **没有输入校验**：非法数字输入会破坏流状态。
- **UI 与领域逻辑混杂**：`StudentManager::displayAllStudents()` 和 `updateStudent()` 直接读 `std::cin`。
- **没有错误处理**：`addStudent` 返回 `void`；`getStudent` 返回裸 `Student*`，可能为 null 或悬空。

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/student_management_v1
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 另请参阅

- [v2](../v2/README.zh-CN.md)：输入校验、错误处理、UI 分离
- [v3](../v3/README.zh-CN.md)：`std::string_view` 借用参数
