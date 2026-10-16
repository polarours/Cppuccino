# Adapter Pattern

## What Is the Adapter Pattern?

The adapter pattern is a structural design pattern that converts the interface
of one class into the interface a client expects. Adapters let incompatible
interfaces work together.

> **Core idea**: wrap an incompatible interface with an adapter so the pieces
> can collaborate.

## Why Do We Need It?

### Problem Scenario

Suppose a drawing program must support several configuration formats:

```cpp
#include <string>

// existing interface: an XML parser
class XMLParser {
public:
    void parse(const std::string& xml);
    std::string getValue(const std::string& key);
};

// target interface: a config reader
class ConfigReader {
public:
    virtual std::string get(const std::string& key) = 0;
    virtual ~ConfigReader() = default;
};

// problem: XMLParser's interface does not match ConfigReader's
```

**The mismatch**:

1. `XMLParser` offers `parse()` + `getValue()`
2. `ConfigReader` expects `get()`
3. `XMLParser` cannot be used where a `ConfigReader` is required

### Solution

Use an adapter:

```cpp
#include <memory>
#include <string>

class XMLParser {
public:
    void parse(const std::string& xml);
    std::string getValue(const std::string& key);
};

class ConfigReader {
public:
    virtual std::string get(const std::string& key) = 0;
    virtual ~ConfigReader() = default;
};

// adapter: makes an XMLParser look like a ConfigReader
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

void demo() {
    auto adapter = std::make_unique<XMLParserAdapter>(
        std::make_unique<XMLParser>()
    );
    std::string value = adapter->get("key");
}
```

**Benefits**:

1. Decoupling: clients need not know the concrete implementation
2. Reuse: existing incompatible classes become usable again
3. Flexibility: adapters can be swapped at runtime

## Pattern Structure

```
                    ┌─────────────┐
                    │  Target     │ ← the interface the client wants
                    │  (ConfigReader)│
                    └──────┬──────┘
                           │
              ┌────────────▼────────────┐
              │     Adapter             │ ← the adapter
              │  (XMLParserAdapter)     │
              └────────────┬────────────┘
                           │ adapts
                           │
              ┌────────────▼────────────┐
              │      Adaptee            │ ← the class being adapted
              │     (XMLParser)         │
              └─────────────────────────┘
```

## Implementation

### 1. The Target Interface

```cpp
#include <memory>
#include <string>
#include <iostream>

// target interface: a media player
class MediaPlayer {
public:
    virtual void play(const std::string& format, const std::string& filename) = 0;
    virtual ~MediaPlayer() = default;
};
```

### 2. The Classes Being Adapted

```cpp
#include <string>
#include <iostream>

// existing class: a VLC player (vlc only)
class VLCPlayer {
public:
    void playVlc(const std::string& file) {
        std::cout << "Playing VLC file: " << file << "\n";
    }
};

// existing class: an MP3 player (mp3 only)
class MP3Player {
public:
    void playMp3(const std::string& file) {
        std::cout << "Playing MP3 file: " << file << "\n";
    }
};

// existing class: an MP4 player (mp4 only)
class MP4Player {
public:
    void playMp4(const std::string& file) {
        std::cout << "Playing MP4 file: " << file << "\n";
    }
};
```

### 3. Implementing the Adapters

```cpp
#include <memory>
#include <string>
#include <iostream>

class MediaPlayer {
public:
    virtual void play(const std::string& format, const std::string& filename) = 0;
    virtual ~MediaPlayer() = default;
};

class VLCPlayer {
public:
    void playVlc(const std::string& file);
};

class MP3Player {
public:
    void playMp3(const std::string& file);
};

class MP4Player {
public:
    void playMp4(const std::string& file);
};

// adapter: makes a VLCPlayer look like a MediaPlayer
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

// adapter: makes an MP3Player look like a MediaPlayer
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

// adapter: makes an MP4Player look like a MediaPlayer
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

### 4. Client Usage

```cpp
#include <memory>
#include <iostream>
#include <string>

class MediaPlayer {
public:
    virtual void play(const std::string& format, const std::string& filename) = 0;
    virtual ~MediaPlayer() = default;
};

class VLCPlayer { public: void playVlc(const std::string& f) { std::cout << f << "\n"; } };
class MP3Player { public: void playMp3(const std::string& f) { std::cout << f << "\n"; } };
class MP4Player { public: void playMp4(const std::string& f) { std::cout << f << "\n"; } };

class VLCAdapter : public MediaPlayer {
public:
    explicit VLCAdapter(std::unique_ptr<VLCPlayer> p) : player_(std::move(p)) {}
    void play(const std::string& format, const std::string& filename) override {
        if (format == "vlc") player_->playVlc(filename);
    }
private:
    std::unique_ptr<VLCPlayer> player_;
};

