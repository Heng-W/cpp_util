#ifndef UTIL_RING_BUFFER_H
#define UTIL_RING_BUFFER_H

namespace util
{

// Refer to linux kernel source: kfifo
class RingBuffer
{
public:
    RingBuffer(unsigned int size);
    ~RingBuffer();
    
    unsigned int put(const unsigned char* buf, unsigned int len);
    unsigned int get(unsigned char* buf, unsigned int len);
    
private:
    
    unsigned int unused() const { return (mask_ + 1) - (in_ - out_); }

    unsigned int size_; // 缓冲区空间的大小，圆整为2的次幂
    unsigned int in_; // 指向buffer中队头
    unsigned int out_; // 指向buffer中的队尾
    unsigned int mask_;
    unsigned char* buffer_; // 数据
};

} // namespace util

#endif // UTIL_RING_BUFFER_H
