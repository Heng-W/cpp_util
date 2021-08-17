
#include "string_helper.h"
#include <regex>
#include <iostream>

using namespace std;


namespace util
{

vector<string> splitString(const string& in, const string& delim)
{
    vector<string> ret;
    try
    {
        regex re{delim};
        return vector<string>
        {
            sregex_token_iterator(in.begin(), in.end(), re, -1),
            sregex_token_iterator()
        };
    }
    catch (const std::exception& e)
    {
        cout << "error:" << e.what() << endl;
    }
    return ret;
}

}


hash_t hash_(char const* str)
{
    hash_t ret{HASH_BASIS};

    while (*str)
    {
        ret ^= *str;
        ret *= HASH_PRIME;
        str++;
    }

    return ret;
}

