#include <iostream>
#include <thread>
#include <chrono>
#include <mutex>
#include <random>
#include <vector>
#include <atomic>
#include <functional>

std::atomic<bool> running(true);

void f1() {
    std::cout << "Я функція f1 зараз працюю\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
}

void f2() {
    std::cout << "Я функція f2 зараз працюю\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
}

void f3() {
    std::cout << "Я функція f3 зараз працюю\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
}

void f4() {
    std::cout << "Я функція f4 зараз працюю\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(rand() % 100));
}
struct Counters {
    int f1_count = 0;
    int f2_count = 0;
    int f3_count = 0;
    int f4_count = 0;
    std::mutex mutex;
};
void proxy_f1(Counters& counters) {
    {
        std::lock_guard<std::mutex> lock(counters.mutex);
        counters.f1_count++;
    }
    f1();
}

void proxy_f2(Counters& counters) {
    {
        std::lock_guard<std::mutex> lock(counters.mutex);
        counters.f2_count++;
    }
    f2();
}

void proxy_f3(Counters& counters) {
    {
        std::lock_guard<std::mutex> lock(counters.mutex);
        counters.f3_count++;
    }
    f3();
}

void proxy_f4(Counters& counters) {
    {
        std::lock_guard<std::mutex> lock(counters.mutex);
        counters.f4_count++;
    }
    f4();
}

void thread_function(Counters& counters) {
    std::vector<std::function<void()>> functions = {
        [&]() { proxy_f1(counters); },
        [&]() { proxy_f2(counters); },
        [&]() { proxy_f3(counters); },
        [&]() { proxy_f4(counters); }
    };

    while (running) {
        int index = rand() % functions.size();
        functions[index]();
    }
}

void worker(int duration, int num_threads) {
    Counters counters;
    std::vector<std::thread> threads;

    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(thread_function, std::ref(counters));
    }

    std::this_thread::sleep_for(std::chrono::seconds(duration));
    running = false;

    for (auto& thread : threads) {
        thread.join();
    }

    std::cout << "f1 was called " << counters.f1_count << " times\n";
    std::cout << "f2 was called " << counters.f2_count << " times\n";
    std::cout << "f3 was called " << counters.f3_count << " times\n";
    std::cout << "f4 was called " << counters.f4_count << " times\n";
}
int main() {
    int num_threads, duration;

    std::cout << "Введіть кількість потоків: ";
    std::cin >> num_threads;

    std::cout << "Введіть тривалість роботи програми в секундах: ";
    std::cin >> duration;

    worker(duration, num_threads);

    return 0;
}