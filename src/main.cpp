#include "ThreadPool.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    ThreadPool pool(4);
    for (int i = 0; i < 8; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        pool.enqueue([i]() {
            
            std::cout << "Task " << i << " executed by thread " << std::this_thread::get_id() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
            });
    }


    std::this_thread::sleep_for(std::chrono::seconds(2));
    return 0;
}