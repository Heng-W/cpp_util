#ifndef PTR_HELPER_HPP
#define PTR_HELPER_HPP

#include <memory>

template <typename T, typename... Ts>
inline std::unique_ptr<T> make_unique(Ts&& ... params)
{
    return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

#endif // PTR_HELPER_HPP
