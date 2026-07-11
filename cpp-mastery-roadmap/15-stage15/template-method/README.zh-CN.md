# 模板方法模式

一个模板方法模式实现，用于算法骨架。

## 学习目标

- 理解模板方法模式
- 练习算法骨架
- 学习钩子方法
- 实现代码复用

## 非目标

- 构建生产级数据挖掘框架
- 实现复杂的解析算法
- 支持分布式挖掘

## 功能

- 带模板方法的 DataMiner 抽象类
- CSV、JSON、XML 具体矿工
- 用于自定义的钩子方法
- 一致的算法结构
- 通过继承实现代码复用

## 构建

```bash
cmake -S . -B build
cmake --build build
```

## 运行

```bash
./build/template_method_example
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

## 示例

```cpp
#include "template_method.hpp"

template_method::CSVDataMiner miner;
miner.mine("data.csv");
```

## 建议的下一步

- 添加更多矿工类型
- 实现钩子方法组合
- 添加矿工配置
- 实现矿工流水线
