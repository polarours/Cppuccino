# pragma once

#include <string>

class Student {
    public:
        Student(int id, const std::string& name, int age, const std::string& major, double grade, const std::string& email, const int phoneNumber);

        int getID() const;
        std::string getName() const;
        int getAge() const;
        std::string getMajor() const;
        double getGrade() const;
        std::string getEmail() const;
        int getPhoneNumber() const;

        void setID(int id);
        void setName(const std::string& name);
        void setAge(int age);
        void setMajor(const std::string& major);
        void setGrade(double grade);
        void setEmail(const std::string& email);
        void setPhoneNumber(int phoneNumber);

    private:
        int id_;
        std::string name_;
        int age_;
        std::string major_;
        double grade_;
        std::string email_;
        int phoneNumber_;
};