
ucoro::awaitable<int> coro_compute_int(int value)
{
    auto ret = co_await callback_awaitable<int>([value](auto handle) {
        std::cout << value << " value\n";
        handle(value * 100);
    });

    co_return (value + ret);
}

ucoro::awaitable<void> coro_compute_exec(int value)
{
    auto comput_promise = coro_compute_int(value);

    auto ret = co_await std::move(comput_promise);
    std::cout << "return: " << ret << std::endl;
    co_return;
}

ucoro::awaitable<void> coro_compute()
{
    for (auto i = 0; i < 100; i++)
    {
        co_await coro_compute_exec(i);
    }
}

int main(int argc, char **argv)
{
    coro_compute().detach();

    return 0;
}