#include <vector>
#include <thread>
#include <queue>
#include <mutex>
#include <functional>
#include <condition_variable>

namespace tiny_http_server {

class ThreadPool {
public:
    explicit ThreadPool(size_t thread_count);
    ~ThreadPool();

    void enqueue(std::function<void()> task);

private:
    std::vector<std::thread> workers_{};
    std::queue<std::function<void()>> tasks_{};
    std::mutex mutex_{};
    std::condition_variable condition_{};

    bool stop_{false};
};

} // namespace tiny_http_server