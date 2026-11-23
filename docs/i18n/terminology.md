# 术语表 / Terminology

这是 Cppuccino 所有双语内容的**首选译法**。写作或翻译前先查这里；表里的
"禁止写法"由 `tools/check_terminology.py` 自动检查。

"悬空指针 / 迷途指针"在陷阱文章中作为释义和 CVE 引文里的历史别名可以保留，
但不是首选；新内容统一用"悬垂指针"。

| English / 概念 | 推荐中文 | 禁止写法 | 说明 |
|:--|:--|:--|:--|
| lifetime | 生命周期 | 生命期 | 对象从构造到销毁的区间 |
| object lifetime | 对象生命周期 | 对象生命期 | |
| ownership | 所有权 | 拥有权 | |
| RAII | RAII | 资源获取即初始化（首次出现除外） | 首次可写全称，之后保留缩写 |
| dangling pointer | 悬垂指针 | 悬荡指针 | "悬空指针/迷途指针"仅作释义或引文别名 |
| dangling reference | 悬垂引用 | 悬空引用 | 与"悬垂指针"区分：引用不能重新绑定 |
| use-after-free, UAF | 释放后使用（UAF） | 使用已释放内存; 释放后再使用 | |
| undefined behavior, UB | 未定义行为（UB） | 未定义动作 | |
| memory leak | 内存泄漏 | 内存泄露 | |
| double free | 重复释放 | 二次释放 | |
| iterator invalidation | 迭代器失效 | 迭代器无效 | |
| data race | 数据竞争 | 数据竞态条件 | 文章标题「数据竞态」作为固定名称保留；正文首选「数据竞争」 |
| race condition | 竞态条件 | 竞争条件 | |
| move semantics | 移动语义 | 搬移语义 | |
| perfect forwarding | 完美转发 | 完全转发 | |
| value category | 值类别 | 值范畴 | |
| lvalue | 左值 | 左值引用（除非确指引用） | |
| rvalue | 右值 | 右值引用（除非确指引用） | |
| prvalue | 纯右值 | prvalue（正文首次除外） | |
| xvalue | 将亡值 | xvalue（正文首次除外） | |
| copy elision | 拷贝消除 | 复制省略 | |
| object pool | 对象池 | 物件池 | |
| memory pool | 内存池 | 记忆池 | |
| arena allocator | 竞技场分配器 | 舞台分配器 | 首次可附 arena 原文 |
| slab allocator | 板坯分配器 | 板块分配器 | 首次可附 slab 原文 |
| allocation tracker | 分配追踪器 | 分配跟踪器 | |
| signal handler | 信号处理函数 | 信号处理器 | |
| file descriptor | 文件描述符 | 文件句柄 | |
| redirection | 重定向 | 转向 | |
| pipeline | 管道 | 管线化 | shell pipeline 用「管道」；async pipeline 可写「异步管线」 |
| design pattern | 设计模式 | 设计范式 | |
| strategy pattern | 策略模式 | 战略模式 | |
| observer pattern | 观察者模式 | 观察器模式 | |
| command pattern | 命令模式 | 指令模式 | 命令行的 command 仍译"命令" |
| visitor pattern | 访问者模式 | 访问器模式 | |
| factory method | 工厂方法 | 工厂模式（除非确指） | |
| proxy pattern | 代理模式 | 代课模式 | |
| bilingual | 双语 | 双语言 | |
| notebook | 笔记本 | 笔记薄 | 指本项目时保留 Cppuccino notebook 语义 |
| Read mode | 阅读模式 | 读模式 | |
| Play mode | 游戏模式 | 游玩模式 | |
| arcade | 街机 | 电玩城 | 视觉主题名可保留 ARCADE |
| boss fight | Boss 战 | 首领战 | |
| quest | 关卡任务 | 任务（与作业混淆时） | 游戏化数据中保留 quest |
| XP | 经验值 | 经验点数 | 界面缩写保留 XP |
| loot | 掉落物 | 战利品 | 数据 kind 保留 loot |
| intel | 情报 | 情报资料 | 数据 kind 保留 intel |

## 新增术语的流程

1. 先查本表；没有就新增一行，并写清禁止写法。
2. 如果两种译法都在仓库里大量存在，先统计再决定，不要凭感觉改。
3. 修改译法要同步更新使用处，并跑 `python3 tools/check_terminology.py`。
4. 术语争议写进备注，不在翻译时临场决定。
