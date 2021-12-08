#ifndef UTIL_CIRCULAR_QUEUE_HPP
#define UTIL_CIRCULAR_QUEUE_HPP

#include <assert.h>
#include <memory>

namespace util
{

// 数组实现的环形队列
template <class T>
class CircularQueue
{
public:
    explicit CircularQueue(int capacity)
        : front_(0), rear_(0)
    {
        capacity_ = roundupPowerOfTwo(capacity);
        data_ = alloc_.allocate(capacity_);
    }

    ~CircularQueue() { free(); }

    // 拷贝构造函数
    CircularQueue(const CircularQueue& rhs)
        : front_(0), rear_(0), capacity_(rhs.capacity_)
    {
        data_ = alloc_.allocate(capacity_);
        unsigned int cur = rhs.front_;
        while (cur != rhs.rear_)
        {
            alloc_.construct(&data_[rear_++], rhs.data_[cur++]);
            cur &= rhs.capacity_ - 1;
        }
    }

    // 移动构造函数
    CircularQueue(CircularQueue&& rhs) noexcept
        : data_(rhs.data_),
          front_(rhs.front_),
          rear_(rhs.rear_),
          capacity_(rhs.capacity_)
    {
        rhs.data_ = nullptr;
        rhs.front_ = rhs.rear_ = rhs.capacity_ = 0;
    }

    // 拷贝赋值运算符
    CircularQueue& operator=(const CircularQueue& rhs)
    {
        CircularQueue copy = rhs;
        return *this = std::move(copy);
    }

    // 移动赋值运算符
    CircularQueue& operator=(CircularQueue&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            data_ = rhs.data_;
            front_ = rhs.front_;
            rear_ = rhs.rear_;
            capacity_ = rhs.capacity_;

            rhs.data_ = nullptr;
            rhs.front_ = rhs.rear_ = rhs.capacity_ = 0;
        }
        return *this;
    }

    // 压入
    void push(const T& x) { _push(x); }
    void push(T&& x) { _push(std::move(x)); }

    // 弹出
    void pop()
    {
        assert(!empty());
        alloc_.destroy(&data_[front_++]);
        front_ &= capacity_ - 1;
    }

    // 清空
    void clear()
    {
        unsigned int cur = front_;
        while (cur != rear_)
        {
            alloc_.destroy(&data_[cur++]);
            cur &= capacity_ - 1;
        }
        front_ = rear_ = 0;
    }

    const T& front() const { return data_[front_]; }

    // 元素数量
    int size() const { return (rear_ - front_ + capacity_) & (capacity_ - 1); }
    // 容量
    int capacity() const { return capacity_; }

    bool empty() const { return front_ == rear_; }
    bool full() const { return front_ == ((rear_ + 1) & (capacity_ - 1)); }

private:

    template <class X>
    void _push(X&& x)
    {
        assert(!full());
        alloc_.construct(&data_[rear_++], std::forward<X>(x));
        rear_ &= capacity_ - 1;
    }

    void free()
    {
        clear();
        if (data_) alloc_.deallocate(data_, capacity_);
    }

    // 取整为2的幂次方
    static unsigned int roundupPowerOfTwo(unsigned int i)
    {
        --i;
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return i + 1;
    }

    static std::allocator<T> alloc_;
    T* data_;
    unsigned int capacity_; // 容量
    unsigned int front_; // 队头索引
    unsigned int rear_; // 队尾索引
};

} // namespace util

#endif // UTIL_CIRCULAR_QUEUE_HPP
