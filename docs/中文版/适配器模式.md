# 适配器模式 (Adapter Pattern)

## 什么是适配器模式？

适配器模式（Adapter Pattern）是一种结构型设计模式，它允许将一个类的接口转换成客户期望的另一个接口。适配器让不兼容的接口能够一起工作。

> **核心思想**：通过适配器包装不兼容的接口，使它们能够协同工作。

## 为什么需要适配器模式？

### 问题场景

假设我们有一个图形绘制程序，需要支持多种格式：

```cpp
// 现有接口：XML 解析器
class XMLParser {
public:
    void parse(const std::string& xml);
    std::string getValue(const std::string& key);
};

// 目标接口：配置读取器
class ConfigReader {
public:
    virtual std::string get(const std::string& key) = 0;
    virtual ~ConfigReader() = default;
};

// 问题：XMLParser 的接口与 ConfigReader 不匹配
```

**问题**：
1. XMLParser 使用 `parse()` + `getValue()`
2. ConfigReader 期望 `get()` 方法
3. 无法直接使用 XMLParser

### 解决方案

使用适配器模式：

```cpp
// 适配器：将 XMLParser 适配为 ConfigReader
class XMLParserAdapter : public ConfigReader {
public:
    explicit XMLParserAdapter(std::unique_ptr<XMLParser> parser)
        : parser_(std::move(parser)) {}
    
    std::string get(const std::string& key) override {
        return parser_->getValue(key);
    }

private:
    std::unique_ptr<XMLParser> parser_;
};

// 使用
auto adapter = std::make_unique<XMLParserAdapter>(
    std::make_unique<XMLParser>()
);
std::string value = adapter->get("key");
```

**优点**：
1. 解耦：客户端不需要知道具体实现
2. 复用：可以复用现有的不兼容类
3. 灵活：可以动态切换适配器

## 模式结构

```
                    ┌─────────────┐
                    │  Target     │ ← 目标接口
                    │  (ConfigReader)│
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │     Adapter             │ ← 适配器
              │  (XMLParserAdapter)     │
              └────────────┬────────────┘
                           │ adapts
                           │
              ┌────────────▼────────────┐
              │      Adaptee            │ ← 被适配的类
              │     (XMLParser)         │
              └─────────────────────────┘
```

## 代码实现

### 1. 目标接口

```cpp
#include <memory>
#include <string>
#include <iostream>

// 目标接口：音乐播放器
class MediaPlayer {
public:
    virtual void play(const std::string& format, const std::string& filename) = 0;
    virtual ~MediaPlayer() = default;
};
```

### 2. 被适配的类

```cpp
// 现有类：VLC 播放器（只支持 vlc 格式）
class VLCPlayer {
public:
    void playVlc(const std::string& file) {
        std::cout << "Playing VLC file: " << file << "\n";
    }
};

// 现有类：MP3 播放器（只支持 mp3 格式）
class MP3Player {
public:
    void playMp3(const std::string& file) {
        std::cout << "Playing MP3 file: " << file << "\n";
    }
};

// 现有类：MP4 播放器（只支持 mp4 格式）
class MP4Player {
public:
    void playMp4(const std::string& file) {
        std::cout << "Playing MP4 file: " << file << "\n";
    }
};
```

### 3. 实现适配器

```cpp
// 适配器：将 VLCPlayer 适配为 MediaPlayer
class VLCAdapter : public MediaPlayer {
public:
    explicit VLCAdapter(std::unique_ptr<VLCPlayer> player)
        : player_(std::move(player)) {}
    
    void play(const std::string& format, const std::string& filename) override {
        if (format == "vlc") {
            player_->playVlc(filename);
        } else {
            std::cout << "Unsupported format: " << format << "\n";
        }
    }

private:
    std::unique_ptr<VLCPlayer> player_;
};

// 适配器：将 MP3Player 适配为 MediaPlayer
class MP3Adapter : public MediaPlayer {
public:
    explicit MP3Adapter(std::unique_ptr<MP3Player> player)
        : player_(std::move(player)) {}
    
    void play(const std::string& format, const std::string& filename) override {
        if (format == "mp3") {
            player_->playMp3(filename);
        } else {
            std::cout << "Unsupported format: " << format << "\n";
        }
    }

private:
    std::unique_ptr<MP3Player> player_;
};

// 适配器：将 MP4Player 适配为 MediaPlayer
class MP4Adapter : public MediaPlayer {
public:
    explicit MP4Adapter(std::unique_ptr<MP4Player> player)
        : player_(std::move(player)) {}
    
    void play(const std::string& format, const std::string& filename) override {
        if (format == "mp4") {
            player_->playMp4(filename);
        } else {
            std::cout << "Unsupported format: " << format << "\n";
        }
    }

private:
    std::unique_ptr<MP4Player> player_;
};
```

### 4. 客户端使用

```cpp
int main() {
    // 创建播放器
    auto vlc = std::make_unique<VLCPlayer>();
    auto mp3 = std::make_unique<MP3Player>();
    auto mp4 = std::make_unique<MP4Player>();
    
    // 创建适配器
    MediaPlayer* player = new VLCAdapter(std::move(vlc));
    
    // 使用统一接口
    player->play("vlc", "movie.vlc");
    
    // 切换播放器
    delete player;
    player = new MP3Adapter(std::move(mp3));
    player->play("mp3", "song.mp3");
    
    delete player;
    player = new MP4Adapter(std::move(mp4));
    player->play("mp4", "video.mp4");
    
    delete player;
    
    return 0;
}
```

