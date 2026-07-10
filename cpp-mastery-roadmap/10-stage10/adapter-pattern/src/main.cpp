#include "adapter_pattern.hpp"

#include <iostream>
#include <memory>
#include <vector>

int main() {
    std::cout << "=== Adapter Pattern Demo ===\n\n";

    // Create adaptees
    auto vlc = std::make_shared<adapter_pattern::VlcPlayer>();
    auto mp3 = std::make_shared<adapter_pattern::Mp3Player>();

    // Create adapters
    std::vector<std::shared_ptr<adapter_pattern::MediaPlayer>> players;
    players.push_back(std::make_shared<adapter_pattern::VlcAdapter>(vlc));
    players.push_back(std::make_shared<adapter_pattern::Mp3Adapter>(mp3));

    // Use common interface
    for (auto& player : players) {
        player->play("test.mp3");
        player->stop();
        std::cout << "\n";
    }

    std::cout << "=== Demo Complete ===\n";
    return 0;
}
