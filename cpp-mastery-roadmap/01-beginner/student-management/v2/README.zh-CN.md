# 学生管理系统 v2

学生管理系统的改进版本，展示了常见的代码质量改进。

## 相对 v1 的改进

### 1. 一致的 I/O

v1 混用 `std::scanf` 和 `std::cin`，容易出错。v2 只使用 C++ I/O 流。

### 2. 输入校验

v2 校验用户输入并妥善处理非法数据：

```cpp
int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Invalid input. Please enter a number.\n";
        clearInput();
    }
}
```

### 3. UI 与领域逻辑分离

v1 把 UI 代码和业务逻辑混在一起。v2 分离关注点：

- `Student` - 数据类
- `StudentManager` - 业务逻辑
- `main.cpp` - 仅 UI

### 4. 正确的错误处理

v1 没有错误处理。v2 用 `bool` 或 `std::optional` 表示成功/失败：

```cpp
bool addStudent(Student student);
std::optional<Student> findStudent(int id) const;
```

### 5. 现代 C++ 惯用法

- getter 使用 `const` 引用
- 用 `std::move` 高效传递字符串
- 用 `std::optional` 代替裸指针
- 用 `friend` 实现流运算符

### 6. 文件格式

v1 使用无结构的简单文本格式。v2 包含数量头：

```
2
1
Alice
20
CS
3.8
alice@example.com
1234567
2
Bob
21
Math
3.9
bob@example.com
7654321
```

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/student_management_v2
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 建议的后续步骤

- 为边界情况添加单元测试（负数 ID、空名字等）
- 支持更新单个字段
- 支持按名字或专业搜索
- 添加数据校验（年龄范围、成绩范围、邮箱格式）
