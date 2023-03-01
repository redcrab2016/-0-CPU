#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include "bs3_bus.h"

#define INTERRUPT_QUEUE_SIZE 16
#define BUS_FREQ_MHZ 20
struct bs3_device * bs3_devices[256];
int nb_bs3_device = 0;
struct bs3_device * bs3_bus_addresses[65536];
static pthread_mutex_t lockbus = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t lockirq = PTHREAD_MUTEX_INITIALIZER;
pthread_t thread_busClock;
static pthread_mutex_t lockBusClock =  PTHREAD_MUTEX_INITIALIZER;
int created_thread_busClock = 0;
int endClock = 0;
unsigned long bs3_clock = 0;
int bs3_bus_interrupt_FIFO[16];
int bs3_bus_interrupt_FIFO_front = 0;
int bs3_bus_interrupt_FIFO_rear = -1;
int bs3_bus_interrupt_FIFO_count = 0;
int bs3_bus_romflash_enabled = 0;
unsigned long long clock_tick_delay;

void bs3_bus_romflash_enable()
{
    bs3_bus_romflash_enabled = 1;
}

void bs3_bus_romflash_disable()
{
    bs3_bus_romflash_enabled = 0;
}

int  bs3_bus_romflash()
{
    return bs3_bus_romflash_enabled;
}


int bs3_bus_interrupt_enqueue(int interrupt) {
  int i;
  pthread_mutex_lock(&lockirq);
  /* check if interrupt is already in queue : if yes do not enqueue interrupt */
  for (i = 0 ; i < INTERRUPT_QUEUE_SIZE ; i++)
  {
    if (bs3_bus_interrupt_FIFO[i] == interrupt)
    {
        pthread_mutex_unlock(&lockirq);
        return bs3_bus_interrupt_FIFO_count;       
    } 
  }
  /* does queue is full */
  if (bs3_bus_interrupt_FIFO_count == INTERRUPT_QUEUE_SIZE) 
  {
    pthread_mutex_unlock(&lockirq);
    return -1;
  }
  /* not full then enqueue */
  bs3_bus_interrupt_FIFO_rear = (bs3_bus_interrupt_FIFO_rear + 1) % INTERRUPT_QUEUE_SIZE;
  bs3_bus_interrupt_FIFO[bs3_bus_interrupt_FIFO_rear] = interrupt;
  bs3_bus_interrupt_FIFO_count++;
  pthread_mutex_unlock(&lockirq);
  return bs3_bus_interrupt_FIFO_count;
}

int bs3_bus_interrupt_dequeue() {
  if (bs3_bus_interrupt_FIFO_count == 0) {
    return -1;
  }
  pthread_mutex_lock(&lockirq);
  int item = bs3_bus_interrupt_FIFO[bs3_bus_interrupt_FIFO_front];
  bs3_bus_interrupt_FIFO[bs3_bus_interrupt_FIFO_front] = -1;/* clean queue */
  bs3_bus_interrupt_FIFO_front = (bs3_bus_interrupt_FIFO_front + 1) % INTERRUPT_QUEUE_SIZE;
  bs3_bus_interrupt_FIFO_count--;
  pthread_mutex_unlock(&lockirq);
  return item;
}
/*
struct bs3_device bs3_bus_default_device = 
{
    .name="The Device Name",
    .address=0x0000,
    .mask=0x0000,
    .startdevice = &bs3_bus_defaultStart,
    .stopdevice = &bs3_bus_defaultStop,
    .readByte = &bs3_bus_defaultReadByte,
    .writeByte = &bs3_bus_defaultWriteByte,
    .getIRQstate = &bs3_bus_defaultIRQstate,
    .interruptNumber = 0
};
*/
void bs3_bus_start()
{
    int i;
    int r;
    for ( i = 0 ; i < nb_bs3_device; i++)
    {
        if (bs3_devices[i] && bs3_devices[i]->startdevice)
        {
            r = (*(bs3_devices[i]->startdevice))();
            if (r) 
            {
                fprintf(stderr,"BS3 error when starting device %s\n",bs3_devices[i]->name);
            }
        }
    }
}

