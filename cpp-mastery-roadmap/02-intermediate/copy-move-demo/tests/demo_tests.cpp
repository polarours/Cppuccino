#include "../include/demo_object.hpp"

#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_construction() {
    DemoObject obj("test");
    expect(obj.getId() == "test", "expected correct ID");
}

void test_copy_ctor() {
    DemoObject obj1("obj1");
    DemoObject obj2 = obj1;  // Should invoke copy ctor
    expect(obj2.getId().find("_copy") != std::string::npos, "copy ctor should append _copy");
}

void test_move_ctor() {
    DemoObject obj1("obj1");
    DemoObject obj2 = std::move(obj1);  // Should invoke move ctor
    expect(obj2.getId().find("_move") != std::string::npos, "move ctor should append _move");
}

void test_copy_assign() {
    DemoObject obj1("obj1");
    DemoObject obj2("obj2");
    obj2 = obj1;  // Should invoke copy assign
    expect(obj2.getId().find("_copy") != std::string::npos, "copy assign should append _copy");
}

void test_move_assign() {
    DemoObject obj1("obj1");
    DemoObject obj2("obj2");
    obj2 = std::move(obj1);  // Should invoke move assign
    expect(obj2.getId().find("_move") != std::string::npos, "move assign should append _move");
}

void test_swap() {
    DemoObject obj1("obj1");
    DemoObject obj2("obj2");
    using std::swap;
    swap(obj1, obj2);
    // After swap, obj1 should have "obj2" and obj2 should have "obj1"
    // (swap exchanges the private id_ members)
    expect(obj1.getId() == "obj2", "swap should exchange IDs");
    expect(obj2.getId() == "obj1", "swap should exchange IDs");
}

} // namespace

int main() {
    try {
        test_construction();
        test_copy_ctor();
        test_move_ctor();
        test_copy_assign();
        test_move_assign();
        test_swap();
    } catch (const std::exception& e) {
        std::cerr << "copy_move_tests failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "copy_move_tests passed" << std::endl;
    return 0;
}
