# 命令备忘录

使用备忘录模式保存状态的命令模式。

空历史上的 `pop()` 什么也没给，`restore()` 拿着这个"没有"就把编辑器原样放着，
而不是把程序带下去。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
