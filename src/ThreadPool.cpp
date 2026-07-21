#include "ThreadPool.h"
#include <iostream>  


ThreadPool::ThreadPool(size_t n) : stop(false) {
    if (n == 0) {
        throw std::invalid_argument("ThreadPool: numThreads must be > 0");
    }
    for (size_t i = 0; i < n; ++i) {
        threads.emplace_back(&ThreadPool::worker, this);
    }
}


ThreadPool::~ThreadPool() {
    {
        std::lock_guard<std::mutex> lock(queueMutex);
        stop = true;
    }
    condition.notify_all();
    for (std::thread& th : threads) {
        if (th.joinable()) {
            th.join();
        }
    }
}


void ThreadPool::worker() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            condition.wait(lock, [this] {
                return !tasks.empty() || stop;
                });
            if (stop && tasks.empty()) {
                return;
            }
            task = std::move(tasks.front());
            tasks.pop();
        }
        try {
            task();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "unknown error" << std::endl;
        }
    }
}