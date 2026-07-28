#include "../student.hpp"
#include "../student_manager.hpp"

#include <iostream>
#include <stdexcept>
#include <string>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_student_construction() {
    Student s(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567);

    expect(s.getID() == 1, "expected id 1");
    expect(s.getName() == "Alice", "expected name Alice");
    expect(s.getAge() == 20, "expected age 20");
    expect(s.getMajor() == "CS", "expected major CS");
    expect(s.getGrade() == 3.8, "expected grade 3.8");
    expect(s.getEmail() == "alice@example.com", "expected email");
    expect(s.getPhoneNumber() == 1234567, "expected phone");
}

void test_student_setters() {
    Student s(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567);

    s.setName("Bob");
    s.setAge(21);
    s.setMajor("Math");
    s.setGrade(3.9);
    s.setEmail("bob@example.com");
    s.setPhoneNumber(7654321);

    expect(s.getName() == "Bob", "expected name Bob");
    expect(s.getAge() == 21, "expected age 21");
    expect(s.getMajor() == "Math", "expected major Math");
    expect(s.getGrade() == 3.9, "expected grade 3.9");
    expect(s.getEmail() == "bob@example.com", "expected email bob");
    expect(s.getPhoneNumber() == 7654321, "expected phone 7654321");
}

void test_manager_add_student() {
    StudentManager manager;
    Student s(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567);
    manager.addStudent(s);
    expect(manager.getAllStudents().size() == 1, "expected count 1");

    manager.addStudent(s);
    expect(manager.getAllStudents().size() == 2, "expected count 2 after duplicate add");
}

void test_manager_remove_student() {
    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));
    manager.addStudent(Student(2, "Bob", 21, "Math", 3.9, "bob@example.com", 7654321));

    expect(manager.removeStudent(1), "expected remove to succeed");
    expect(manager.getAllStudents().size() == 1, "expected count 1");

    expect(!manager.removeStudent(99), "expected remove nonexistent to fail");
    expect(manager.getAllStudents().size() == 1, "expected count still 1");
}

void test_manager_find_student() {
    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));

    Student* found = manager.getStudent(1);
    expect(found != nullptr, "expected to find student 1");
    expect(found->getName() == "Alice", "expected name Alice");

    Student* notFound = manager.getStudent(99);
    expect(notFound == nullptr, "expected not to find student 99");
}

void test_manager_all_students() {
    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));
    manager.addStudent(Student(2, "Bob", 21, "Math", 3.9, "bob@example.com", 7654321));

    auto all = manager.getAllStudents();
    expect(all.size() == 2, "expected 2 students");
}

void test_manager_save_load() {
    const std::string filename = "/tmp/student_v1_test.txt";

    StudentManager manager;
    manager.addStudent(Student(1, "Alice", 20, "CS", 3.8, "alice@example.com", 1234567));
    manager.addStudent(Student(2, "Bob", 21, "Math", 3.9, "bob@example.com", 7654321));

    manager.saveToFile(filename);

    StudentManager loaded;
    loaded.loadFromFile(filename);
    expect(loaded.getAllStudents().size() == 2, "expected 2 loaded students");

    auto s1 = loaded.getStudent(1);
    expect(s1 != nullptr && s1->getName() == "Alice", "expected to find student 1");

    auto s2 = loaded.getStudent(2);
    expect(s2 != nullptr && s2->getName() == "Bob", "expected to find student 2");
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
        test_manager_save_load();
    } catch (const std::exception& e) {
        std::cerr << "student_management_tests failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "student_management_tests passed" << std::endl;
    return 0;
}
