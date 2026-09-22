# 学生管理系统 v3

学生管理系统的改进版本，展示通过 `std::string_view` 参数实现的**借用所有权模型**。

## 相对 v2 的改进

### 1. `std::string_view` 借用参数

v2 按值传递字符串参数，再 move 进对象。v3 用 `std::string_view` 借用输入，内部自己做拷贝：

```cpp
// v2: 按值传递，再 move
Student(int id, std::string name, int age, std::string major,
        double grade, std::string email, int phoneNumber);

// v3: 边界处借用，内部持有
Student(int id, std::string_view name, int age, std::string_view major,
        double grade, std::string_view email, int phoneNumber);
```

为什么重要：

- **调用侧**：热路径上省掉临时 `std::string` 的构造——签名明确表达了"借用"。
- **持有侧**：`Student` 仍然持有自己的 `std::string` 成员。对象从不持有 view，没有悬空风险。
- **API 边界**：类在签名层面明确了所有权模型——边界处借用，内部持有。

### 2. Setter 也借用

```cpp
void setName(std::string_view name) { name_ = name; }  // 从 view 拷贝
```

成员仍是 `std::string`（持有）；setter 避免了仅为 move 而按值接收 `std::string`。

### 3. v2 的内容全部保留

v2 的输入校验、UI 分离、`bool`/`std::optional` 错误处理和带数量头的文件格式均不变。

## 构建

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
```

## 运行

```bash
./build/student_management_v3
```

## 测试

```bash
ctest --test-dir build --output-on-failure
```

## 另请参阅

- [v1](../v1/README.zh-CN.md)：有问题基线版
- [v2](../v2/README.zh-CN.md)：校验、错误处理、UI 分离
