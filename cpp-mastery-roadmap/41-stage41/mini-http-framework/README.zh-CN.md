# Mini HTTP 框架

将多种设计模式组合到一个实际 HTTP 框架中的综合实战项目。

## 使用的设计模式

| 模式 | 位置 | 用途 |
|------|------|------|
| **中间件管道** | `middleware.hpp` | 链式请求处理 |
| **策略模式** | `router.hpp` | 路由匹配 |
| **观察者模式** | `event_bus.hpp` | 请求/响应事件 |
| **工厂模式** | `Request::parse()` | 对象创建 |
| **建造者模式** | `Response::ok().header().body()` | 流式响应构建 |
| **类型擦除** | `handler.hpp` | 多态处理器 |
| **外观模式** | `app.hpp` | 简化接口 |
| **单例模式** | `App::instance()` | 全局应用状态 |
| **装饰器模式** | 中间件包装 | 添加行为 |

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行示例

```bash
./build/mini_http_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```
