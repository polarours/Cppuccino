#include <iostream>
#include <vector>
#include <utility>

std::vector<std::string> data_;

void push_back_left(const std::string& value) {
    data_.push_back(value);  
}

void push_back_right(std::string&& value) {
    data_.push_back(std::move(value));  
}

int main() {
    std::string s_1 = "Hello";   
    std::string s_2 = "World";   

    push_back_left(s_1); 

    push_back_right(std::move(s_2));  

    for(size_t i = 0; i < data_.size(); ++i) {
        std::cout << data_[i] << " ";
    }
    std::cout << std::endl;

    std::cout << "s_1: " << s_1 << std::endl;  // s_1 should remain "Hello"
    std::cout << "s_2: " << (s_2.empty() ? "nullptr" : s_2.c_str()) << std::endl;  // s_2 may be in a valid but unspecified state

    return 0;
}
