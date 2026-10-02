# 自定义错误类别

领域错误枚举通过 `is_error_code_enum` 接入 `std::error_code`，
并用 `make_error_condition` 映射到标准 condition（timed_out、
permission_denied 等）。

## 学习目标

- 三件套：枚举 + 类别单例 + is_error_code_enum 特化
- error_code vs error_condition：跨类别比较的语义
- 为什么 message() 必须处理未知码（前向兼容）
- 与 std::system_error / std::expected<T, error_code> 组合

## 非目标

- 层级化类别
- POSIX errno 镜像
- 全异常式 API

## 构建 / 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
