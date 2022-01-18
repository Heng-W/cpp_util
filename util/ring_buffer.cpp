
#include "ring_buffer.h"

#include <stdlib.h>
#include <string.h>
#include <algorithm>

namespace util
{

static bool isPowerOfTwo(unsigned long n)
{ return n != 0 && (n & (n - 1)) == 0; }

static unsigned long roundupPowerOfTwo(unsigned long val)
{
    if (isPowerOfTwo(val)) return val;
    unsigned long maxulong = (unsigned long)((unsigned long) ~0);
    unsigned long andv = ~(maxulong & (maxulong >> 1));
    while ((andv & val) == 0)
        andv = andv >> 1;
    return andv << 1;
}

RingBuffer::RingBuffer(unsigned int size)
    : size_(roundupPowerOfTwo(size)), in_(0), out_(0), mask_(size_ - 1)
{
    buffer_ = (unsigned char*)malloc(size_);
}

RingBuffer::~RingBuffer()
{
    if (buffer_) free(buffer_);
}


unsigned int RingBuffer::put(const unsigned char* buf, unsigned int len)
{
    unsigned int l;
    len = std::min(len, size_ - in_ + out_);

    // Ensure that we sample the fifo->out index -before- we
    // start putting bytes into the kfifo.
    // smp_mb();
    __sync_synchronize();

    l = std::min(len, size_ - (in_ & (size_ - 1)));
    memcpy(buffer_ + (in_ & (size_ - 1)), buf, l);
    memcpy(buffer_, buf + l, len - l);

    // Ensure that we add the bytes to the kfifo -before-
    // we update the fifo->in index.
    // smp_wmb();
    __sync_synchronize();

    in_ += len;

    return len;
}

unsigned int RingBuffer::get(unsigned char* buf, unsigned int len)
{
    unsigned int l;
    len = std::min(len, in_ - out_);

    // Ensure that we sample the fifo->in index -before- we
    // start removing bytes from the kfifo.
    // smp_rmb();
    __sync_synchronize();

    l = std::min(len, size_ - (out_ & (size_ - 1)));
    memcpy(buf, buffer_ + (out_ & (size_ - 1)), l);
    memcpy(buf + l, buffer_, len - l);

    // Ensure that we remove the bytes from the kfifo -before-
    // we update the fifo->out index.
    // smp_mb();
    __sync_synchronize();

    out_ += len;

    return len;
}

} // namespace util
