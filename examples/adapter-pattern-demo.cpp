// examples/adapter-pattern-demo.cpp
// Demonstrates Adapter Pattern: VlcPlayer + Mp3Player adapted to MediaPlayer interface.
// Compile: g++ -std=c++20 -o adapter-pattern-demo adapter-pattern-demo.cpp

#include <iostream>
#include <memory>
#include <string>

namespace adapter_pattern {

class MediaPlayer {
public:
    virtual ~MediaPlayer() = default;
    virtual void play(const std::string& filename) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
};

class VlcPlayer {
public:
    void playVlc(const std::string& f) { std::cout << "[VLC] Playing: " << f << "\n"; }
    void pauseVlc() { std::cout << "[VLC] Paused\n"; }
    void stopVlc()  { std::cout << "[VLC] Stopped\n"; }
};

class Mp3Player {
public:
    void loadMp3(const std::string& f) { std::cout << "[MP3] Loading: " << f << "\n"; }
    void playMp3()  { std::cout << "[MP3] Playing\n"; }
    void stopMp3()   { std::cout << "[MP3] Stopped\n"; }
};

class VlcAdapter : public MediaPlayer {
public:
    explicit VlcAdapter(std::shared_ptr<VlcPlayer> p) : player_(std::move(p)) {}
    void play(const std::string& f) override  { player_->playVlc(f); }
    void pause() override                     { player_->pauseVlc(); }
    void stop() override                     { player_->stopVlc(); }
private:
    std::shared_ptr<VlcPlayer> player_;
};

class Mp3Adapter : public MediaPlayer {
public:
    explicit Mp3Adapter(std::shared_ptr<Mp3Player> p) : player_(std::move(p)) {}
    void play(const std::string& f) override { player_->loadMp3(f); player_->playMp3(); }
    void pause() override                    { std::cout << "[MP3] Pause not supported\n"; }
    void stop() override                    { player_->stopMp3(); }
private:
    std::shared_ptr<Mp3Player> player_;
};

} // namespace adapter_pattern

int main() {
    using namespace adapter_pattern;
    std::cout << "=== Adapter Pattern Demo ===\n\n";

    std::shared_ptr<MediaPlayer> player1 = std::make_shared<VlcAdapter>(std::make_shared<VlcPlayer>());
    std::shared_ptr<MediaPlayer> player2 = std::make_shared<Mp3Adapter>(std::make_shared<Mp3Player>());

    player1->play("movie.vlc");
    player1->pause();
    player1->stop();

    player2->play("song.mp3");
    player2->stop();

    std::cout << "\n=== Demo Complete ===\n";
    return 0;
}