class MP3Adapter : public MediaPlayer {
public:
    explicit MP3Adapter(std::unique_ptr<MP3Player> p) : player_(std::move(p)) {}
    void play(const std::string& format, const std::string& filename) override {
        if (format == "mp3") player_->playMp3(filename);
    }
private:
    std::unique_ptr<MP3Player> player_;
};

class MP4Adapter : public MediaPlayer {
public:
    explicit MP4Adapter(std::unique_ptr<MP4Player> p) : player_(std::move(p)) {}
    void play(const std::string& format, const std::string& filename) override {
        if (format == "mp4") player_->playMp4(filename);
    }
private:
    std::unique_ptr<MP4Player> player_;
};

int main() {
    // create the players
    auto vlc = std::make_unique<VLCPlayer>();
    auto mp3 = std::make_unique<MP3Player>();
    auto mp4 = std::make_unique<MP4Player>();

    // wrap each one in its adapter
    std::unique_ptr<MediaPlayer> player = std::make_unique<VLCAdapter>(std::move(vlc));
    player->play("vlc", "movie.vlc");

    player = std::make_unique<MP3Adapter>(std::move(mp3));
    player->play("mp3", "song.mp3");

    player = std::make_unique<MP4Adapter>(std::move(mp4));
    player->play("mp4", "video.mp4");

    return 0;
}
```

## Variants and Extensions

### Variant 1: Class Adapter (Multiple Inheritance)

```cpp
#include <string>

class MediaPlayer {
public:
    virtual void play(const std::string& format, const std::string& filename) = 0;
    virtual ~MediaPlayer() = default;
};

class VLCPlayer {
public:
    void playVlc(const std::string& file) {}
};

// class adapter: implemented through multiple inheritance
class VLCClassAdapter : public MediaPlayer, public VLCPlayer {
public:
    void play(const std::string& format, const std::string& filename) override {
        if (format == "vlc") {
            playVlc(filename);  // VLCPlayer's method directly
        }
    }
};
```

**Note**: in C++ a class adapter needs multiple inheritance and is less
flexible than an object adapter.

### Variant 2: Two-Way Adapter

```cpp
struct AudioData { int sample = 0; };

struct AudioInput {
    virtual ~AudioInput() = default;
    virtual AudioData capture() = 0;
};

struct AudioOutput {
    virtual ~AudioOutput() = default;
    virtual void process(const AudioData& data) = 0;
};

// two-way adapter: adapts both interfaces at once
class SoundConverter : public AudioInput, public AudioOutput {
public:
    SoundConverter(AudioInput& in, AudioOutput& out) : input_(in), output_(out) {}

    // as an AudioOutput: forward what arrives to the paired output
    void process(const AudioData& data) override {
        output_.process(data);
    }

    // as an AudioInput: pull from the paired input
    AudioData capture() override {
        return input_.capture();
    }

private:
    AudioInput& input_;     // references: both interfaces are abstract
    AudioOutput& output_;
};
```

### Variant 3: Adapter Factory

```cpp
#include <memory>
#include <stdexcept>
#include <string>

class MediaPlayer {
public:
    virtual void play(const std::string& format, const std::string& filename) = 0;
    virtual ~MediaPlayer() = default;
};

class VLCPlayer { public: void playVlc(const std::string&) {} };
class MP3Player { public: void playMp3(const std::string&) {} };
class MP4Player { public: void playMp4(const std::string&) {} };

class VLCAdapter : public MediaPlayer {
public:
    explicit VLCAdapter(std::unique_ptr<VLCPlayer> p) : player_(std::move(p)) {}
    void play(const std::string& format, const std::string& filename) override {
        if (format == "vlc") player_->playVlc(filename);
    }
private:
    std::unique_ptr<VLCPlayer> player_;
};

class MP3Adapter : public MediaPlayer {
public:
    explicit MP3Adapter(std::unique_ptr<MP3Player> p) : player_(std::move(p)) {}
    void play(const std::string& format, const std::string& filename) override {
        if (format == "mp3") player_->playMp3(filename);
    }
private:
    std::unique_ptr<MP3Player> player_;
};

class MP4Adapter : public MediaPlayer {
public:
    explicit MP4Adapter(std::unique_ptr<MP4Player> p) : player_(std::move(p)) {}
    void play(const std::string& format, const std::string& filename) override {
        if (format == "mp4") player_->playMp4(filename);
    }
private:
    std::unique_ptr<MP4Player> player_;
};

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

