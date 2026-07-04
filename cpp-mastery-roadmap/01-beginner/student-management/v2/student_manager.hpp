#pragma once

#include "student.hpp"

#include <vector>
#include <string>
#include <optional>

class StudentManager {
public:
    bool addStudent(Student student);
    bool removeStudent(int id);
    std::optional<Student> findStudent(int id) const;
    std::vector<Student> allStudents() const;
    bool updateStudent(int id, Student updated);

    bool saveToFile(const std::string& filename) const;
    bool loadFromFile(const std::string& filename);

    std::size_t count() const { return students_.size(); }

private:
    std::vector<Student> students_;
};
