#include "demo_object.hpp"

#include <iostream>
#include <vector>
#include <string>

void copy_object_demo() {
    std::cout << "=== Copy Object Demo ===\n";
    
    // Copy construction via initializer list
    DemoObject obj1("obj1");
    
    // Copy construction from lvalue (calls copy ctor)
    DemoObject obj2 = obj1;  // OR: DemoObject obj2(obj1);
    
    // Copy assignment
    DemoObject obj3("obj3");
    obj3 = obj1;  // Calls copy assignment
    
    std::cout << "\n";
}

void move_object_demo() {
    std::cout << "=== Move Object Demo ===\n";
    
    // Move construction from rvalue (calls move ctor)
    DemoObject obj4("obj4");
    DemoObject obj5 = std::move(obj4);
    
    // Move assignment
    DemoObject obj6("obj6");
    obj6 = std::move(obj4);
    
    std::cout << "\n";
}

void pass_by_value_demo() {
    std::cout << "=== Pass by Value Demo ===\n";
    
    auto func = [](DemoObject param) {
        // param is constructed via copy/move depending on argument
    };
    
    DemoObject obj7("obj7");
    func(obj7);           // Copy construct parameter (lvalue)
    func(DemoObject("temp"));  // Direct construction (may elide)
    func(std::move(obj7));    // Move construct parameter (rvalue)
    
    std::cout << "\n";
}

void pass_by_reference_demo() {
    std::cout << "=== Pass by Reference Demo ===\n";
    
    auto func = [](const DemoObject& ref) {
        // No construction - just references existing object
    };
    
    DemoObject obj8("obj8");
    func(obj8);           // Binds to lvalue
    func(DemoObject("temp"));   // Binds to temporary (extends lifetime)
    
    std::cout << "\n";
}

int main() {
    std::cout << "========== Copy & Move Semantics Demo ==========\n\n";
    
    copy_object_demo();
    move_object_demo();
    pass_by_value_demo();
    pass_by_reference_demo();
    
    std::cout << "All objects destroyed at end of main()\n";
    
    return 0;
}
