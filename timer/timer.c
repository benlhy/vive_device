#include "timer.h"

unsigned int timer_start()
{
    return _CP0_GET_COUNT();
}

unsigned char timer_timeout(unsigned int ts, unsigned int to)
{
    return (timer_count(ts)>to);
}

unsigned int timer_count(unsigned int ts)
{
    unsigned int t = _CP0_GET_COUNT();
    return timer_diff(ts,t);
}

unsigned int timer_diff(unsigned int ts, unsigned int t)
{
    if (t > ts)
    {
        return t-ts;
    }
    else
    {
        long r = (MAXUINT32-ts+t);
        return (unsigned int)r;
    }
}

void timer_wait(unsigned int to)
{
    unsigned int ts=timer_start();
    while (!timer_timeout(ts,to));
}