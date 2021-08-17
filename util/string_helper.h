#ifndef _STRING_HELPER_HPP_
#define _STRING_HELPER_HPP_

#include <string>
#include <vector>


namespace util
{
std::vector<std::string> splitString(const std::string& in, const std::string& delim);
}

typedef std::uint64_t hash_t;

constexpr hash_t HASH_PRIME = 0x100000001B3ull;
constexpr hash_t HASH_BASIS = 0xCBF29CE484222325ull;


hash_t hash_(char const* str);

constexpr hash_t hash_compile_time(char const* str, hash_t last_value = HASH_BASIS)
{
    return *str ? hash_compile_time(str + 1, (*str ^ last_value) * HASH_PRIME) : last_value;
}

constexpr unsigned long long operator "" _hash(char const* p, size_t)
{
    return hash_compile_time(p);
}


#endif

