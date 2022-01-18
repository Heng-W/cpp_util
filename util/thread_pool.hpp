#ifndef UTIL_THREAD_POOL_HPP
#define UTIL_THREAD_POOL_HPP

#include <assert.h>
#include <functional>
#include <future>
#include <vector>
#include <queue>
#include "common.h"

namespace util
{

class ThreadPool
{
public:
    using Task = std::function<void()>;

    explicit ThreadPool()
        : running_(false)
    {
    }

    ~ThreadPool()
    {
        if (running_) stop();
    }

    DISALLOW_COPY_AND_ASSIGN(ThreadPool);

    template <class Fn, class... Args>
    auto commit(Fn&& fn, Args&& ... args) ->std::future<decltype(fn(args...))>;

    void start(int numThreads);
    void stop();

    int numThreads() const { return threads_.size(); }

private:
    mutable std::mutex mutex_;
    std::condition_variable cvTask_;

    std::vector<std::thread> threads_;
    std::queue<Task> tasks_;     //任务队列
    bool running_;
};


template <class Fn, class... Args>
auto ThreadPool::commit(Fn&& fn, Args&& ... args) ->std::future<decltype(fn(args...))>
{
    using RetType = decltype(fn(args...));
    auto task = std::make_shared<std::packaged_task<RetType()>>(
                    std::bind(std::forward<Fn>(fn), std::forward<Args>(args)...));
    auto future = task->get_future();
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!running_) throw std::runtime_error("ThreadPool is stopped");
        tasks_.emplace([task] { (*task)(); });   
    }
    cvTask_.notify_one();
    return future;
}

inline void ThreadPool::start(int numThreads)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(numThreads);
    for (int i = 0; i < numThreads; ++i)
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
                        return !tasks_.empty() || !running_;
                    });
                    if (tasks_.empty() && !running_) return;
                    task = std::move(tasks_.front()); // 获取一个task
                    tasks_.pop();
                }
                if (task) task(); // 执行任务
            }
        });
    }
}

inline void ThreadPool::stop()
{
    {
        std::lock_guard<std::mutex> lock(mutex_);
        running_ = false;
        cvTask_.notify_all(); // 唤醒所有线程
    }
    for (auto& thread : threads_)
    {
        if (thread.joinable()) thread.join();
    }
}


} // namespace util

#endif // UTIL_THREAD_POOL_HPP
