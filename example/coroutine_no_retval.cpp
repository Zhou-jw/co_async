#include <coroutine>
#include <iostream>

struct Promise {
    auto initial_suspend() noexcept {
        std::cout << "initial_suspend" << std::endl;
        return std::suspend_always{};
    }
    auto final_suspend() noexcept {
        std::cout << "final_suspend" << std::endl;

        return std::suspend_always{};
    }
    void unhandled_exception() {}
    auto get_return_object() {
        std::cout << "get return object" << std::endl;
        return std::coroutine_handle<Promise>::from_promise(*this);
    }

    void return_void() { std::cout << "Hello Coroutine!" << std::endl; }
};

struct Task : std::coroutine_handle<Promise> {
    using promise_type = Promise;
    Task(std::coroutine_handle<promise_type> handle)
        : std::coroutine_handle<Promise>(handle) {}
};

Task hello() {
    std::cout << "hello()" << std::endl;
    co_return;
}

int main() {
    auto t = hello();
    std::cout << "ready to resume" << std::endl;
    t.resume();
}