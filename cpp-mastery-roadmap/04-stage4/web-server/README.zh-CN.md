# Web 服务器

一个支持路由和中间件的 HTTP 服务器。

## 学习目标

- 理解 HTTP 协议和请求/响应
- 练习套接字编程和并发
- 学习路由和中间件模式
- 实现静态文件服务

## 非目标

- 构建生产级 Web 服务器
- 支持 HTTP/2 或 TLS
- 实现复杂的中间件链

## 功能

- HTTP 请求解析
- 路由注册（GET、POST、PUT、DELETE）
- 中间件支持
- 静态文件服务
- 基于线程的并发

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/web_server_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "web_server.hpp"

web_server::WebServer server(8080);

server.get("/", [](const web_server::Request&) {
    return web_server::Response::ok("Hello, World!");
});

server.get("/users/:id", [](const web_server::Request& req) {
    return web_server::Response::ok("User " + req.path);
});

server.start();
```

## 建议的下一步

- 添加 URL 参数解析（/users/:id）
- 实现 JSON 请求/响应
- 添加 WebSocket 支持
- 实现连接池
