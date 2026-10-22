# Facade Pattern

## What Is the Facade Pattern?

The Facade Pattern is a structural design pattern that provides a unified
interface for a set of interfaces within a subsystem. It defines a
higher-level interface that makes the subsystem easier to use.

> **Core idea**: hide the complexity of the subsystem behind a simplified
> interface.

## Why Do We Need It?

### Problem Scenario

Suppose we have a complex media playback system:

```cpp
// Problem: the client has to know too many details
class AudioCodec {
public:
    void decode(const std::string& file);
    void setQuality(int quality);
    void start();
    void stop();
};

class VideoCodec {
public:
    void decode(const std::string& file);
    void setResolution(int width, int height);
    void start();
    void stop();
};

class SubtitleRenderer {
public:
    void load(const std::string& file);
    void render();
    void hide();
};

class Speaker {
public:
    void play();
    void pause();
    void stop();
};

// Using it directly is complicated
void playMovie(const std::string& movie) {
    AudioCodec audio;
    VideoCodec video;
    SubtitleRenderer subtitle;
    Speaker speaker;
    
    audio.decode(movie);
    video.decode(movie);
    subtitle.load(movie + ".srt");
    
    audio.start();
    video.start();
    subtitle.render();
    speaker.play();
}
```

**Problems**:

1. The client needs to know too many subsystem details
2. Usage is complicated and error-prone
3. Hard to maintain

### Solution

Use the facade pattern:

```cpp
class MediaFacade {
public:
    void play(const std::string& movie) {
        audio_.decode(movie);
        video_.decode(movie);
        subtitle_.load(movie + ".srt");
        
        audio_.start();
        video_.start();
        subtitle_.render();
        speaker_.play();
    }
    
    void pause() {
        speaker_.pause();
    }
    
    void stop() {
        audio_.stop();
        video_.stop();
        subtitle_.hide();
        speaker_.stop();
    }

private:
    AudioCodec audio_;
    VideoCodec video_;
    SubtitleRenderer subtitle_;
    Speaker speaker_;
};

// Using it is simple
MediaFacade media;
media.play("movie.mp4");
```

**Benefits**:

1. Simplified interface: the client only talks to the facade
2. Decoupling: the client does not depend on the subsystems
3. Easy to use: the complexity is hidden

## Pattern Structure

```
                    ┌─────────────┐
                    │   Facade    │
                    │ (simplified)│
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  SubSystem1 │ │ SubSystem2 │ │ SubSystem3 │ ← subsystems
    │   (Audio)   │ │  (Video)   │ │ (Subtitle) │
    └─────────────┘ └────────────┘ └────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← client
                    └─────────────────┘
```

## Implementation

### 1. Define the Subsystems

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>

// the audio subsystem
class AudioDecoder {
public:
    void decode(const std::string& file) {
        std::cout << "[Audio] Decoding: " << file << "\n";
    }
    
    void play() {
        std::cout << "[Audio] Playing audio\n";
    }
    
    void stop() {
        std::cout << "[Audio] Stopping audio\n";
    }
};

// the video subsystem
class VideoDecoder {
public:
    void decode(const std::string& file) {
        std::cout << "[Video] Decoding: " << file << "\n";
    }
    
    void play() {
        std::cout << "[Video] Playing video\n";
    }
    
    void stop() {
        std::cout << "[Video] Stopping video\n";
    }
};

// the subtitle subsystem
class SubtitleRenderer {
public:
    void load(const std::string& file) {
        std::cout << "[Subtitle] Loading: " << file << "\n";
    }
    
    void render() {
        std::cout << "[Subtitle] Rendering subtitles\n";
    }
    
    void hide() {
        std::cout << "[Subtitle] Hiding subtitles\n";
    }
};

// the speaker subsystem
class Speaker {
public:
    void play() {
        std::cout << "[Speaker] Playing\n";
    }
    
    void pause() {
        std::cout << "[Speaker] Pausing\n";
    }
    
    void stop() {
        std::cout << "[Speaker] Stopping\n";
    }
};
```

### 2. Implement the Facade Class

```cpp
class MediaFacade {
public:
    MediaFacade()
        : audio_(std::make_unique<AudioDecoder>()),
          video_(std::make_unique<VideoDecoder>()),
          subtitle_(std::make_unique<SubtitleRenderer>()),
          speaker_(std::make_unique<Speaker>()) {}
    
    void play(const std::string& movie) {
        std::cout << "=== Playing: " << movie << " ===\n";
        
        // decode
        audio_->decode(movie);
        video_->decode(movie);
        subtitle_->load(movie + ".srt");
        
        // play
        audio_->play();
        video_->play();
        subtitle_->render();
        speaker_->play();
        
        std::cout << "=== Playback Started ===\n";
    }
    
    void pause() {
        std::cout << "=== Paused ===\n";
        speaker_->pause();
    }
    
