// examples/facade-demo.cpp
// Demonstrates the Facade Pattern: a simplified interface over a set of
// complex subsystems (home theater boot sequence).
// Compile: g++ -std=c++20 -Wall -Wextra -o facade-demo facade-demo.cpp

#include <iostream>
#include <string>

namespace facade_demo {

class Amplifier {
public:
    void on() { std::cout << "Amplifier on\n"; volume_ = 5; }
    void setVolume(int v) { volume_ = v; std::cout << "Amplifier volume " << v << "\n"; }
    void off() { std::cout << "Amplifier off\n"; }
private:
    int volume_ = 0;
};

class Projector {
public:
    void on() { std::cout << "Projector on\n"; }
    void setResolution(const std::string& r) { std::cout << "Projector resolution " << r << "\n"; }
    void off() { std::cout << "Projector off\n"; }
};

class StreamingPlayer {
public:
    void on() { std::cout << "Player on\n"; }
    void play(const std::string& movie) {
        movie_ = movie;
        std::cout << "Playing \"" << movie_ << "\"\n";
    }
    void stop() { std::cout << "Stopped \"" << movie_ << "\"\n"; }
    void off() { std::cout << "Player off\n"; }
private:
    std::string movie_;
};

// Facade: one button instead of coordinating 3 subsystems manually
class HomeTheaterFacade {
public:
    void watchMovie(const std::string& movie) {
        std::cout << "Get ready to watch a movie...\n";
        projector_.on();
        projector_.setResolution("1080p");
        amplifier_.on();
        amplifier_.setVolume(7);
        player_.on();
        player_.play(movie);
    }
    void endMovie() {
        std::cout << "Shutting movie theater down...\n";
        player_.stop();
        player_.off();
        amplifier_.off();
        projector_.off();
    }

private:
    Amplifier amplifier_;
    Projector projector_;
    StreamingPlayer player_;
};

} // namespace facade_demo

int main() {
    using namespace facade_demo;
    std::cout << "=== Facade Pattern Demo ===\n\n";

    HomeTheaterFacade theater;
    theater.watchMovie("The C++ Standard");
    std::cout << "\n";
    theater.endMovie();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
