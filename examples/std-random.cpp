// examples/std-random.cpp
// Demonstrates C++17 random number generation.
// Compile with: g++ -std=c++17 -o std_random std-random.cpp

#include <iostream>
#include <random>
#include <vector>
#include <algorithm>
#include <map>

// --- 1. Basic random ---

void basicRandom() {
    std::cout << "--- Basic Random ---\n";

    // Random device for seeding
    std::random_device rd;

    // Mersenne Twister engine
    std::mt19937 gen(rd());

    // Uniform distribution
    std::uniform_int_distribution<> dis(1, 100);

    std::cout << "Random numbers: ";
    for (int i = 0; i < 10; ++i) {
        std::cout << dis(gen) << " ";
    }
    std::cout << "\n";
}

// --- 2. Different distributions ---

void distributions() {
    std::cout << "\n--- Distributions ---\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    // Uniform real
    std::uniform_real_distribution<> real_dis(0.0, 1.0);
    std::cout << "Uniform real: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << real_dis(gen) << " ";
    }
    std::cout << "\n";

    // Normal distribution
    std::normal_distribution<> normal_dis(0.0, 1.0);
    std::cout << "Normal: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << normal_dis(gen) << " ";
    }
    std::cout << "\n";

    // Bernoulli distribution
    std::bernoulli_distribution bern_dis(0.7);
    std::cout << "Bernoulli: ";
    for (int i = 0; i < 10; ++i) {
        std::cout << bern_dis(gen) << " ";
    }
    std::cout << "\n";
}

// --- 3. Seeding ---

void seeding() {
    std::cout << "\n--- Seeding ---\n";

    // Fixed seed for reproducibility
    std::mt19937 gen(42);
    std::uniform_int_distribution<> dis(1, 100);

    std::cout << "Same seed produces same sequence:\n";
    for (int i = 0; i < 5; ++i) {
        std::cout << dis(gen) << " ";
    }
    std::cout << "\n";

    // Reset seed
    gen.seed(42);
    std::cout << "After reset: ";
    for (int i = 0; i < 5; ++i) {
        std::cout << dis(gen) << " ";
    }
    std::cout << "\n";
}

// --- 4. Random in containers ---

void randomInContainers() {
    std::cout << "\n--- Random in Containers ---\n";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 100);

    std::vector<int> vec(10);
    std::generate(vec.begin(), vec.end(), [&]() { return dis(gen); });

    std::cout << "Random vector: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 5. Shuffle ---

void shuffleDemo() {
    std::cout << "\n--- Shuffle ---\n";

    std::vector<int> vec = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::random_device rd;
    std::mt19937 gen(rd());

    std::shuffle(vec.begin(), vec.end(), gen);

    std::cout << "Shuffled: ";
    for (int n : vec) std::cout << n << " ";
    std::cout << "\n";
}

// --- 6. Random choice ---

void randomChoice() {
    std::cout << "\n--- Random Choice ---\n";

    std::vector<std::string> choices = {"apple", "banana", "cherry", "date"};

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, choices.size() - 1);

    std::cout << "Random choices: ";
    for (int i = 0; i < 10; ++i) {
        std::cout << choices[dis(gen)] << " ";
    }
    std::cout << "\n";
}

// --- 7. Weighted distribution ---

void weightedDistribution() {
    std::cout << "\n--- Weighted Distribution ---\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    // Weights for [0, 1, 2]
    std::discrete_distribution<> dis({1, 2, 3});

    std::map<int, int> counts;
    for (int i = 0; i < 1000; ++i) {
        counts[dis(gen)]++;
    }

    std::cout << "Distribution (1000 samples):\n";
    for (const auto& [value, count] : counts) {
        std::cout << "  " << value << ": " << count << "\n";
    }
}

// --- 8. Piecewise distribution ---

void piecewiseDistribution() {
    std::cout << "\n--- Piecewise Distribution ---\n";

    std::random_device rd;
    std::mt19937 gen(rd());

    // Poisson distribution as alternative
    std::poisson_distribution<> dis(3.0);

    std::map<int, int> counts;
    for (int i = 0; i < 1000; ++i) {
        counts[dis(gen)]++;
    }

    std::cout << "Poisson distribution (1000 samples):\n";
    for (const auto& [value, count] : counts) {
        if (value <= 10) {
            std::cout << "  " << value << ": " << count << "\n";
        }
    }
}

// --- 9. Sampling ---

void samplingDemo() {
    std::cout << "\n--- Sampling ---\n";

    std::vector<int> population = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    std::random_device rd;
    std::mt19937 gen(rd());

    // Sample 3 with replacement
    std::uniform_int_distribution<> dis(0, population.size() - 1);
    std::cout << "Sample with replacement: ";
    for (int i = 0; i < 3; ++i) {
        std::cout << population[dis(gen)] << " ";
    }
    std::cout << "\n";
}

// --- 10. Practical example ---

void practicalExample() {
    std::cout << "\n--- Practical Example ---\n";

    // Simple dice roller
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(1, 6);

    std::cout << "Rolling 5 dice: ";
    int total = 0;
    for (int i = 0; i < 5; ++i) {
        int roll = dis(gen);
        std::cout << roll << " ";
        total += roll;
    }
    std::cout << "\nTotal: " << total << "\n";
}

int main() {
    basicRandom();
    distributions();
    seeding();
    randomInContainers();
    shuffleDemo();
    randomChoice();
    weightedDistribution();
    piecewiseDistribution();
    samplingDemo();
    practicalExample();

    std::cout << "\n=== All random scenarios completed ===\n";
    return 0;
}
