#include "student.hpp"

Student::Student(int id, const std::string& name, int age, const std::string& major, double grade, const std::string& email, const int phoneNumber)
    : id_(id)
    , name_(name)
    , age_(age)
    , major_(major)
    , grade_(grade)
    , email_(email)
    , phoneNumber_(phoneNumber)
    {}

int Student::getID() const {
    return id_;
}

std::string Student::getName() const {
    return name_;
}

int Student::getAge() const {
    return age_;
}

std::string Student::getMajor() const {
    return major_;
}

double Student::getGrade() const {
    return grade_;
}

std::string Student::getEmail() const {
    return email_;
}

int Student::getPhoneNumber() const {
    return phoneNumber_;
}

void Student::setID(int id) {
    id_ = id;
}

void Student::setName(const std::string& name) {
    name_ = name;
}

void Student::setAge(int age) {
    age_ = age;
}

void Student::setMajor(const std::string& major) {
    major_ = major;
}

void Student::setGrade(double grade) {
    grade_ = grade;
}

void Student::setEmail(const std::string& email) {
    email_ = email;
}

void Student::setPhoneNumber(int phoneNumber) {
    phoneNumber_ = phoneNumber;
}

