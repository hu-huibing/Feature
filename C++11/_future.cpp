#include <future>
#include <thread>
#include <iostream>

template <typename Fun, typename... Args>
auto async_call(Fun fun, Args... args)
    ->std::future<typename std::result_of<Fun(Args ...)>::type>
{
    using res_type = typename std::result_of<Fun(Args ...)>::type;
    std::packaged_task<res_type(Args...)> task(fun);
    auto future = task.get_future();
    std::thread t(std::move(task), args...);
    t.detach();
    return future;
}

/*----test----*/
int main(void)
{
    std::cout << "async call a + b" << std::endl;
    auto future = async_call([](int a, int b){
        std::cout << "thread start" << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        return a + b;
    }, 1, 2);
    std::cout << "do something ... " << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "result: " << future.get() << std::endl;
    return 0;
}