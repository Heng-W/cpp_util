#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <assert.h>
#include <functional>
#include <future>
#include <vector>
#include <queue>

#include "noncopyable.h"

namespace util
{

class ThreadPool : noncopyable
{
public:
    using Task = std::function<void()>;

    explicit ThreadPool(int threadNum = -1)
        : runFlag_(false)
    {
        if (threadNum > 0)
            start(threadNum);
    }

    ~ThreadPool()
    {
        if (runFlag_)
            stop();
    }

    template <class Fn, class... Args>
    auto commit(Fn&& fn, Args&& ... args) ->std::future<decltype(fn(args...))>;

    void start(int threadNum);
    void stop();

    int threadNum() const { return threads_.size(); }

private:
    mutable std::mutex mutex_;
    std::condition_variable cvTask_;

    std::vector<std::thread> threads_;
    std::queue<Task> tasks_;     //任务队列
    bool runFlag_;
};


template <class Fn, class... Args>
auto ThreadPool::commit(Fn&& fn, Args&& ... args) ->std::future<decltype(fn(args...))>
{
    using RetType = decltype(fn(args...));
    auto task = std::make_shared<std::packaged_task<RetType()>>(
                    std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!runFlag_)
            throw std::runtime_error("ThreadPool is stopped");
        tasks_.emplace([task]() { (*task)(); });
        cvTask_.notify_one();
    }
    return task->get_future();
}

inline void ThreadPool::start(int threadNum)
{
    if (!threads_.empty())
        throw std::runtime_error("ThreadPool has started");
    runFlag_ = true;
    threads_.reserve(threadNum);
    for (int i = 0; i < threadNum ; ++i)
    {
        threads_.emplace_back([this]
        {
            while (true)
            {
                Task task;
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    cvTask_.wait(lock, [this]
                    {
                        return !tasks_.empty() || !runFlag_;
                    }); //等待task
                    if (tasks_.empty() && !runFlag_)
                        return;
                    task = std::move(tasks_.front());//获取一个task
                    tasks_.pop();
                }
                if (task) task();//执行任务
            }
        });
    }
}

inline void ThreadPool::stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        runFlag_ = false;
        cvTask_.notify_all(); // 唤醒所有线程
    }
    for (auto& thread : threads_)
    {
        if (thread.joinable())
            thread.join();
    }
}


} //namespace util

#endif //THREAD_POOL_HPP
