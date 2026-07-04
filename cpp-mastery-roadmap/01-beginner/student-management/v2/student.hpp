#pragma once

#include <string>
#include <ostream>
#include <istream>

class Student {
public:
    Student(int id, std::string name, int age, std::string major,
            double grade, std::string email, int phoneNumber);

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    int age() const { return age_; }
    const std::string& major() const { return major_; }
    double grade() const { return grade_; }
    const std::string& email() const { return email_; }
    int phoneNumber() const { return phoneNumber_; }

    void setName(std::string name) { name_ = std::move(name); }
    void setAge(int age) { age_ = age; }
    void setMajor(std::string major) { major_ = std::move(major); }
    void setGrade(double grade) { grade_ = grade; }
    void setEmail(std::string email) { email_ = std::move(email); }
    void setPhoneNumber(int phone) { phoneNumber_ = phone; }

    friend std::ostream& operator<<(std::ostream& os, const Student& s);
    friend std::istream& operator>>(std::istream& is, Student& s);

private:
    int id_;
    std::string name_;
    int age_;
    std::string major_;
    double grade_;
    std::string email_;
    int phoneNumber_;
};
