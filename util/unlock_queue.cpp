
#include "unlock_queue.h"

#include <stdlib.h>
#include <string.h>


#ifndef max
#define max(x, y) ({                \
        typeof(x) _max1 = (x);          \
        typeof(y) _max2 = (y);          \
        (void) (&_max1 == &_max2);      \
        _max1 > _max2 ? _max1 : _max2; })
#endif

#ifndef min
#define min(x, y) ({                \
        typeof(x) _min1 = (x);          \
        typeof(y) _min2 = (y);          \
        (void) (&_min1 == &_min2);      \
        _min1 < _min2 ? _min1 : _min2; })
#endif


unsigned long UnlockQueue::roundup_power_of_two(unsigned long val)
{
    if ((val & (val - 1)) == 0)
    {
        return val;
    }
    unsigned long maxulong = (unsigned long)((unsigned long) ~0);
    unsigned long andv = ~(maxulong & (maxulong >> 1));
    while ((andv & val) == 0)
        andv = andv >> 1;
    return andv << 1;
}

UnlockQueue::UnlockQueue(unsigned int isize): size(isize), in(0), out(0), mask(size - 1)
{
    if (!is_power_of_2(isize))
    {
        size = roundup_power_of_two(isize);
    }
    buffer = (unsigned char*) malloc(isize);
}

UnlockQueue::~UnlockQueue()
{
    if (buffer) free(buffer);
    size = in = out = 0;
}


unsigned int UnlockQueue::get(unsigned char* _buffer, unsigned int len)
{
    unsigned int l;
    len = min(len, in - out);
    __sync_synchronize();

    l = min(len, size - (out & (size - 1)));
    memcpy(_buffer, buffer + (out & (size - 1)), l);
    memcpy(_buffer + l, buffer, len - l);

    __sync_synchronize();

    out += len;
    return len;

}

unsigned int UnlockQueue::put(const unsigned char* _buffer, unsigned int len)
{
    unsigned int l;
    len = min(len, size - in + out);
    __sync_synchronize();

    l = min(len, size - (in & (size - 1)));
    memcpy(buffer + (in & (size - 1)), _buffer, l);
    memcpy(buffer, _buffer + l, len - l);

    __sync_synchronize();

    in += len;
    return len;

}



#ifdef TEST_MAIN


#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>

using namespace  std;


void* consumer(void* arg)
{
    printf("consumer\n");
    UnlockQueue* fifo = (UnlockQueue*) arg;
    if (!fifo)
    {
        return NULL;
    }

    for (;;)
    {
        proto p;
        unsigned int len = fifo->get((unsigned char*)&p, sizeof(p));
        if (len > 0)
        {
            cout << "~~~~~~~~~~~~~~~~~~~~" << endl;
            cout << "consumer proto msg id :" << p.msgid << endl;
            cout << "consumer proto msg cmd :" << p.cmd << endl;
            cout << "consumer proto msg info :" << p.info << endl;
            cout << "~~~~~~~~~~~~~~~~~~~~" << endl;
        }
        usleep(100000);
    }

    return (void*)fifo;
}


void* producer(void* args)
{
    UnlockQueue* fifo = (UnlockQueue*) args;

    if (!fifo)
    {
        return NULL;
    }
    unsigned int i = 0;
    for (;;)
    {
        proto p;
        p.msgid = i++;
        p.cmd =  333;
        p.info = 44444;
        fifo->put((const unsigned char*)&p, sizeof(p));
        cout << "producer put msgid :" << p.msgid << endl;
        usleep(60000);
    }
    return (void*)fifo;
}
int main()
{
    UnlockQueue* fifo = new UnlockQueue(1024);
    pthread_t consumeid, producerid;

    pthread_create(&producerid, NULL, producer, (void*)fifo);
    pthread_create(&consumeid, NULL, consumer, (void*)fifo);

    printf("info!!\n");
    pthread_join(consumeid, NULL);
    pthread_join(producerid, NULL);
    return 0;
}

#endif
