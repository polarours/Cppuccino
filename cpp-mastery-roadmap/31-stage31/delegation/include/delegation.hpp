#pragma once

#include <string>
#include <memory>
#include <functional>
#include <iostream>

namespace delegation_pattern {

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

    virtual std::string draw() const {
        return renderer_->render();
    }

    void setRenderer(std::shared_ptr<Renderer> renderer) { renderer_ = std::move(renderer); }

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
    TextField(std::shared_ptr<Renderer> renderer)
        : Widget(std::move(renderer)) {}

    std::string draw() const override {
        return "(" + renderer_->render() + ")";
    }
};

} // namespace delegation_pattern
