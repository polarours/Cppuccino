#include "student_manager.hpp"

#include <iostream>
#include <fstream>
#include <cstdio>
#include <sstream>

void StudentManager::addStudent(const Student& student) {
    students_.push_back(student);
}

bool StudentManager::removeStudent(int id) {
    for (auto it  = students_.begin(); it != students_.end(); ++it) {
        if (it->getID() == id) {
            students_.erase(it);
            return true;
        }
    }
    return false;
}

Student* StudentManager::getStudent(int id) {
    for (auto& student : students_) {
        if (student.getID() == id) {
            return &student;
        }
    }
    return nullptr;
}

std::vector<Student> StudentManager::getAllStudents() const {
    return students_;
}

void StudentManager::displayAllStudents() const {
    for (const auto& student : students_) {
        std::printf("ID: %d, Name: %s, Age: %d, Major: %s, Grade: %.2f, Email: %s, Phone Number: %d\n",
            student.getID(), student.getName().c_str(), student.getAge(), student.getMajor().c_str(),
            student.getGrade(), student.getEmail().c_str(), student.getPhoneNumber());
    }
}

void StudentManager::updateStudent(int id) {
    Student* student = getStudent(id);
    if (student) {
        std::printf("Which field do you want to update? (1: Name, 2: Age, 3: Major, 4: Grade, 5: Email, 6: Phone Number): ");
        int choice;
        std::scanf("%d", &choice);
        std::cin.ignore();
        switch (choice) {
            case 1: {
                std::string name;
                std::printf("Enter new name: ");
                std::getline(std::cin, name);
                student->setName(name);
                break;
            }
            case 2: {
                int age;
                std::printf("Enter new age: ");
                std::scanf("%d", &age);
                student->setAge(age);
                break;
            }
            case 3: {
                std::string major;
                std::printf("Enter new major: ");
                std::getline(std::cin, major);
                student->setMajor(major);
                break;
            }
            case 4: {
                double grade;
                std::printf("Enter new grade: ");
                std::scanf("%lf", &grade);
                student->setGrade(grade);
                break;
            }
            case 5: {
                std::string email;
                std::printf("Enter new email: ");
                std::getline(std::cin, email);
                student->setEmail(email);
                break;
            }
            case 6: {
                int phoneNumber;
                std::printf("Enter new phone number: ");
                std::scanf("%d", &phoneNumber);
                student->setPhoneNumber(phoneNumber);
                break;
            }
            default:
                std::printf("Invalid choice.\n");
        }
    }else {
        std::printf("Student with ID %d not found.\n", id);
    }
}

void StudentManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::printf("Could not open file %s for reading.\n", filename.c_str());
        return;
    }

    students_.clear();
    
    int id, age, phoneNumber;
    std::string name, major, email;
    double grade;

    while (file >> id >> name >> age >> major >> grade >> email >> phoneNumber) {
        students_.emplace_back(id, name, age, major, grade, email, phoneNumber);
    }

    file.close();
}

void StudentManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::printf("Could not open file %s for writing.\n", filename.c_str());
        return; 
    }

    for (const auto& student : students_) {
        file << student.getID() << " " << student.getName() << " " << student.getAge() << " "
             << student.getMajor() << " " << student.getGrade() << " " << student.getEmail() << " "
             << student.getPhoneNumber() << "\n";
    }

    file.close();
}