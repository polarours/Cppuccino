# 委托模式 (Delegation Pattern)

## 什么是委托模式？

委托模式（Delegation Pattern）把"完成某件事"这件事转发给一个被组合进来的辅助对象（delegate / helper），宿主类只负责持有、转发和组合，而具体行为由辅助对象实现。

> **核心思想**：用组合替代继承，把"行为变化的部分"抽到独立对象里，宿主通过接口调用。

## 与继承的对比

| | 继承 | 委托 |
|---|---|---|
| 关系 | "is-a" | "has-a" |
| 行为共享方式 | 父类方法 | 持有辅助对象并调用 |
| 运行时替换 | 不可（vtable 静态） | 容易（setter 换对象） |
| 多重行为来源 | 受菱形继承困扰 | 自然支持多委托组合 |
| 副作用 | 子类与父类紧耦合 | 委托对象可独立测试 |

## 核心概念

- **委托对象（Delegate）**：实现某接口的具体行为载体
- **宿主对象（Host）**：持有一个 `shared_ptr<Interface>`，转发请求给委托
- **可替换性**：通过 `setDelegate` 可以在运行时改变宿主的行为

## 与 Strategy 的关系

委托模式 = 委派给单个行为对象；Strategy 模式 = 把一族算法作为可互换的策略。两者结构高度相似，区别在于意图：Strategy 强调"在多个算法间选择"，委托强调"宿主自己不实现，交给别人"。

## 代码实现

```cpp
#include <memory>
#include <string>
#include <utility>

class Renderer {
public:
    virtual ~Renderer() = default;
    virtual std::string render() const = 0;
};

class TextRenderer : public Renderer {
public:
    explicit TextRenderer(std::string text) : text_(std::move(text)) {}
    std::string render() const override { return text_; }
private:
    std::string text_;
};

class Widget {
public:
    explicit Widget(std::shared_ptr<Renderer> renderer) : renderer_(std::move(renderer)) {}
    virtual ~Widget() = default;

    virtual std::string draw() const { return renderer_->render(); }
    void setRenderer(std::shared_ptr<Renderer> r) { renderer_ = std::move(r); }

protected:
    std::shared_ptr<Renderer> renderer_;
};

class Button : public Widget {
public:
    Button(std::shared_ptr<Renderer> renderer, std::string label)
        : Widget(std::move(renderer)), label_(std::move(label)) {}

    std::string draw() const override {
        return "[" + label_ + ": " + renderer_->render() + "]";
    }
    const std::string& label() const { return label_; }

private:
    std::string label_;
};

class TextField : public Widget {
public:
    TextField(std::shared_ptr<Renderer> renderer) : Widget(std::move(renderer)) {}
    std::string draw() const override { return "(" + renderer_->render() + ")"; }
};
```

完整可运行示例见 [delegation 项目](../../cpp-mastery-roadmap/31-stage31/delegation)。

## 使用示例

```cpp
auto html = std::make_shared<TextRenderer>("<b>OK</b>");
Button ok(html, "OK");
assert(ok.draw() == "[OK: <b>OK</b>]");

// 运行时换委托
ok.setRenderer(std::make_shared<TextRenderer>("✓"));
assert(ok.draw() == "[OK: ✓]");
```

## 最佳实践

1. **委托接口要窄**：只暴露宿主真正会用到的方法，否则宿主会替委托承担过多承诺
2. **多委托用组合**：宿主持有一组 `vector<shared_ptr<IDelegate>>`，按顺序调用
3. **避免委托链过度**：`A → B → C → D` 难以调试，宁可在 A 显式组合 B/C
4. **委托对象要可独立构造**：不要让委托的构造参数依赖宿主

## 总结

委托模式用组合替代继承，是开放-封闭原则（OCP）的直接体现——新增行为只需新增委托类，宿主无需改动。它是 Bridge、Decorator、Strategy 等多种模式的共同基石。
