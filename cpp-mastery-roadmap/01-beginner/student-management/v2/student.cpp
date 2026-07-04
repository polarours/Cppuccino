#include "student.hpp"

Student::Student(int id, std::string name, int age, std::string major,
                 double grade, std::string email, int phoneNumber)
    : id_(id), name_(std::move(name)), age_(age), major_(std::move(major)),
      grade_(grade), email_(std::move(email)), phoneNumber_(phoneNumber) {}

std::ostream& operator<<(std::ostream& os, const Student& s) {
    os << s.id_ << '\n'
       << s.name_ << '\n'
       << s.age_ << '\n'
       << s.major_ << '\n'
       << s.grade_ << '\n'
       << s.email_ << '\n'
       << s.phoneNumber_;
    return os;
}

std::istream& operator>>(std::istream& is, Student& s) {
    is >> s.id_;
    is.ignore();
    std::getline(is, s.name_);
    is >> s.age_;
    is.ignore();
    std::getline(is, s.major_);
    is >> s.grade_;
    is.ignore();
    std::getline(is, s.email_);
    is >> s.phoneNumber_;
    return is;
}
