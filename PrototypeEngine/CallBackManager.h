#pragma once
#include "Typedefs.h"
#include "StandardLibrary.h"

/*
* ===エンジン内部処理/Engine internal processing===
*/

//コールバック処理をカプセル化したもの
//C#のevent Actionを元に作成
// C++のstd::functionを使用している
template<typename Signature>
class Event;

template<typename Ret, typename... Args>
class Event<Ret(Args...)> 
{
public:
    using Handler = std::function<Ret(Args...)>;
private:
    std::vector<Handler> handlers;
public:

    void Add(const Handler& handler) {
        handlers.push_back(handler);
    }

    void Invoke(Args... args) {
        for (auto& handler : handlers) {
            handler(args...);
        }
    }

};

struct WaitForSeconds {
    std::chrono::milliseconds duration;
    bool await_ready() const noexcept { return false; }
    void await_suspend(std::coroutine_handle<> handle) const {
        std::thread([handle, d = duration]() {
            std::this_thread::sleep_for(d);
            handle.resume();
            }).detach();
    }
    void await_resume() const noexcept {}
};

struct Coroutine {
    struct promise_type {
        Coroutine get_return_object() { return {}; }
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() { std::terminate(); }
    };
};