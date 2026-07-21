#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>
#include <stdexcept>

class ThreadPool {
public:
    
    ThreadPool(size_t n);

    
    template <typename F>
    void enqueue(F&& task);

    
    ~ThreadPool();

private:
    
    void worker();

    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex queueMutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
};


template <typename F>
void ThreadPool::enqueue(F&& task) {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (stop) {
            throw std::runtime_error("ThreadPool is stopped");
        }
        tasks.push(std::forward<F>(task));
    }
    condition.notify_one();
}