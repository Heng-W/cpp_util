#ifndef _UNLOCK_QUEUE_H_
#define _UNLOCK_QUEUE_H_


//refer kfifo
class UnlockQueue
{
public:
    UnlockQueue(unsigned int  isize);
    ~UnlockQueue();
    unsigned int get(unsigned char* buffer, unsigned int len);
    unsigned int put(const unsigned char* buffer, unsigned int len);
    static unsigned long roundup_power_of_two(unsigned long val);
private:
    inline bool is_power_of_2(unsigned long n)
    {
        return (n != 0 && ((n & (n - 1)) == 0));
    };
    inline unsigned int unused()
    {
        return (mask + 1) - (in - out);
    }

private:
    unsigned int size;    //缓冲区空间的大小，圆整为2的次幂
    unsigned int in;     //指向buffer中队头
    unsigned int out;    //指向buffer中的队尾
    unsigned int mask;
    unsigned char* buffer;    //存放数据的缓存
};

struct proto
{
    unsigned int msgid;
    unsigned int cmd;
    unsigned int info;
    proto(): msgid(0), cmd(0), info(0) {}
};

#endif
