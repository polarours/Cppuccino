# Delegation Pattern

## What Is the Delegation Pattern?

The delegation pattern forwards "getting something done" to an auxiliary object
(the delegate/helper) that is composed into the host: the host class only
holds, forwards, and combines, while the concrete behavior lives in the
delegate.

> **Core idea**: replace inheritance with composition — extract the "parts that
> vary" into a separate object the host calls through an interface.

## Delegation vs Inheritance

| | Inheritance | Delegation |
|---|---|---|
| Relationship | "is-a" | "has-a" |
| How behavior is shared | Parent's methods | Hold an auxiliary object and call it |
| Runtime replacement | No (vtable is static) | Easy (swap the object via setter) |
| Multiple behavior sources | Hampered by the diamond | Multiple delegates compose naturally |
| Side effects | Subclass tightly coupled to parent | Delegates testable in isolation |

## Core Concepts

- **Delegate**: the concrete behavior carrier implementing some interface
- **Host**: holds a `shared_ptr<Interface>` and forwards requests to the delegate
- **Replaceability**: `setDelegate` changes the host's behavior at runtime

## How It Relates to Strategy

Delegation = forwarding to a single behavior object; Strategy = a family of
algorithms as interchangeable strategies. The structures are nearly identical —
the difference is intent: Strategy emphasizes "choosing among algorithms",
delegation emphasizes "the host does not implement it itself, someone else
does".

## Implementation

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

For the full runnable example, see the
[delegation project](../../cpp-mastery-roadmap/31-stage31/delegation).

## Usage Example

```cpp
#include <cassert>

auto html = std::make_shared<TextRenderer>("<b>OK</b>");
Button ok(html, "OK");
assert(ok.draw() == "[OK: <b>OK</b>]");

// swap the delegate at runtime
ok.setRenderer(std::make_shared<TextRenderer>("✓"));
assert(ok.draw() == "[OK: ✓]");
```

## Best Practices

1. **Keep the delegate interface narrow**: expose only what the host actually
   uses, otherwise the host promises too much on the delegate's behalf
2. **Compose multiple delegates**: hold a `vector<shared_ptr<IDelegate>>` and
   call them in order
3. **Avoid long delegation chains**: `A → B → C → D` is hard to debug; prefer
   composing B/C explicitly in A
4. **Delegates must be independently constructible**: a delegate's constructor
   arguments must not depend on the host

## Example

A runnable version of this topic:

- [delegation-demo.cpp](../../examples/delegation-demo.cpp)

## Summary

Delegation replaces inheritance with composition and is a direct expression of
the open-closed principle — adding behavior means adding a delegate class,
never touching the host. It is a common building block of Bridge, Decorator,
Strategy, and many other patterns.
