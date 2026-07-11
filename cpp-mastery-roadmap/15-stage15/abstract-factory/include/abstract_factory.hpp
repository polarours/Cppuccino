#pragma once

#include <string>
#include <memory>
#include <iostream>

namespace abstract_factory {

// --- Abstract Products ---

class Button {
public:
    virtual ~Button() = default;
    virtual void render() = 0;
    virtual std::string getType() const = 0;
};

class Checkbox {
public:
    virtual ~Checkbox() = default;
    virtual void render() = 0;
    virtual std::string getType() const = 0;
};

// --- Concrete Products ---

class WindowsButton : public Button {
public:
    void render() override { std::cout << "[Windows Button]\n"; }
    std::string getType() const override { return "Windows"; }
};

class WindowsCheckbox : public Checkbox {
public:
    void render() override { std::cout << "[Windows Checkbox]\n"; }
    std::string getType() const override { return "Windows"; }
};

class LinuxButton : public Button {
public:
    void render() override { std::cout << "[Linux Button]\n"; }
    std::string getType() const override { return "Linux"; }
};

class LinuxCheckbox : public Checkbox {
public:
    void render() override { std::cout << "[Linux Checkbox]\n"; }
    std::string getType() const override { return "Linux"; }
};

class MacButton : public Button {
public:
    void render() override { std::cout << "[Mac Button]\n"; }
    std::string getType() const override { return "Mac"; }
};

class MacCheckbox : public Checkbox {
public:
    void render() override { std::cout << "[Mac Checkbox]\n"; }
    std::string getType() const override { return "Mac"; }
};

// --- Abstract Factory ---

class GUIFactory {
public:
    virtual ~GUIFactory() = default;
    virtual std::unique_ptr<Button> createButton() = 0;
    virtual std::unique_ptr<Checkbox> createCheckbox() = 0;
};

// --- Concrete Factories ---

class WindowsFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<WindowsButton>();
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<WindowsCheckbox>();
    }
};

class LinuxFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<LinuxButton>();
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<LinuxCheckbox>();
    }
};

class MacFactory : public GUIFactory {
public:
    std::unique_ptr<Button> createButton() override {
        return std::make_unique<MacButton>();
    }

    std::unique_ptr<Checkbox> createCheckbox() override {
        return std::make_unique<MacCheckbox>();
    }
};

} // namespace abstract_factory
