#ifndef UTIL_BOUNDED_BLOCKING_QUEUE_HPP
#define UTIL_BOUNDED_BLOCKING_QUEUE_HPP

#include <condition_variable>
#include "circular_queue.hpp"

namespace util
{

template <class T>
class BoundedBlockingQueue
{
public:
    explicit BoundedBlockingQueue(int maxSize): queue_(maxSize) {}

    void put(const T& x)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] { return !queue_.full(); });
        queue_.push(x);
        notEmpty_.notify_one();
    }

    void put(T&& x)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        notFull_.wait(lock, [this] { return !queue_.full(); });
        queue_.push(std::move(x));
        notEmpty_.notify_one();
    }

    T get()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        notEmpty_.wait(lock, [this] { return !queue_.empty(); });
        T front(std::move(queue_.front()));
        queue_.pop();
        notFull_.notify_one();
        return front;
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    bool full() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.full();
    }

    size_t size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.size();
    }

    size_t capacity() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return queue_.capacity();
    }

private:
    mutable std::mutex mutex_;
    std::condition_variable notEmpty_;
    std::condition_variable notFull_;
    CircularQueue<T> queue_;
};

} // namespace util

#endif // UTIL_BOUNDED_BLOCKING_QUEUE_HPP