void bs3_bus_stop()
{
    int i;
    int r;
    for ( i = 0 ; i < nb_bs3_device; i++)
    {
        if (bs3_devices[i] && bs3_devices[i]->stopdevice)
        {
            r = (*(bs3_devices[i]->stopdevice))();
            if (r) 
            {
                fprintf(stderr,"BS3 error when stoping device %s\n",bs3_devices[i]->name);
            }
        }
    }
}

int  bs3_bus_plugdevice(struct bs3_device * ptrdevice)
{
    long addr;

    if (nb_bs3_device >= 256) return 1; /* error ... too many devices */
    bs3_devices[nb_bs3_device++] = ptrdevice;

    if (ptrdevice->mask == BS3_BUS_NOPORT_MASK &&
        ptrdevice->address == BS3_BUS_NOPORT_ADDR)
        return 0; /* if device do not handle any address */

    for (addr = 0; addr < 65536; addr++)
    {
        if ((addr & ptrdevice->mask) == (ptrdevice->address & ptrdevice->mask))
        {
            bs3_bus_addresses[addr & 0x0FFFF] = ptrdevice;
        }
    }
    return 0;
}

/* bus access without clock consumption */
BYTE bs3_bus_readByte_(WORD address)
{
    BYTE value;
    if (bs3_bus_addresses[address] == 0) return 0;
    if (bs3_bus_addresses[address]->readByte == 0) return 0;
//    pthread_mutex_lock(&lockbus); 
    value = (*(bs3_bus_addresses[address]->readByte))(address);
//    pthread_mutex_unlock(&lockbus); 
    return value;
}

BYTE bs3_bus_readByte(WORD address)
{
    BYTE value;
    bs3_bus_clock_wait();
    return bs3_bus_readByte_(address);
}

/* bus access without clock consumption */
void bs3_bus_writeByte_(WORD address, BYTE data)
{
    if (bs3_bus_addresses[address] == 0) return;
    if (bs3_bus_addresses[address]->writeByte == 0) return;
//    pthread_mutex_lock(&lockbus); 
    (*(bs3_bus_addresses[address]->writeByte))(address, data);
//    pthread_mutex_unlock(&lockbus); 
}

void bs3_bus_writeByte(WORD address, BYTE data)
{
    bs3_bus_clock_wait();
    bs3_bus_writeByte_(address, data);
}

WORD bs3_bus_readWord(WORD address)
{
    WORD value;
    bs3_bus_clock_wait();    
    if (bs3_bus_addresses[address] == 0) return 0;
    if (bs3_bus_addresses[address]->readWord == 0) 
    {
        value = bs3_bus_readByte_(address) ;
        if (address & 0x0001 == 0)
            value = value | (bs3_bus_readByte_(address+1) << 8 );
        else
            value = value | (bs3_bus_readByte(address+1) << 8 ); /* read word on odd address consume one more bus clock */
        return value;    
    }
//    pthread_mutex_lock(&lockbus); 
    value = (*(bs3_bus_addresses[address]->readWord))(address);
//    pthread_mutex_unlock(&lockbus); 
    return value;
}

void bs3_bus_writeWord(WORD address, WORD data)
{
    bs3_bus_clock_wait();
    if (bs3_bus_addresses[address] == 0) return;
    if (bs3_bus_addresses[address]->writeWord == 0) 
    {
        bs3_bus_writeByte_(address, (BYTE)(data & 0x00FF));
        if ((address & 0x0001) == 0) /* if address is even use same clock bus time */
            bs3_bus_writeByte_(address + 1, (BYTE)((data >> 8) & 0x00FF));
        else
            bs3_bus_writeByte(address + 1, (BYTE)((data >> 8) & 0x00FF)); /* word access on odd address consume one more bus clock */
        return;
    }
//    pthread_mutex_lock(&lockbus); 
    (*(bs3_bus_addresses[address]->writeWord))(address, data);
//    pthread_mutex_unlock(&lockbus); 
}

/* to be used by the device that emit an interrupt */
int bs3_bus_setinterrupt(int interrupt)
{
    return bs3_bus_interrupt_enqueue(interrupt);
}

