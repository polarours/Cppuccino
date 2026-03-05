#include "student_manager.hpp"

#include <iostream>

void printMenu() {
    std::cout << "\n====== Student Management System ======\n";
    std::cout << "1. Add Student\n";
    std::cout << "2. Remove Student\n";
    std::cout << "3. Update Student\n";
    std::cout << "4. Query Student\n";
    std::cout << "5. Display All Students\n";
    std::cout << "6. Save to File\n";
    std::cout << "7. Load from File\n";
    std::cout << "0. Exit\n";
    std::cout << "=====================================\n";
    std::cout << "Enter your choice: ";
}

int main() {
    StudentManager manager;
    std::string fileName = "students.txt";

    int choice;

    while (true) {
        printMenu();
        std::scanf("%d", &choice);
        std::cin.ignore();

        switch (choice) {
        case 1: 
            {
                std::printf("Enter student details:\n");

                int id, age, phoneNumber;
                std::string name, major, email;
                double grade;

                std::printf("Enter ID: ");
                std::scanf("%d", &id);
                std::cin.ignore();

                std::printf("Enter name: ");
                std::getline(std::cin, name);

                std::printf("Enter age: ");
                std::scanf("%d", &age);
                std::cin.ignore();

                std::printf("Enter major: ");
                std::getline(std::cin, major);

                std::printf("Enter grade: ");
                std::scanf("%lf", &grade);
                std::cin.ignore();

                std::printf("Enter email: ");
                std::getline(std::cin, email);

                std::printf("Enter phone number: ");
                std::scanf("%d", &phoneNumber);
                std::cin.ignore();

                manager.addStudent(Student(id, name, age, major, grade, email, phoneNumber));
            }
            break;

        case 2: 
            {
                int id;
                std::printf("Enter student ID to remove: ");
                std::scanf("%d", &id);
                if (manager.removeStudent(id)) {
                    std::printf("Student removed successfully.\n");
                } else {
                    std::printf("Student with ID %d not found.\n", id);
                }
            }
        break;

        case 3: 
            {
                int id;
                std::printf("Enter student ID to update: ");
                std::scanf("%d", &id);
                std::cin.ignore();
                Student* student = manager.getStudent(id);
                if (student) {
                    manager.updateStudent(id);
                } else {
                    std::printf("Student with ID %d not found.\n", id);
                }
            }
        break;

        case 4: 
            {
                int id;
                std::printf("Enter student ID to query: ");
                std::scanf("%d", &id);
                std::cin.ignore();
                Student* student = manager.getStudent(id);
                if (student) {
                    std::printf("ID: %d\n", student->getID());
                    std::printf("Name: %s\n", student->getName().c_str());
                    std::printf("Age: %d\n", student->getAge());
                    std::printf("Major: %s\n", student->getMajor().c_str());
                    std::printf("Grade: %.2f\n", student->getGrade());
                    std::printf("Email: %s\n", student->getEmail().c_str());
                    std::printf("Phone Number: %d\n", student->getPhoneNumber());
                } else {
                    std::printf("Student with ID %d not found.\n", id);
                }
            }
        break;  

        case 5:
            manager.displayAllStudents();
        break;

        case 6:
            manager.saveToFile(fileName);
            std::printf("Students saved to file successfully.\n");
        break;

        case 7:
            manager.loadFromFile(fileName);
            std::printf("Students loaded from file successfully.\n");
        break;

        case 0:
            std::printf("Exiting...\n");
            return 0;

        default:
            std::printf("Invalid choice. Please try again.\n");
        break;
        }
    }

    return 0;
}