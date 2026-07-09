# HTTP 解析器

一个简单的 HTTP 请求/响应解析器。

## 学习目标

- 理解文本协议解析
- 练习字符串操作和状态机
- 学习 HTTP/1.1 请求和响应格式
- 实现不区分大小写的头部查找

## 非目标

- 构建完整的 HTTP 服务器
- 支持 HTTP/2 或分块编码
- 实现 keep-alive 连接

## 功能

- 解析 HTTP 请求（GET、POST、PUT、DELETE、HEAD）
- 解析请求头和请求体
- 构建带状态码的 HTTP 响应
- 不区分大小写的头部查找
- 响应序列化

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/http_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "http.hpp"

// 解析请求
http::Request req;
req.parse("GET /api/data HTTP/1.1\r\nHost: example.com\r\n\r\n");
std::cout << req.methodName() << " " << req.path << "\n";

// 构建响应
auto resp = http::Response::ok("Hello, World!");
std::cout << resp.serialize();
```

## 建议的下一步

- 添加分块传输编码支持
- 实现请求体解析（表单数据、多部分）
- 添加 HTTP/2 帧解析
- 使用此解析器实现简单的 HTTP 服务器
