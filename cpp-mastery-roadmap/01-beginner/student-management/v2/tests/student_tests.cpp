#include "student.hpp"
#include "student_manager.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <cstdio>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_student_construction() {
    Student s(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567);

    expect(s.id() == 1, "expected id 1");
    expect(s.name() == "Alice", "expected name Alice");
    expect(s.age() == 20, "expected age 20");
    expect(s.major() == "CS", "expected major CS");
    expect(s.grade() == 3.8, "expected grade 3.8");
    expect(s.email() == "alice@example.com", "expected email");
    expect(s.phoneNumber() == 1234567, "expected phone");
}

void test_student_setters() {
    Student s(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567);

    s.setName("Bob");
    s.setAge(21);
    s.setMajor("Math");
    s.setGrade(3.9);
    s.setEmail("bob@example.com");
    s.setPhoneNumber(7654321);

    expect(s.name() == "Bob", "expected name Bob");
    expect(s.age() == 21, "expected age 21");
    expect(s.major() == "Math", "expected major Math");
    expect(s.grade() == 3.9, "expected grade 3.9");
    expect(s.email() == "bob@example.com", "expected email bob");
    expect(s.phoneNumber() == 7654321, "expected phone 7654321");
}

void test_manager_add_student() {
    StudentManager manager;
    Student s(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567);

    expect(manager.addStudent(s), "expected add to succeed");
    expect(manager.count() == 1, "expected count 1");

    // Duplicate ID should fail
    expect(!manager.addStudent(s), "expected duplicate to fail");
    expect(manager.count() == 1, "expected count still 1");
}

void test_manager_remove_student() {
    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));
    manager.addStudent(Student(2, "Bob", 21, "Math", 3.9, "bob@example.com", 7654321));

    expect(manager.removeStudent(1), "expected remove to succeed");
    expect(manager.count() == 1, "expected count 1");

    expect(!manager.removeStudent(99), "expected remove nonexistent to fail");
    expect(manager.count() == 1, "expected count still 1");
}

void test_manager_find_student() {
    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));

    auto found = manager.findStudent(1);
    expect(found.has_value(), "expected to find student");
    expect(found->name() == "Alice", "expected name Alice");

    auto notFound = manager.findStudent(99);
    expect(!notFound.has_value(), "expected not to find student 99");
}

void test_manager_all_students() {
    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));
    manager.addStudent(Student(2, "Bob", 21, "Math", 3.9, "bob@example.com", 7654321));

    auto all = manager.allStudents();
    expect(all.size() == 2, "expected 2 students");
}

void test_manager_update_student() {
    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));

    Student updated(1, "Alice Smith", 21, "CS", 3.9, "alice.s@example.com", 1111111);
    expect(manager.updateStudent(1, updated), "expected update to succeed");

    auto found = manager.findStudent(1);
    expect(found->name() == "Alice Smith", "expected updated name");
    expect(found->age() == 21, "expected updated age");

    expect(!manager.updateStudent(99, updated), "expected update nonexistent to fail");
}

void test_manager_save_load() {
    const std::string filename = "/tmp/student_test.txt";
    std::remove(filename.c_str());

    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));
    manager.addStudent(Student(2, "Bob", 21, "Math", 3.9, "bob@example.com", 7654321));

    expect(manager.saveToFile(filename), "expected save to succeed");

    StudentManager loaded;
    expect(loaded.loadFromFile(filename), "expected load to succeed");
    expect(loaded.count() == 2, "expected 2 loaded students");

    auto s1 = loaded.findStudent(1);
    expect(s1.has_value(), "expected to find student 1");
    expect(s1->name() == "Alice", "expected name Alice");

    auto s2 = loaded.findStudent(2);
    expect(s2.has_value(), "expected to find student 2");
    expect(s2->name() == "Bob", "expected name Bob");

    std::remove(filename.c_str());
}

void test_manager_save_empty() {
    const std::string filename = "/tmp/student_empty_test.txt";
    std::remove(filename.c_str());

    StudentManager manager;
    expect(manager.saveToFile(filename), "expected save empty to succeed");

    StudentManager loaded;
    expect(loaded.loadFromFile(filename), "expected load empty to succeed");
    expect(loaded.count() == 0, "expected 0 students");

    std::remove(filename.c_str());
}

} // namespace

int main() {
    try {
        test_student_construction();
        test_student_setters();
        test_manager_add_student();
        test_manager_remove_student();
        test_manager_find_student();
        test_manager_all_students();
        test_manager_update_student();
        test_manager_save_load();
        test_manager_save_empty();
    } catch (const std::exception& e) {
        std::cerr << "student_management_tests failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "student_management_tests passed" << std::endl;
    return 0;
}
