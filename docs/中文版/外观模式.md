# 外观模式 (Facade Pattern)

## 什么是外观模式？

外观模式（Facade Pattern）是一种结构型设计模式，它为子系统中的一组接口提供一个一致的界面，外观模式定义了一个高层接口，这个接口使得这一子系统更加容易使用。

> **核心思想**：通过提供一个简化的接口来隐藏子系统的复杂性。

## 为什么需要外观模式？

### 问题场景

假设我们有一个复杂的媒体播放系统：

```cpp
// 问题：客户端需要了解太多细节
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

// 客户端使用很复杂
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

**问题**：
1. 客户端需要了解太多子系统细节
2. 使用复杂，容易出错
3. 难以维护

### 解决方案

使用外观模式：

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

// 客户端使用简单
MediaFacade media;
media.play("movie.mp4");
```

**优点**：
1. 简化接口：客户端只需调用外观
2. 解耦：客户端不依赖子系统
3. 易于使用：隐藏了复杂性

## 模式结构

```
                    ┌─────────────┐
                    │   Facade    │
                    │  (外观)      │
                    └──────┬──────┘
                           │
              ┌────────────┼────────────┐
              │            │            │
    ┌─────────▼───┐ ┌──────▼─────┐ ┌───▼────────┐
    │  SubSystem1 │ │ SubSystem2 │ │ SubSystem3 │ ← 子系统
    │   (Audio)   │ │  (Video)   │ │ (Subtitle) │
    └─────────────┘ └────────────┘ └────────────┘
                           │
                    ┌────────▼────────┐
                    │    Client       │ ← 客户端
                    └─────────────────┘
```

## 代码实现

### 1. 定义子系统

```cpp
#include <memory>
#include <string>
#include <iostream>
#include <vector>

// 音频子系统
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

// 视频子系统
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

// 字幕子系统
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

// 扬声器子系统
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

### 2. 实现外观类

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
        
        // 解码
        audio_->decode(movie);
        video_->decode(movie);
        subtitle_->load(movie + ".srt");
        
        // 播放
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

### 3. 客户端使用

```cpp
int main() {
    MediaFacade media;
    
    // 播放电影
    media.play("movie.mp4");
    
    // 暂停
    media.pause();
    
    // 继续播放
    media.play("movie.mp4");
    
    // 停止
    media.stop();
    
    return 0;
}
```

## 现代 C++ 实现

### 使用模板外观

```cpp
template<typename... SubSystems>
class GenericFacade {
public:
    template<typename T, typename... Args>
    void registerSubsystem(Args&&... args) {
        // 注册子系统
    }
    
    void performOperation() {
        // 执行操作
    }

private:
    std::tuple<SubSystems...> subsystems_;
};
```

### 使用 builder 模式

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

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **适配器模式** | 适配器转换接口；外观简化接口 |
| **代理模式** | 代理控制访问；外观简化接口 |
| **工厂模式** | 工厂创建对象；外观提供统一接口 |

## 最佳实践

### 1. 保持外观简单

```cpp
// 推荐：外观只做简单的封装
class SimpleFacade {
public:
    void doSomething() {
        subsystem1_->operation();
        subsystem2_->operation();
    }
};

// 不推荐：外观包含太多逻辑
class ComplexFacade {
public:
    void doSomething() {
        // 太多业务逻辑...
        if (condition1) {
            // ...
        } else if (condition2) {
            // ...
        }
        // 太多逻辑，应该拆分成多个外观
    }
};
```

### 2. 支持多个外观

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

### 3. 使用依赖注入

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

## 常见陷阱

### 陷阱 1：God Facade

```cpp
// 不推荐：外观包含所有功能
class GodFacade {
public:
    void playMovie() { /* ... */ }
    void playMusic() { /* ... */ }
    void recordVideo() { /* ... */ }
    void editPhoto() { /* ... */ }
    void processAudio() { /* ... */ }
    // 太多功能，难以维护
};

// 推荐：拆分成多个外观
class MediaPlayerFacade { ... };
class PhotoEditorFacade { ... };
class AudioProcessorFacade { ... };
```

### 陷阱 2：暴露子系统细节

```cpp
// 不推荐：暴露子系统
class BadFacade {
public:
    AudioDecoder& getAudio() { return *audio_; }  // 暴露内部
    VideoDecoder& getVideo() { return *video_; }  // 暴露内部
};

// 推荐：只暴露需要的接口
class GoodFacade {
public:
    void play() { audio_->play(); video_->play(); }
    void pause() { speaker_->pause(); }
};
```

### 陷阱 3：过度使用外观

```cpp
// 不推荐：为简单操作也创建外观
class SimpleOperation {
public:
    void doWork() {
        // 简单操作，不需要外观
    }
};

class SimpleFacade {  // 过度设计
public:
    void doWork() {
        operation_.doWork();
    }
private:
    SimpleOperation operation_;
};
```

## 总结

外观模式的核心要点：

- **适用场景**：子系统复杂、需要简化接口、解耦客户端
- **核心优点**：简化使用、隐藏复杂性、降低耦合
- **实现要点**：保持简单、支持多个外观、使用依赖注入
- **常见陷阱**：God Facade、暴露细节、过度使用

> **记住**：外观模式的关键是"简化接口"——为复杂的子系统提供一个简单的接口，让客户端更容易使用。
