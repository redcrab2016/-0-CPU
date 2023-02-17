#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "bs3_bus.h"

struct dev_bs3timer {
    union 
    {
        BYTE reg[8];
        unsigned long timer; /* must be 32 bits, do not forget to mask with 0x0FFFFFFFF*/
        struct 
        {
            DWORD timerLow16;
            DWORD timerHigh16;
        };
        struct 
        {
            BYTE timerByte0;
            BYTE timerByte1;
            BYTE timerByte2;
            BYTE timerByte3; /* a write to this address reinit the timer */
            BYTE timerMode; /* ==0 repeat, ==1 once */
            BYTE reserved5;
            BYTE reserved6; /* same address as bs3 IRQ port ,  bs3 IRQ device must mask this address */
            BYTE reserved7;
        };
    };
};

struct dev_bs3timer reg_bs3timer;
int endTimer;
int timerChanged;
pthread_t thread_bs3timer; 
int created_thread_bs3timer = 0;
pthread_cond_t condWakeUp = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t lockTimer = PTHREAD_MUTEX_INITIALIZER; 
extern struct bs3_device dev_bs3timer;

BYTE dev_bs3timer_read(WORD address)
{
    WORD addr;
    addr = address & 0x0007;
    return reg_bs3timer.reg[addr];
}

void dev_bs3timer_write(WORD address, BYTE data)
{
    WORD addr;
    addr = address & 0x0007;
    reg_bs3timer.reg[addr] = data;
    if ( addr == 0x0003)
    {
        pthread_mutex_lock(&lockTimer);
        timerChanged = 1;
        pthread_cond_signal(&condWakeUp);
        pthread_mutex_unlock(&lockTimer);
    }
}

static void * dev_bs3timer_run(void * bs3_device_bus) /* thread dedicated function */
{
    endTimer = 0;
    struct timespec request;
    unsigned long timer; /* micro second timer */
    unsigned long long tmptimer;
    int afterwait;
    timer = reg_bs3timer.timer & 0x0FFFFFFFF;

    pthread_mutex_lock(&lockTimer);
    while (!endTimer)
    {
        clock_gettime(CLOCK_REALTIME, &request);
        if (timer)
        {
            request.tv_sec += (timer / 1000000);
            request.tv_nsec += ((timer % 1000000) * 1000);
            request.tv_sec += request.tv_nsec / 1000000000;
            request.tv_nsec = request.tv_nsec % 1000000000;
        } 
        else
        {
            request.tv_sec += 1;
            request.tv_nsec += 0;
        }
       
        afterwait = pthread_cond_timedwait(&condWakeUp, &lockTimer, &request);
        if (timer && reg_bs3timer.timerMode) timer = 0; /* do not readmed the timer is mode is 1 */
        if (timer && afterwait == ETIMEDOUT) 
            bs3_bus_setinterrupt(dev_bs3timer.interruptNumber); /* if out of timedwait without signal then set interrupt */
        if (timerChanged) 
        {
            timerChanged = 0;
            timer = reg_bs3timer.timer & 0x0FFFFFFFF;
        }

    }
    pthread_mutex_unlock(&lockTimer);
    return NULL;
}


int dev_bs3timer_stop()
{
    int resultTimer ;
    if (created_thread_bs3timer)   resultTimer = pthread_kill(thread_bs3timer, 0);

    if (resultTimer == 0 && created_thread_bs3timer) 
    {
        endTimer = 1;
        pthread_mutex_lock(&lockTimer);
        pthread_cond_signal(&condWakeUp);
        pthread_mutex_unlock(&lockTimer);        
        pthread_join(thread_bs3timer, NULL);
    }
    created_thread_bs3timer = 0;
    return resultTimer;
}

int dev_bs3timer_start()
{
     int resultTimer ;
    if (created_thread_bs3timer) dev_bs3timer_stop(); /* just to avoid double start without stop */
    resultTimer = pthread_create(&thread_bs3timer, NULL, &dev_bs3timer_run, NULL); 
    if (resultTimer == 0) created_thread_bs3timer = 1;
    if (resultTimer) return resultTimer;
    return 0;
}
/* declare BS3 Timer device */

struct bs3_device dev_bs3timer = 
{
    .name="BS3-TIMER",
    .address=0x0108,
    .mask=0xFFF8, /* 8 BS3 I/O registers (5 in/out + 3 reserved) */
    .startdevice = &dev_bs3timer_start,
    .stopdevice = &dev_bs3timer_stop,
    .readByte = &dev_bs3timer_read,
    .writeByte = &dev_bs3timer_write,
    .readWord = NULL,
    .writeWord = NULL,
    .interruptNumber = 3 /* interrupt vector 3 */
};
