# tiny-http-server

一个用现代 C++ 编写的小型 HTTP 服务器。

本项目是 **Cppuccino** 学习路线图的一部分，旨在展示套接字处理、
请求解析、路由和响应构建如何在最小服务器中组合在一起。

## 学习目标

- 理解套接字资源如何在小型服务器中流转
- 看到请求解析、路由和响应构建应保持分离
- 跨模块练习显式所有权和生命周期边界
- 学习简单的固定大小线程池模型，不引入过多机制
- 阅读足够小可以重构、但足够大有真实结构的代码

## 非目标

- 实现完整的 HTTP 服务器
- 覆盖 HTTP/1.1 keep-alive、分块传输或 TLS
- 构建生产级并发模型
- 在边界稳定之前就追求吞吐量

## 当前功能

- IPv4 上的 TCP 套接字服务器
- 固定大小线程池处理接受的连接
- 最小化的 HTTP 请求行与头部解析
- 简单的路由，支持 `GET /` 与 `GET /hello`
- HTTP 响应构建器，含状态行、头部与响应体
- 基础错误响应：
  `400 Bad Request`、`404 Not Found`、`405 Method Not Allowed`

## 当前限制

- 用固定缓冲区单次读取请求
- 采用简单的"一连接一任务"模型
- 只支持少量硬编码路由
- 尚不支持优雅关闭与 keep-alive 连接

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/tiny_http_server
```

服务器监听 `0.0.0.0:8082`。
当前的 `main` 函数以 `4` 个工作线程启动它。

## 测试

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## 试一试

```bash
curl http://127.0.0.1:8082/
curl http://127.0.0.1:8082/hello
curl http://127.0.0.1:8082/missing
curl -X POST http://127.0.0.1:8082/hello
```

## 建议的下一步

- 把硬编码路由换成处理函数注册
- 为文件描述符加上 RAII 包装
- 改进请求解析与部分读取的处理
- 为服务器级行为与关闭路径补测试
