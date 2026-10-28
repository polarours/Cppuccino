#include "adapter_pattern.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <memory>
#include <sstream>
#include <vector>

namespace {

void expect(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error(message);
    }
}

// The classes under test report what they do by printing, so the output
// is what the test looks at.
class CapturedOutput {
public:
    CapturedOutput() : previous_(std::cout.rdbuf(buffer_.rdbuf())) {}
    ~CapturedOutput() { std::cout.rdbuf(previous_); }

    CapturedOutput(const CapturedOutput&) = delete;
    CapturedOutput& operator=(const CapturedOutput&) = delete;

    std::string text() const { return buffer_.str(); }

private:
    std::ostringstream buffer_;
    std::streambuf* previous_;
};

void test_vlc_adapter() {
    auto vlc = std::make_shared<adapter_pattern::VlcPlayer>();
    auto adapter = std::make_shared<adapter_pattern::VlcAdapter>(vlc);

    CapturedOutput output;
    adapter->play("test.vlc");
    adapter->pause();
    adapter->stop();

    const std::string text = output.text();
    expect(text.find("[VLC] Playing: test.vlc") != std::string::npos,
           "expected the vlc player to play the file");
    expect(text.find("[VLC] Paused") != std::string::npos,
           "expected the vlc player to pause");
    expect(text.find("[VLC] Stopped") != std::string::npos,
           "expected the vlc player to stop");
}

void test_mp3_adapter() {
    auto mp3 = std::make_shared<adapter_pattern::Mp3Player>();
    auto adapter = std::make_shared<adapter_pattern::Mp3Adapter>(mp3);

    CapturedOutput output;
    adapter->play("test.mp3");
    adapter->stop();

    const std::string text = output.text();
    expect(text.find("[MP3] Loading: test.mp3") != std::string::npos,
           "expected the mp3 player to load the file");
    expect(text.find("[MP3] Playing") != std::string::npos,
           "expected the mp3 player to play");
    expect(text.find("[MP3] Stopped") != std::string::npos,
           "expected the mp3 player to stop");
}

void test_common_interface() {
    auto vlc = std::make_shared<adapter_pattern::VlcPlayer>();
    auto mp3 = std::make_shared<adapter_pattern::Mp3Player>();

    std::vector<std::shared_ptr<adapter_pattern::MediaPlayer>> players;
    players.push_back(std::make_shared<adapter_pattern::VlcAdapter>(vlc));
    players.push_back(std::make_shared<adapter_pattern::Mp3Adapter>(mp3));

    // Use common interface
    CapturedOutput output;
    for (auto& player : players) {
        player->play("test");
        player->stop();
    }

    const std::string text = output.text();
    expect(text.find("[VLC] Playing: test") != std::string::npos,
           "expected the vlc player to play through the common interface");
    expect(text.find("[MP3] Loading: test") != std::string::npos,
           "expected the mp3 player to play through the common interface");
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
