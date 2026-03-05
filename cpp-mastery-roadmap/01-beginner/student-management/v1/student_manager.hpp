# pragma once

#include "student.hpp"
#include <vector>

class StudentManager {
    public:
        void addStudent(const Student& student);
        bool removeStudent(int id);
        Student* getStudent(int id);
        std::vector<Student> getAllStudents() const;
        void displayAllStudents() const;
        void updateStudent(int id);

        void loadFromFile(const std::string& filename);
        void saveToFile(const std::string& filename) const;

    private:
        std::vector<Student> students_;
};