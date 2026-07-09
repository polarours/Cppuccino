# Web 框架

一个简单的 HTTP Web 框架，支持路由和中间件。

## 学习目标

- 理解 HTTP 协议和请求/响应
- 练习套接字编程和并发
- 学习路由和中间件模式
- 实现静态文件服务

## 非目标

- 构建生产级 Web 框架
- 支持 HTTP/2 或 TLS
- 实现复杂的中间件链

## 功能

- HTTP 请求解析
- 路由注册（GET、POST、PUT、DELETE）
- 中间件支持
- 响应构建器（支持链式调用）
- 静态文件服务

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/web_framework_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "web_framework.hpp"

web_framework::App app;

app.get("/", [](const web_framework::Request&) {
    return web_framework::Response::html("<h1>Hello, World!</h1>");
});

app.get("/api/users", [](const web_framework::Request&) {
    return web_framework::Response::json(R"({"users": []})");
});

app.listen(8080);
```

## 建议的下一步

- 添加 URL 参数解析（/users/:id）
- 实现 JSON 请求/响应
- 添加 WebSocket 支持
- 实现模板引擎