/* to be used by the CPU to get the IRQ : emulated BS3 CPU IRQ pin + IRQ controller information retreival */
int bs3_bus_getinterrupt()
{
    int interrupt;
    int i;
    if (bs3_bus_interrupt_FIFO_count == 0) return -1;
    interrupt = bs3_bus_interrupt_dequeue();
    return interrupt;
}

/* Interrupt controler device */

BYTE bs3_bus_intctrl_readbyte(WORD address)
{
    return (BYTE)bs3_bus_interrupt_FIFO_count;
}

void bs3_bus_intctrl_writebyte(WORD address, BYTE data)
{
    while (bs3_bus_interrupt_dequeue() != -1 );/* empty the FIFO IRQ queue */
}

/* Bus clock device */

/* duration of execution of this function must be at most the time delay of the clock */
void bs3_bus_clock_wait()
{
    pthread_mutex_lock(&lockBusClock);
    pthread_mutex_unlock(&lockBusClock);
}

/* this function is invoked by an independant thread */
static void * bs3_bus_clock_run(void * bs3_device_bus)
{
    int response;
    unsigned long long ticker;
    endClock = 0;
    while (!endClock)
    {
        pthread_mutex_lock(&lockBusClock);
        ticker = clock_tick_delay;
        while (ticker) ticker--;
        bs3_clock++;      
        pthread_mutex_unlock(&lockBusClock);

    }
    return NULL;
}

int bs3_bus_clock_stop()
{
    int result;
    if (!created_thread_busClock) return 0;
    result = pthread_kill(thread_busClock, 0);
    created_thread_busClock = 0;
    endClock = 1;

    if (result == ESRCH) 
    {
        return result;
    } 
    else if (result == 0) 
    {
        pthread_join(thread_busClock, NULL);
        return 0;
    }    
}

int bs3_bus_clock_start()
{
    if (created_thread_busClock) bs3_bus_clock_stop();
    bs3_clock = 0;
    endClock = 0;
    /* calibration strategy */
    struct timespec tS;
    unsigned long long ticker;
    tS.tv_sec = 0;
    tS.tv_nsec = 0;
    ticker = 1000000000UL;
    clock_settime(CLOCK_THREAD_CPUTIME_ID /*CLOCK_PROCESS_CPUTIME_ID */, &tS);
    while (ticker) ticker--;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID /* CLOCK_PROCESS_CPUTIME_ID */, &tS); 
    long double period = 1000000000.0/(BUS_FREQ_MHZ * 1000000.0);
    clock_tick_delay = (unsigned long long)(((long double)(1000000000.0) * period)/  ((long double)tS.tv_sec * (long double)(1000000000UL) + (long double)tS.tv_nsec));   
    /* for debug : 
    fprintf(stdout, "Clock calibration time spent for 1000000000 cyles\n Duration: %lu s and %ld nanosecond\n Period: %Lf ns for %d Mhz\n %llu loops/period\n", 
            tS.tv_sec, tS.tv_nsec,period, BUS_FREQ_MHZ,clock_tick_delay);
    */
    /* end of calibration */



    int result = pthread_create(&thread_busClock, NULL, &bs3_bus_clock_run, NULL); 
    if (result == 0) created_thread_busClock = 1;
    return result;
}



struct bs3_device dev_bs3busclock =
{
    .name = "BS3 Bus Clock",
    .address = BS3_BUS_NOPORT_ADDR,
    .mask = BS3_BUS_NOPORT_MASK,
    .startdevice = &bs3_bus_clock_start,
    .stopdevice = &bs3_bus_clock_stop,
    .writeByte = NULL,
    .readByte = NULL,
    .writeWord = NULL,
    .readWord = NULL,
    .interruptNumber = 0 /* no interrupt */
};

struct bs3_device dev_bs3irqctrl = 
{
    .name="BS3 IRQ-Controller",
    .address=0x010E,
    .mask=0xFFFF, /* One I/O for IRQ */
    .startdevice = NULL,
    .stopdevice = NULL,
    .readByte = &bs3_bus_intctrl_readbyte,
    .writeByte = &bs3_bus_intctrl_writebyte,
    .readWord = NULL,
    .writeWord = NULL,
    .interruptNumber = 0 /* No interrupt (BS3 CPU IRQ pin emulated) */
};