    void stop() {
        std::cout << "=== Stopping ===\n";
        audio_->stop();
        video_->stop();
        subtitle_->hide();
        speaker_->stop();
    }

private:
    std::unique_ptr<AudioDecoder> audio_;
    std::unique_ptr<VideoDecoder> video_;
    std::unique_ptr<SubtitleRenderer> subtitle_;
    std::unique_ptr<Speaker> speaker_;
};
```

### 3. Client Usage

```cpp
int main() {
    MediaFacade media;
    
    // play the movie
    media.play("movie.mp4");
    
    // pause
    media.pause();
    
    // resume playback
    media.play("movie.mp4");
    
    // stop
    media.stop();
    
    return 0;
}
```

## Modern C++ Implementations

### With a Template Facade

```cpp
template<typename... SubSystems>
class GenericFacade {
public:
    template<typename T, typename... Args>
    void registerSubsystem(Args&&... args) {
        // register the subsystem
    }
    
    void performOperation() {
        // perform the operation
    }

private:
    std::tuple<SubSystems...> subsystems_;
};
```

### With the Builder Pattern

```cpp
class MediaBuilder {
public:
    MediaFacade& withAudio(std::unique_ptr<AudioDecoder> audio) {
        audio_ = std::move(audio);
        return *this;
    }
    
    MediaFacade& withVideo(std::unique_ptr<VideoDecoder> video) {
        video_ = std::move(video);
        return *this;
    }
    
    MediaFacade build() {
        return MediaFacade(std::move(audio_), std::move(video_));
    }

private:
    std::unique_ptr<AudioDecoder> audio_;
    std::unique_ptr<VideoDecoder> video_;
};
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Adapter** | An adapter converts an interface; a facade simplifies one |
| **Proxy** | A proxy controls access; a facade simplifies an interface |
| **Factory** | A factory creates objects; a facade provides a unified interface |

## Best Practices

### 1. Keep the Facade Simple

```cpp
// Recommended: the facade only wraps things simply
class SimpleFacade {
public:
    void doSomething() {
        subsystem1_->operation();
        subsystem2_->operation();
    }
};

// Not recommended: the facade contains too much logic
class ComplexFacade {
public:
    void doSomething() {
        // too much business logic...
        if (condition1) {
            // ...
        } else if (condition2) {
            // ...
        }
        // too much logic — it should be split into several facades
    }
};
```

### 2. Support Multiple Facades

```cpp
class SimpleMediaFacade {
public:
    void playAudio(const std::string& file) {
        audio_->decode(file);
        audio_->play();
    }
};

class FullMediaFacade {
public:
    void playMovie(const std::string& movie) {
        audio_->decode(movie);
        video_->decode(movie);
        subtitle_->load(movie + ".srt");
        // ...
    }
};
```

### 3. Use Dependency Injection

```cpp
class MediaFacade {
public:
    MediaFacade(std::unique_ptr<AudioDecoder> audio,
                std::unique_ptr<VideoDecoder> video,
                std::unique_ptr<SubtitleRenderer> subtitle,
                std::unique_ptr<Speaker> speaker)
        : audio_(std::move(audio)),
          video_(std::move(video)),
          subtitle_(std::move(subtitle)),
          speaker_(std::move(speaker)) {}

private:
    std::unique_ptr<AudioDecoder> audio_;
    std::unique_ptr<VideoDecoder> video_;
    std::unique_ptr<SubtitleRenderer> subtitle_;
    std::unique_ptr<Speaker> speaker_;
};
```

## Common Traps

### Trap 1: God Facade

```cpp
// Not recommended: the facade contains every feature
class GodFacade {
public:
    void playMovie() { /* ... */ }
    void playMusic() { /* ... */ }
    void recordVideo() { /* ... */ }
    void editPhoto() { /* ... */ }
    void processAudio() { /* ... */ }
    // too many features — hard to maintain
};

// Recommended: split it into several facades
class MediaPlayerFacade { ... };
class PhotoEditorFacade { ... };
class AudioProcessorFacade { ... };
```

### Trap 2: Exposing Subsystem Details

```cpp
// Not recommended: exposing the subsystems
class BadFacade {
public:
    AudioDecoder& getAudio() { return *audio_; }  // exposes the internals
    VideoDecoder& getVideo() { return *video_; }  // exposes the internals
};

// Recommended: expose only the interface you need
class GoodFacade {
public:
    void play() { audio_->play(); video_->play(); }
    void pause() { speaker_->pause(); }
};
```

### Trap 3: Overusing the Facade

```cpp
// Not recommended: creating a facade even for a simple operation
class SimpleOperation {
public:
    void doWork() {
        // a simple operation — no facade needed
    }
};

class SimpleFacade {  // over-engineering
public:
    void doWork() {
        operation_.doWork();
    }
private:
    SimpleOperation operation_;
};
```

## Example

A runnable version of this topic:

- [facade-demo.cpp](../../examples/facade-demo.cpp)

## Summary

The key points of the facade pattern:

- **When to use it**: complex subsystems, a need to simplify the interface,
  decoupling the client
- **Main benefits**: simpler usage, hidden complexity, lower coupling
- **Implementation advice**: stay simple, support multiple facades, use
  dependency injection
- **Common traps**: god facades, exposing details, overuse

> **Remember**: the key of the facade pattern is *simplifying the interface* —
> give a complex subsystem a simple interface so the client is easy to use.
