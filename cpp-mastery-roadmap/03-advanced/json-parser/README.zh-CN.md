# JSON 解析器

一个简单的 JSON 解析器和序列化器。

## 学习目标

- 理解递归下降解析
- 练习 std::variant 和标签联合体
- 学习 JSON 格式规范
- 实现状态机解析器

## 非目标

- 构建生产级 JSON 解析器
- 支持完整 JSON 规范（注释、尾随逗号）
- 实现 JSON 路径或查询语言

## 功能

- 将 JSON 字符串解析为值树
- 将值序列化回 JSON 字符串
- 支持 null、boolean、number、string、array、object
- 带缩进的美化打印
- 字符串转义处理

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/json_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "json.hpp"

// 解析 JSON
auto value = json::JsonValue::parse(R"({"name": "test", "value": 42})");

// 访问值
std::string name = value.asObject().at("name").asString();
double val = value.asObject().at("value").asNumber();

// 构建 JSON
json::JsonObject obj;
obj["key"] = json::JsonValue("value");
json::JsonValue json(obj);

// 序列化
std::string output = json.stringify(2);  // 美化打印
```

## 建议的下一步

- 添加 JSON 注释支持
- 实现 JSON 路径查询
- 添加模式验证
- 支持流式解析
