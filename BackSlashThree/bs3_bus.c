#include <pthread.h>
#include <stdio.h>
#include "bs3_bus.h"

#define INTERRUPT_QUEUE_SIZE 16

struct bs3_device * bs3_devices[256];
int nb_bs3_device = 0;
struct bs3_device * bs3_bus_addresses[65536];
pthread_mutex_t lockbus = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lockirq = PTHREAD_MUTEX_INITIALIZER;
int bs3_bus_interrupt_FIFO[16];
int bs3_bus_interrupt_FIFO_front = 0;
int bs3_bus_interrupt_FIFO_rear = -1;
int bs3_bus_interrupt_FIFO_count = 0;

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
    if (nb_bs3_device >= 256) return 1; /* error ... too many devices */
    bs3_devices[nb_bs3_device++] = ptrdevice;
    long addr;
    for (addr = 0; addr < 65536; addr++)
    {
        if ((addr & ptrdevice->mask) == (ptrdevice->address & ptrdevice->mask))
        {
            bs3_bus_addresses[addr & 0x0FFFF] = ptrdevice;
        }
    }
    return 0;
}

BYTE bs3_bus_readByte(WORD address)
{
    BYTE value;
    if (bs3_bus_addresses[address] == 0) return 0;
    if (bs3_bus_addresses[address]->readByte == 0) return 0;
//    pthread_mutex_lock(&lockbus); 
    value = (*(bs3_bus_addresses[address]->readByte))(address);
//    pthread_mutex_unlock(&lockbus); 

}

void bs3_bus_writeByte(WORD address, BYTE data)
{
    if (bs3_bus_addresses[address] == 0) return;
    if (bs3_bus_addresses[address]->writeByte == 0) return;
//    pthread_mutex_lock(&lockbus); 
    (*(bs3_bus_addresses[address]->writeByte))(address, data);
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

/* Interrupt controler */

BYTE bs3_bus_intctrl_readbyte(WORD address)
{
    return (BYTE)bs3_bus_interrupt_FIFO_count;
}

void bs3_bus_intctrl_writebyte(WORD address, BYTE data)
{
    while (bs3_bus_interrupt_dequeue() != -1 );/* empty the FIFO IRQ queue */
}

struct bs3_device dev_bs3irqctrl = 
{
    .name="BS3 IRQ-Controller",
    .address=0x010E,
    .mask=0xFFFF, /* One I/O for IRQ */
    .startdevice = NULL,
    .stopdevice = NULL,
    .readByte = &bs3_bus_intctrl_readbyte,
    .writeByte = &bs3_bus_intctrl_writebyte,
    .interruptNumber = 0 /* No interrupt (BS3 CPU IRQ pin emulated) */
};
