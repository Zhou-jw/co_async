#include <coroutine>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <fstream>

inline void log(const std::string &msg) {
    std::cout << std::this_thread::get_id() << " " << msg << std::endl;
}

struct Promise;
struct Coroutine : std::coroutine_handle<Promise> {
    using promise_type = ::Promise;
};

struct Promise {
    Coroutine get_return_object() { return {Coroutine::from_promise(*this)}; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    void return_void() {}
    // void return_value(auto result) {
    //     log("return_result:" + std::to_string(result));
    // }
    void unhandled_exception() {}

    std::vector<std::string> lines;
};

struct Awaiter {
    std::string filename_;
    std::coroutine_handle<Promise> handle_;

    Awaiter(const std::string &filename) : filename_(filename) {}

    // 返回 false 代表 awaiter 没有准备好，co_await 会挂起当前协程
    constexpr bool await_ready() const noexcept { return false; }

    void await_suspend(std::coroutine_handle<Promise> handle) {
        handle_ = handle; // [1]
        std::thread t{[filename{std::move(filename_)}, handle] {
            std::ifstream in{filename};
            std::vector<std::string> lines;
            while(in) {
                std::string line;
                std::getline(in, line);
                lines.emplace_back(std::move(line));
            }
            //Ignore the end of file
            if(!lines.empty() && lines.back().empty()) {
                lines.pop_back();
            }

            in.close();
            log("Finished reading "+ filename);
            handle.promise().lines = std::move(lines); // [2]
            handle.resume(); // [3]
        }};
        t.join();
    }

    std::vector<std::string> await_resume() const noexcept {
        return std::move(handle_.promise().lines);
    }
};
// Coroutine f(std::string &&s, char ch) {
//     log("f " + s + ", " + ch);
//     co_return s.find(ch);
// }

Coroutine f() {
  const std::string filename = "/home/code/co_async/example/first_coroutine.cpp";
  log("Before co_await");
  auto lines = co_await Awaiter{filename};
  log(std::to_string(lines.size()) + " lines in " + filename);
}

int main() {
    // auto h = f("hello", 'o');
    auto h = f();
    log("main 1");
    h.resume();
    log("main 2");
    h.destroy();
}