void demo() {
    auto player = AdapterFactory::create("mp3");
    player->play("mp3", "song.mp3");
}
```

## How It Differs from Related Patterns

| Pattern | Difference |
|---------|-----------|
| **Bridge** | Bridge separates abstraction from implementation; adapter converts an interface |
| **Decorator** | Decorator adds behavior; adapter converts an interface |
| **Proxy** | Proxy controls access; adapter converts an interface |

## Best Practices

### 1. Prefer the Object Adapter

```cpp
#include <memory>

struct Target { virtual ~Target() = default; };
struct Adaptee {};

// recommended: object adapter (composition)
class ObjectAdapter : public Target {
public:
    explicit ObjectAdapter(std::unique_ptr<Adaptee> adaptee)
        : adaptee_(std::move(adaptee)) {}

private:
    std::unique_ptr<Adaptee> adaptee_;
};

// not recommended: class adapter (multiple inheritance)
// class ClassAdapter : public Target, public Adaptee { /* ... */ };
```

### 2. Adapters Should Be Transparent

```cpp
#include <memory>
#include <string>

struct ConfigReader {
    virtual ~ConfigReader() = default;
    virtual std::string get(const std::string& key) = 0;
};

struct XmlParser {
    std::string getValue(const std::string& key) { return key; }
};

// recommended: the adapter hides its internals from the outside
class ConfigAdapter : public ConfigReader {
public:
    explicit ConfigAdapter(std::unique_ptr<XmlParser> parser)
        : parser_(std::move(parser)) {}

    std::string get(const std::string& key) override {
        // the conversion stays inside the adapter
        return parser_->getValue(key);
    }

private:
    std::unique_ptr<XmlParser> parser_;
};
```

### 3. Consider a Template Adapter

```cpp
#include <memory>
#include <string>

struct Target { virtual ~Target() = default; };

struct VLCPlayer {
    std::string adaptedProcess() { return "vlc"; }
};

template <typename Adaptee>
class TemplateAdapter : public Target {
public:
    explicit TemplateAdapter(std::unique_ptr<Adaptee> adaptee)
        : adaptee_(std::move(adaptee)) {}

    std::string process() {
        return adaptee_->adaptedProcess();
    }

private:
    std::unique_ptr<Adaptee> adaptee_;
};

void demo() {
    auto adapter = std::make_unique<TemplateAdapter<VLCPlayer>>(
        std::make_unique<VLCPlayer>()
    );
    (void)adapter;
}
```

## Common Traps

### Trap 1: Too Many Adapters

```cpp
#include <string>

// the trap: a pile of adapters that only increases the complexity
// class JSONtoXMLAdapter { /* ... */ };
// class XMLtoJSONAdapter { /* ... */ };
// class CSVtoXMLAdapter  { /* ... */ };
// class XMLtoCSVAdapter  { /* ... */ };

// better: unify on one interface, or reach for a serialization library
struct FormatConverter {
    virtual ~FormatConverter() = default;
    virtual std::string convert(const std::string& input) = 0;
};
```

### Trap 2: Circular Dependencies Between Adapters

```cpp
#include <memory>

// the trap: two adapters that depend on each other
// (sketch — this shape does not compile as written)
// class AdapterA : public TargetA {
//     void process() override { adapterB_->convert(data_); }  // needs B
//     std::unique_ptr<AdapterB> adapterB_;
// };
// class AdapterB : public TargetB {
//     void process() override { adapterA_->convert(data_); }  // needs A
//     std::unique_ptr<AdapterA> adapterA_;
// };
//
// fix: depend on an interface both can implement, or invert one direction

struct TargetA { virtual ~TargetA() = default; };
struct TargetB { virtual ~TargetB() = default; };
```

### Trap 3: Adapters Leaking Resources

```cpp
#include <memory>

struct Target { virtual ~Target() = default; };
struct Resource { int id = 0; };

Resource* allocate() { return new Resource{}; }

// the trap: the adapter manages a resource but never releases it
class BadAdapter : public Target {
public:
    void process() {
        resource_ = allocate();  // leaked on every call and at destruction
    }
    Resource* resource_ = nullptr;
};

// recommended: use a smart pointer
class GoodAdapter : public Target {
public:
    void process() {
        resource_ = std::make_unique<Resource>();
        // released automatically
    }
    std::unique_ptr<Resource> resource_;
};
```

## Summary

The core points of the adapter pattern:

- **When to use it**: reuse incompatible interfaces, unify interfaces, wrap
  existing classes
- **Main benefits**: decoupling, reuse, flexibility
- **Implementation advice**: prefer object adapters, stay transparent,
  consider templates
- **Common traps**: too many adapters, circular dependencies, leaked resources

> **Remember**: an adapter's job is "interface translation" — letting
> incompatible interfaces collaborate without touching the original code.
