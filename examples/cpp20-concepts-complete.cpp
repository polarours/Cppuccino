#include <iostream>
#include <concepts>
#include <string>
#include <vector>
#include <numeric>
#include <algorithm>

// C++20 Concepts 完整示例
// 展示 concepts 的多种用法和组合

// 基础概念定义
template<typename T>
concept Printable = requires(T a) {
    { std::cout << a } -> std::convertible_to<std::ostream&>;
};

template<typename T>
concept Addable = requires(T a, T b) {
    { a + b } -> std::same_as<T>;
};

template<typename T>
concept Sortable = requires(T a) {
    { a.begin() } -> std::input_iterator;
    { a.end() } -> std::input_iterator;
};

// 概念组合
template<typename T>
concept Number = std::integral<T> || std::floating_point<T>;

template<typename T>
concept Container = requires(T a) {
    { a.size() } -> std::convertible_to<std::size_t>;
    { a.begin() } -> std::input_iterator;
    { a.end() } -> std::input_iterator;
};

template<Printable T, Addable T>
concept PrintableAndAddable = true;

// 使用概念的函数
template<Printable T>
void print(const T& value) {
    std::cout << "Value: " << value << "\n";
}

template<Number T>
T add(T a, T b) {
    return a + b;
}

template<Sortable T>
void sortContainer(T& container) {
    std::sort(container.begin(), container.end());
}

// 概念约束的成员函数
class Calculator {
public:
    template<Number T>
    T calculate(T a, T b) requires(true) {
        return a + b;
    }
    
    template<Container C>
    auto sum(const C& container) -> decltype(*container.begin()) {
        return std::accumulate(container.begin(), container.end(), 
                               decltype(*container.begin()){});
    }
};

// 概念用于模板特化
template<Printable T>
struct TypeTraits {
    static void describe() {
        std::cout << "Type is printable\n";
    }
};

// 约束模板别名
template<Number T>
using NumberRef = T&;

// 概念用于类模板
template<Container C>
class ContainerWrapper {
public:
    explicit ContainerWrapper(C container) : container_(std::move(container)) {}
    
    void print() const {
        for (const auto& item : container_) {
            std::cout << item << " ";
        }
        std::cout << "\n";
    }

private:
    C container_;
};

int main() {
    std::cout << "=== C++20 Concepts Demo ===\n\n";
    
    // 1. 基础概念使用
    std::cout << "1. Basic concepts:\n";
    print(42);
    print(std::string("Hello"));
    
    // 2. 数值概念
    std::cout << "\n2. Number concept:\n";
    std::cout << "int add: " << add(10, 20) << "\n";
    std::cout << "double add: " << add(10.5, 20.3) << "\n";
    
    // 3. 容器排序
    std::cout << "\n3. Sortable container:\n";
    std::vector<int> vec = {5, 2, 8, 1, 9};
    sortContainer(vec);
    std::cout << "Sorted: ";
    for (auto v : vec) std::cout << v << " ";
    std::cout << "\n";
    
    // 4. 组合概念
    std::cout << "\n4. Combined concepts:\n";
    if constexpr (Printable<int> && Addable<int>) {
        std::cout << "int is both Printable and Addable\n";
    }
    
    // 5. ContainerWrapper
    std::cout << "\n5. ContainerWrapper:\n";
    ContainerWrapper<std::vector<int>> wrapper({1, 2, 3, 4, 5});
    wrapper.print();
    
    // 6. 类型特征
    std::cout << "\n6. Type traits:\n";
    TypeTraits<int>::describe();
    TypeTraits<std::string>::describe();
    
    // 7. 概念约束
    std::cout << "\n7. Concept constraints:\n";
    Calculator calc;
    std::cout << "Calculate: " << calc.calculate(10, 20) << "\n";
    
    std::vector<double> nums = {1.5, 2.5, 3.0};
    std::cout << "Sum: " << calc.sum(nums) << "\n";
    
    return 0;
}
