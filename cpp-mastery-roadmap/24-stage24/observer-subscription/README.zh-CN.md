# 观察者订阅

带 RAII 订阅管理的观察者。

handler 可以在事件派发的过程中退订自己：派发开始之前会先把 handler 抄一份，
所以改动订阅列表不会搅乱正在进行的这次派发。

订阅不能比总线活得久：它握着一���回到总线的路，而总线没了这条路也就没了。

## 构建

```bash
cmake -S . -B build && cmake --build build
```

## 测试

```bash
cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build && ctest --test-dir build
```
