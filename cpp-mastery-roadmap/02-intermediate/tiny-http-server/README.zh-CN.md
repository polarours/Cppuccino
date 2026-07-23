# tiny-http-server

一个用现代 C++ 编写的小型 HTTP 服务器。

本项目是 **Cppuccino** 学习路线图的一部分，旨在展示套接字处理、
请求解析、路由和响应构建如何在最小服务器中组合在一起。

## 学习目标

- 理解套接字资源如何在小型服务器中流转
- 看到请求解析、路由和响应构建应保持分离
- 跨模块练习显式所有权和生命周期边界
- 学习简单的固定大小线程池模型
- 阅读足够小可以重构但足够大有真实结构的代码

## 非目标

- 实现完整的 HTTP 服务器
- 覆盖 HTTP/1.1 keep-alive、分块传输或 TLS

## 功能

- **HTTP 解析**: 支持 GET 方法
- **路由**: 动态路由注册
- **中间件**: 请求前处理
- **线程池**: 固定大小线程池
- **静态文件**: 路径前缀匹配

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/http_server
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
