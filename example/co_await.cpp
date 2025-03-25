#include <coroutine>
#include <iostream>

struct PreAwaiter {
    bool await_ready() const noexcept { return false; }

    std::coroutine_handle<>
    await_suspend(std::coroutine_handle<> handle) const noexcept {
        std::cout << "PreAwaiter await_suspend" << std::endl;
        if (m_handle) { return m_handle; }
        return std::noop_coroutine();
    }

    void await_resume() const noexcept {}
    std::coroutine_handle<> m_handle;
};

struct Promise {
    auto initial_suspend() noexcept { return std::suspend_always{}; }
    auto final_suspend() noexcept { return PreAwaiter{m_pre}; }
    void unhandled_exception() {}
    auto get_return_object() {
        return std::coroutine_handle<Promise>::from_promise(*this);
    }

    void return_value(int val) {
        m_val = val;
        std::cout << "return_value: " << val << std::endl;
    }

    int m_val;
    std::coroutine_handle<> m_pre;
};

struct Awaiter {
    bool await_ready() const noexcept { return false; }

    auto await_suspend(std::coroutine_handle<> handle) const noexcept {
        std::cout << "await_suspend" << std::endl;
        m_handle.promise().m_pre = handle;
        return m_handle;
    }

    int await_resume() const noexcept { return m_handle.promise().m_val; }
    std::coroutine_handle<Promise> m_handle;
};

struct Task : std::coroutine_handle<Promise> {
    using promise_type = Promise;
    Task(std::coroutine_handle<promise_type> handle)
        : std::coroutine_handle<Promise>(handle) {}

    auto operator co_await() {
        return Awaiter{*static_cast<std::coroutine_handle<Promise> *>(this)};
    }
};

Task world() {
    std::cout << "world" << std::endl;
    co_return 1;
}

Task hello() {
    std::cout << "hello" << std::endl;
    auto i = co_await world();
    co_return i + 1;
}

int main() {
    auto t = hello();
    std::cout << "ready to resume: " << t.promise().m_val << std::endl;
    while (!t.done()) { t.resume(); }
}