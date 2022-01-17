#ifndef _SINGLETON_HPP_
#define _SINGLETON_HPP_

#include <mutex>

template <class T>
class Singleton
{
public:
    Singleton() = delete;
    ~Singleton() = delete;
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;

    template <typename... Args>
    static T* instance(Args&& ... args)
    {
        std::call_once(flag_, [&]
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
            instance_ = nullptr;
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