## 变体与扩展

### 变体 1：类适配器（多重继承）

```cpp
// 类适配器：通过多重继承实现
class VLCClassAdapter : public MediaPlayer, public VLCPlayer {
public:
    void play(const std::string& format, const std::string& filename) override {
        if (format == "vlc") {
            playVlc(filename);  // 直接调用 VLCPlayer 的方法
        }
    }
};
```

**注意**：C++ 中类适配器使用多重继承，不如对象适配器灵活。

### 变体 2：双向适配器

```cpp
// 双向适配器：同时适配两个接口
class SoundConverter : public AudioInput, public AudioOutput {
public:
    void setInput(const AudioData& data) override {
        output_.process(data);
    }
    
    AudioData getOutput() override {
        return input_.capture();
    }

private:
    AudioInput input_;
    AudioOutput output_;
};
```

### 变体 3：适配器工厂

```cpp
class AdapterFactory {
public:
    static std::unique_ptr<MediaPlayer> create(const std::string& type) {
        if (type == "vlc") {
            return std::make_unique<VLCAdapter>(
                std::make_unique<VLCPlayer>()
            );
        } else if (type == "mp3") {
            return std::make_unique<MP3Adapter>(
                std::make_unique<MP3Player>()
            );
        } else if (type == "mp4") {
            return std::make_unique<MP4Adapter>(
                std::make_unique<MP4Player>()
            );
        }
        throw std::invalid_argument("Unknown player type: " + type);
    }
};

// 使用
auto player = AdapterFactory::create("mp3");
player->play("mp3", "song.mp3");
```

## 与相关模式的区别

| 模式 | 区别 |
|------|------|
| **桥接模式** | 桥接分离抽象和实现；适配器转换接口 |
| **装饰器模式** | 装饰器增强功能；适配器转换接口 |
| **代理模式** | 代理控制访问；适配器转换接口 |

## 最佳实践

### 1. 使用对象适配器（首选）

```cpp
// 推荐：对象适配器（组合）
class Adapter : public Target {
public:
    explicit Adapter(std::unique_ptr<Adaptee> adaptee)
        : adaptee_(std::move(adaptee)) {}

private:
    std::unique_ptr<Adaptee> adaptee_;
};

// 不推荐：类适配器（多重继承）
class Adapter : public Target, public Adaptee { ... };
```

### 2. 适配器应该透明

```cpp
// 推荐：适配器对外隐藏内部细节
class ConfigAdapter : public ConfigReader {
public:
    std::string get(const std::string& key) override {
        // 适配器内部处理转换
        return parser_->getValue(key);
    }
};
```

### 3. 考虑使用模板适配器

```cpp
template<typename Adaptee>
class TemplateAdapter : public Target {
public:
    explicit TemplateAdapter(std::unique_ptr<Adaptee> adaptee)
        : adaptee_(std::move(adaptee)) {}
    
    Result process() override {
        return adaptee_->adaptedProcess();
    }

private:
    std::unique_ptr<Adaptee> adaptee_;
};

// 使用
auto adapter = std::make_unique<TemplateAdapter<VLCPlayer>>(
    std::make_unique<VLCPlayer>()
);
```

## 常见陷阱

### 陷阱 1：适配器过多

```cpp
// 问题：创建了太多适配器，反而增加了复杂度
class JSONtoXMLAdapter { ... };
class XMLtoJSONAdapter { ... };
class CSVtoXMLAdapter { ... };
class XMLtoCSVAdapter { ... };

// 推荐：考虑统一接口或序列化库
```

### 陷阱 2：适配器循环依赖

```cpp
// 问题：两个适配器互相依赖
class AdapterA : public TargetA {
public:
    void process() override {
        adapterB_->convert(data_);  // 依赖 B
    }
    std::unique_ptr<AdapterB> adapterB_;
};

class AdapterB : public TargetB {
public:
    void process() override {
        adapterA_->convert(data_);  // 依赖 A
    }
    std::unique_ptr<AdapterA> adapterA_;
};
```

### 陷阱 3：适配器泄露资源

```cpp
// 问题：适配器管理资源但未正确释放
class BadAdapter : public Target {
public:
    void process() {
        resource_ = allocate();  // 分配资源
        // 忘记释放！
    }
    
    Resource* resource_ = nullptr;
};

// 推荐：使用智能指针
class GoodAdapter : public Target {
public:
    void process() {
        resource_ = std::make_unique<Resource>();
        // 自动释放
    }
    
    std::unique_ptr<Resource> resource_;
};
```

## 总结

适配器模式的核心要点：

- **适用场景**：复用不兼容接口、统一接口、包装现有类
- **核心优点**：解耦、复用、灵活
- **实现要点**：优先使用对象适配器、保持透明、考虑模板
- **常见陷阱**：适配器过多、循环依赖、资源泄露

> **记住**：适配器的关键是"翻译接口"——让不兼容的接口能够协同工作，而不需要修改原始代码。
