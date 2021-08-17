#ifndef _SYS_CMD_H_
#define _SYS_CMD_H_

#include <string>

namespace util
{

//解析系统命令
int system(const std::string& cmd, std::string& result);

inline std::string system(const std::string& cmd)
{
    std::string result;
    system(cmd, result);
    return result;
}

}

#endif

