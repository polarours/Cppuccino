#include "adapter_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

void test_vlc_adapter() {
    auto vlc = std::make_shared<adapter_pattern::VlcPlayer>();
    auto adapter = std::make_shared<adapter_pattern::VlcAdapter>(vlc);

    // Should not throw
    adapter->play("test.vlc");
    adapter->pause();
    adapter->stop();
}

void test_mp3_adapter() {
    auto mp3 = std::make_shared<adapter_pattern::Mp3Player>();
    auto adapter = std::make_shared<adapter_pattern::Mp3Adapter>(mp3);

    // Should not throw
    adapter->play("test.mp3");
    adapter->stop();
}

void test_common_interface() {
    auto vlc = std::make_shared<adapter_pattern::VlcPlayer>();
    auto mp3 = std::make_shared<adapter_pattern::Mp3Player>();

    std::vector<std::shared_ptr<adapter_pattern::MediaPlayer>> players;
    players.push_back(std::make_shared<adapter_pattern::VlcAdapter>(vlc));
    players.push_back(std::make_shared<adapter_pattern::Mp3Adapter>(mp3));

    // Use common interface
    for (auto& player : players) {
        player->play("test");
        player->stop();
    }
}

} // namespace

int main() {
    try {
        std::cout << "Adapter Pattern Tests:\n";
        test_vlc_adapter();
        std::cout << "  test_vlc_adapter: PASS\n";

        test_mp3_adapter();
        std::cout << "  test_mp3_adapter: PASS\n";

        test_common_interface();
        std::cout << "  test_common_interface: PASS\n";

    } catch (const std::exception& exception) {
        std::cerr << "adapter_pattern_tests failed: " << exception.what() << std::endl;
        return 1;
    }

    std::cout << "\nadapter_pattern_tests passed" << std::endl;
    return 0;
}
