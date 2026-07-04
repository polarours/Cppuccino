#include "student_manager.hpp"

#include <iostream>
#include <string>
#include <limits>

namespace {

void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

int readInt(const std::string& prompt) {
    int value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Invalid input. Please enter a number.\n";
        clearInput();
    }
}

double readDouble(const std::string& prompt) {
    double value;
    while (true) {
        std::cout << prompt;
        if (std::cin >> value) {
            clearInput();
            return value;
        }
        std::cout << "Invalid input. Please enter a number.\n";
        clearInput();
    }
}

std::string readLine(const std::string& prompt) {
    std::string value;
    std::cout << prompt;
    std::getline(std::cin, value);
    return value;
}

void printMenu() {
    std::cout << "\n=== Student Management System ===\n"
              << "1. Add Student\n"
              << "2. Remove Student\n"
              << "3. Find Student\n"
              << "4. Display All\n"
              << "5. Save to File\n"
              << "6. Load from File\n"
              << "0. Exit\n"
              << "================================\n";
}

void printStudent(const Student& s) {
    std::cout << "  ID: " << s.id() << "\n"
              << "  Name: " << s.name() << "\n"
              << "  Age: " << s.age() << "\n"
              << "  Major: " << s.major() << "\n"
              << "  Grade: " << s.grade() << "\n"
              << "  Email: " << s.email() << "\n"
              << "  Phone: " << s.phoneNumber() << "\n";
}

void handleAdd(StudentManager& manager) {
    std::cout << "--- Add Student ---\n";
    int id = readInt("ID: ");
    std::string name = readLine("Name: ");
    int age = readInt("Age: ");
    std::string major = readLine("Major: ");
    double grade = readDouble("Grade: ");
    std::string email = readLine("Email: ");
    int phone = readInt("Phone: ");

    Student s(id, std::move(name), age, std::move(major), grade, std::move(email), phone);
    if (manager.addStudent(std::move(s))) {
        std::cout << "Student added.\n";
    } else {
        std::cout << "Error: Student with ID " << id << " already exists.\n";
    }
}

void handleRemove(StudentManager& manager) {
    int id = readInt("Student ID to remove: ");
    if (manager.removeStudent(id)) {
        std::cout << "Student removed.\n";
    } else {
        std::cout << "Student with ID " << id << " not found.\n";
    }
}

void handleFind(const StudentManager& manager) {
    int id = readInt("Student ID to find: ");
    auto student = manager.findStudent(id);
    if (student) {
        printStudent(*student);
    } else {
        std::cout << "Student with ID " << id << " not found.\n";
    }
}

void handleDisplayAll(const StudentManager& manager) {
    auto students = manager.allStudents();
    if (students.empty()) {
        std::cout << "No students.\n";
        return;
    }
    std::cout << "--- All Students (" << students.size() << ") ---\n";
    for (const auto& s : students) {
        printStudent(s);
        std::cout << "\n";
    }
}

void handleSave(const StudentManager& manager) {
    std::string filename = readLine("Filename: ");
    if (manager.saveToFile(filename)) {
        std::cout << "Saved " << manager.count() << " students.\n";
    } else {
        std::cout << "Error: Could not save to file.\n";
    }
}

void handleLoad(StudentManager& manager) {
    std::string filename = readLine("Filename: ");
    if (manager.loadFromFile(filename)) {
        std::cout << "Loaded " << manager.count() << " students.\n";
    } else {
        std::cout << "Error: Could not load from file.\n";
    }
}

} // namespace

int main() {
    StudentManager manager;

    // Load default file if it exists
    manager.loadFromFile("students.txt");

    while (true) {
        printMenu();
        int choice = readInt("Choice: ");

        switch (choice) {
        case 1: handleAdd(manager); break;
        case 2: handleRemove(manager); break;
        case 3: handleFind(manager); break;
        case 4: handleDisplayAll(manager); break;
        case 5: handleSave(manager); break;
        case 6: handleLoad(manager); break;
        case 0:
            manager.saveToFile("students.txt");
            std::cout << "Goodbye.\n";
            return 0;
        default:
            std::cout << "Invalid choice.\n";
        }
    }
}
