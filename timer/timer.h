#include<xc.h>           // processor SFR definitions

#define TIMER_DONE 1
#define TIMER_PENDING 0

#define TIMER_MILLISECOND 24000
#define TIMER_MICROSECOND 24

#define MAXUINT32 4294967295LL

unsigned int timer_start();
unsigned char timer_timeout(unsigned int ts, unsigned int to);
unsigned int timer_count(unsigned int ts);
unsigned int timer_diff(unsigned int ts, unsigned int t);
void timer_wait(unsigned int to);