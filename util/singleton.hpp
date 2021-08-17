#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

#include <mutex>

template <class T>
class Singleton
{
private:
    Singleton() = default;
    ~Singleton() = default;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

public:
    template <typename... Args>
    static T* instance(Args&& ... args)
    {
        std::call_once(flag_, [&]()
        {
            instance_ = new T(std::forward<Args>(args)...);
        });
        return instance_;
    }

    static void destroy()
    {
        if (instance_)
        {
            delete instance_;
            instance_ = NULL;
        }
    }

private:
    static T* instance_;
    static std::once_flag flag_;
};

template <class T>
T* Singleton<T>::instance_ = NULL;

template <class T>
std::once_flag Singleton<T>::flag_;


#endif
