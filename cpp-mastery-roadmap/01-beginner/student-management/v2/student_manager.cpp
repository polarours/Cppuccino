#include "student_manager.hpp"

#include <fstream>
#include <algorithm>

bool StudentManager::addStudent(Student student) {
    for (const auto& s : students_) {
        if (s.id() == student.id()) {
            return false;
        }
    }
    students_.push_back(std::move(student));
    return true;
}

bool StudentManager::removeStudent(int id) {
    auto it = std::remove_if(students_.begin(), students_.end(),
                             [id](const Student& s) { return s.id() == id; });
    if (it == students_.end()) {
        return false;
    }
    students_.erase(it, students_.end());
    return true;
}

std::optional<Student> StudentManager::findStudent(int id) const {
    for (const auto& s : students_) {
        if (s.id() == id) {
            return s;
        }
    }
    return std::nullopt;
}

std::vector<Student> StudentManager::allStudents() const {
    return students_;
}

bool StudentManager::updateStudent(int id, Student updated) {
    for (auto& s : students_) {
        if (s.id() == id) {
            s = std::move(updated);
            return true;
        }
    }
    return false;
}

bool StudentManager::saveToFile(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    file << students_.size() << '\n';
    for (const auto& s : students_) {
        file << s << '\n';
    }
    return true;
}

bool StudentManager::loadFromFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    std::size_t count;
    file >> count;
    file.ignore();

    students_.clear();
    for (std::size_t i = 0; i < count; ++i) {
        Student s(0, "", 0, "", 0.0, "", 0);
        file >> s;
        if (file.good()) {
            students_.push_back(std::move(s));
        }
    }
    return true;
}
