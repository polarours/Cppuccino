#pragma once

#include <string>
#include <iostream>
#include <memory>

namespace adapter_pattern {

// --- Target Interface ---

class MediaPlayer {
public:
    virtual ~MediaPlayer() = default;
    virtual void play(const std::string& filename) = 0;
    virtual void pause() = 0;
    virtual void stop() = 0;
};

// --- Adaptee ---

class VlcPlayer {
public:
    void playVlc(const std::string& filename) {
        std::cout << "[VLC] Playing: " << filename << "\n";
    }

    void pauseVlc() {
        std::cout << "[VLC] Paused\n";
    }

    void stopVlc() {
        std::cout << "[VLC] Stopped\n";
    }
};

class Mp3Player {
public:
    void loadMp3(const std::string& filename) {
        std::cout << "[MP3] Loading: " << filename << "\n";
    }

    void playMp3() {
        std::cout << "[MP3] Playing\n";
    }

    void stopMp3() {
        std::cout << "[MP3] Stopped\n";
    }
};

// --- Adapter ---

class VlcAdapter : public MediaPlayer {
public:
    explicit VlcAdapter(std::shared_ptr<VlcPlayer> player)
        : player_(std::move(player)) {}

    void play(const std::string& filename) override {
        player_->playVlc(filename);
    }

    void pause() override {
        player_->pauseVlc();
    }

    void stop() override {
        player_->stopVlc();
    }

private:
    std::shared_ptr<VlcPlayer> player_;
};

class Mp3Adapter : public MediaPlayer {
public:
    explicit Mp3Adapter(std::shared_ptr<Mp3Player> player)
        : player_(std::move(player)) {}

    void play(const std::string& filename) override {
        player_->loadMp3(filename);
        player_->playMp3();
    }

    void pause() override {
        std::cout << "[MP3] Pause not supported\n";
    }

    void stop() override {
        player_->stopMp3();
    }

private:
    std::shared_ptr<Mp3Player> player_;
};

} // namespace adapter_pattern
