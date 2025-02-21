#include <chrono>
#include <coroutine>
#include "debug.hpp"

struct Promise {

    std::coroutine_handle<Promise> get_return_object() {
        return std::coroutine_handle<Promise>::from_promise(*this);
    }

    auto initial_suspend() {
        return std::suspend_always{};
    }

    void return_void() {
        mRetValue_ = 0;
    }

    void unhandled_exception() {
        throw ;
    }

    auto final_suspend() noexcept {
        return std::suspend_always{};
    }
    int mRetValue_;
};

struct Task {
    using promise_type = Promise;

    Task(std::coroutine_handle<promise_type> coroutine)
        : mCoroutine(coroutine) {}

    std::coroutine_handle<promise_type> mCoroutine;
};

Task hello() {
    // Task::promise_type promise;
    // Task ret = promise.get_return_object();
    // co_await promise.initial_suspend();      //->suspend_always{} or -> suspend_never{}
    // co_await some will call some.await_ready(), if it is false then call some.await_suspend(coroutine) and suspend, await_resume() return a value 

    debug{}, "hello";
    co_return;     // calls promise.return_void()
    // destroy all variables with automatic storage duration in reverse order
    // co_await promise.final_suspend()
}

int main() {
    debug{}, "main will call hello()";
    Task t = hello();
    debug(), "call hello()";
    t.mCoroutine.resume();
    debug(), "hello() ends";
}