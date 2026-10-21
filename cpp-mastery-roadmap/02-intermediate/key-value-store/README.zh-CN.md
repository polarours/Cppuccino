# key-value-store

一个用现代 C++ 编写的小型有状态存储练习。

本项目是中级路线图的一部分，旨在以比 HTTP 服务器项目更直接的方式
教授状态所有权、命令处理和持久化边界。

## 学习目标

- 显式表示长生命周期状态
- 保持命令解析与存储行为分离
- 使持久化决策可见而非隐式
- 为格式错误的命令和缺失的键定义简单的失败行为
- 构建一个状态足够丰富、需要测试的小型系统

## 非目标

- 构建生产级数据库
- 支持复杂的查询语言或事务
- 在 v1 中解决并发或分布式一致性问题
- 用过度通用的抽象掩盖存储行为

## 当前功能

- 内存中的 `std::unordered_map<std::string, std::string>` 存储
- 使用读写锁（`shared_mutex`）保证线程安全
- 按行处理的命令处理器，命令动作显式化
- 支持的命令：
  `SET`、`GET`、`DELETE`、`EXISTS`、`COUNT`、`CLEAR`、`SAVE`、`LOAD`、`EXIT`
- 简单的文本持久化，使用带引号的键值对
- 面向结构化请求/响应的文本协议层
- 支持多客户端的 TCP 服务器，可通过网络访问
- 用于本地交互的 CLI 模式
- 覆盖存储行为、命令处理、持久化与协议的原生测试

## 协议

协议层提供结构化的请求/响应格式：

### 请求格式

```
COMMAND [ARG1 [ARG2 ...]]
```

### 响应格式

```
+OK\n                    （成功）
+data\n                  （成功并带数据）
$bulk_string\n           （值响应）
:not_found\n             （键未找到）
-ERROR message\n         （错误）
:count\n                 （整数响应）
```

### 协议命令

| 命令 | 说明 | 响应 |
|------|------|------|
| `SET key value` | 设置键值对 | `+OK` |
| `GET key` | 按键取值 | `$value` 或 `-NOT_FOUND` |
| `DEL key` | 删除一个键 | `+OK` 或 `-NOT_FOUND` |
| `EXISTS key` | 检查键是否存在 | `+1` 或 `+0` |
| `COUNT` | 获取条目数量 | `:count` |
| `CLEAR` | 清空所有条目 | `+OK` |
| `PING` | 测试服务器连通性 | `+pong` |
| `HELP` | 显示帮助信息 | `$help_text` |

### 示例会话

```
SET language C++
+OK
GET language
$C++
EXISTS language
+1
COUNT
:1
DEL language
+OK
GET language
-NOT_FOUND
PING
+pong
```

## 当前限制

- 单进程、单线程设计
- 值以完整字符串的形式加载和保存
- 写命令会自动持久化到当前文件
- `LOAD` 与 `SAVE` 只作用于当前这一个持久化文件路径
- 不支持事务、快照或并发控制

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

### 服务器模式（默认）

```bash
./build/key_value_store
```

带上选项：

```bash
./build/key_value_store -h 0.0.0.0 -p 6379 -f my-store.db
```

### CLI 模式

```bash
./build/key_value_store --cli
./build/key_value_store --cli -f my-store.db
```

### 选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `-h, --host` | 绑定的主机 | `127.0.0.1` |
| `-p, --port` | 监听的端口 | `6379` |
| `-f, --file` | 持久化文件 | `key_value_store.db` |
| `--cli` | 以 CLI 模式运行 | 关闭 |

### 客户端

```bash
# 交互模式
./build/kv_client

# 单条命令
./build/kv_client SET key value
./build/kv_client GET key
./build/kv_client PING

# 连接自定义服务器
./build/kv_client -h 192.168.1.100 -p 6379
```

#### 客户端选项

| 选项 | 说明 | 默认值 |
|------|------|--------|
| `-h, --host` | 服务器主机 | `127.0.0.1` |
| `-p, --port` | 服务器端口 | `6379` |
| `-n, --no-echo` | 关闭响应格式化 | 关闭 |

## 示例会话

```text
SET language C++
GET language
EXISTS language
COUNT
SAVE
EXIT
```

## 测试

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## 建议的下一步

- 认真决定并发应当在何时进入设计
- 为批量操作加入事务支持
- 为键的过期加入 TTL（time-to-live）
- 实现不同的淘汰策略（LRU、LFU）
