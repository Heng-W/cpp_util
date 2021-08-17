#ifndef _UTIL_TIME_HPP_
#define _UTIL_TIME_HPP_


#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <string>
#include <iostream>


namespace util
{

inline void sleepMs(unsigned long ms)
{
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    select(0, NULL, NULL, NULL, &tv);
}


inline timeval time()
{
    timeval tv;
    gettimeofday(&tv, NULL);
    return tv;
}


inline long long getDiffTime(const timeval& tStart, const timeval& cTime)
{
    long long tPassed = 0;

    tPassed = 1000000LL * (cTime.tv_sec - tStart.tv_sec) + (cTime.tv_usec - tStart.tv_usec);

    //tPassed /= 1000;

    return tPassed;

    //printf("Time = %lldms\n", tPassed);
}

inline void printRuntime(const timeval& tStart, const timeval& cTime, const std::string& info = "runtime")
{
    std::cout << info << ": " << getDiffTime(tStart, cTime) << " us" << std::endl;
}

inline void printRuntime(const timeval& tStart, const std::string& info = "runtime")
{
    printRuntime(tStart, time(), info);
}

struct TimeNode
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
    int sec;
    int usec;
};

inline void getTime(TimeNode& tm)
{
    struct timeval tv;

    gettimeofday(&tv, NULL); //获取1970-1-1到现在的时间结果保存到tv中

    uint64_t sec = tv.tv_sec;

    struct tm cur_tm;//保存转换后的时间结果

    localtime_r((time_t*)&sec, &cur_tm);

    tm.year = cur_tm.tm_year + 1900;
    tm.mon = cur_tm.tm_mon + 1;
    tm.day = cur_tm.tm_mday;

    tm.hour = cur_tm.tm_hour;
    tm.min = cur_tm.tm_min;
    tm.sec = cur_tm.tm_sec;
    tm.usec = tv.tv_usec;



}


inline std::string getDetailTime()
{

    TimeNode tm;

    getTime(tm);


    char cur_time[50];

    sprintf(cur_time, "%d%02d%02d_%02d%02d%02d_%06d",  tm.year, tm.mon, tm.day, tm.hour, tm.min, tm.sec, tm.usec);


    return std::string(cur_time);
}



inline std::string getCurrDate()
{

    TimeNode tm;

    getTime(tm);

    char cur_date[20];

    sprintf(cur_date, "%d-%02d-%02d", tm.year, tm.mon, tm.day);


    return std::string(cur_date);
}

inline std::string getCurrTime()
{

    TimeNode tm;

    getTime(tm);

    char cur_time[20];

    sprintf(cur_time, "%02d:%02d:%02d", tm.hour, tm.min, tm.sec);


    return std::string(cur_time);
}

}

#endif

