#pragma once

#include <string>
#include <string_view>  // Added: non-owning view for parameters
#include <ostream>
#include <istream>

class Student {
public:
    // Constructor using string_view for non-owning parameters
    // This demonstrates the "borrowing" ownership model - parameters are borrowed,
    // and the class makes its own owned copy internally.
    Student(int id, std::string_view name, int age, std::string_view major,
            double grade, std::string_view email, int phoneNumber);

    int id() const { return id_; }
    const std::string& name() const { return name_; }
    int age() const { return age_; }
    const std::string& major() const { return major_; }
    double grade() const { return grade_; }
    const std::string& email() const { return email_; }
    int phoneNumber() const { return phoneNumber_; }

    // Setters: use string_view for non-owning input (borrowing)
    void setName(std::string_view name) { name_ = name; }  // Copy from view
    void setAge(int age) { age_ = age; }
    void setMajor(std::string_view major) { major_ = major; }  // Copy from view
    void setGrade(double grade) { grade_ = grade; }
    void setEmail(std::string_view email) { email_ = email; }  // Copy from view
    void setPhoneNumber(int phone) { phoneNumber_ = phone; }

    friend std::ostream& operator<<(std::ostream& os, const Student& s);
    friend std::istream& operator>>(std::istream& is, Student& s);

private:
    int id_;
    std::string name_;   // Owned copy
    int age_;
    std::string major_;  // Owned copy
    double grade_;
    std::string email_;  // Owned copy
    int phoneNumber_;
};

