#include <pthread.h>
#include <stdio.h>
#include "bs3_bus.h"

#define INTERRUPT_QUEUE_SIZE 16

struct bs3_device * bs3_devices[256];
int nb_bs3_device = 0;
struct bs3_device * bs3_bus_addresses[65536];
pthread_mutex_t lockbus = PTHREAD_MUTEX_INITIALIZER;
int bs3_bus_interrupt_FIFO[16];
int bs3_bus_interrupt_FIFO_front = 0;
int bs3_bus_interrupt_FIFO_rear = 0;
int bs3_bus_interrupt_FIFO_count = 0;

int bs3_bus_interrupt_enqueue(int item) {
  if (bs3_bus_interrupt_FIFO_count == INTERRUPT_QUEUE_SIZE) {
    return -1;
  }
  bs3_bus_interrupt_FIFO_rear = (bs3_bus_interrupt_FIFO_rear + 1) % INTERRUPT_QUEUE_SIZE;
  bs3_bus_interrupt_FIFO[bs3_bus_interrupt_FIFO_rear] = item;
  bs3_bus_interrupt_FIFO_count++;
  return bs3_bus_interrupt_FIFO_count;
}

int bs3_bus_interrupt_dequeue() {
  if (bs3_bus_interrupt_FIFO_count == 0) {
    return -1;
  }
  int item = bs3_bus_interrupt_FIFO[bs3_bus_interrupt_FIFO_front];
  bs3_bus_interrupt_FIFO_front = (bs3_bus_interrupt_FIFO_front + 1) % INTERRUPT_QUEUE_SIZE;
  bs3_bus_interrupt_FIFO_count--;
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
    pthread_mutex_lock(&lockbus); 
    value = (*(bs3_bus_addresses[address]->readByte))(address);
    pthread_mutex_unlock(&lockbus); 

}

void bs3_bus_writeByte(WORD address, BYTE data)
{
    if (bs3_bus_addresses[address] == 0) return;
    if (bs3_bus_addresses[address]->writeByte == 0) return;
    pthread_mutex_lock(&lockbus); 
    (*(bs3_bus_addresses[address]->writeByte))(address, data);
    pthread_mutex_unlock(&lockbus); 
}

int bs3_bus_interrupt()
{
    int interrupt;
    int i;
    for (i = 0 ; i < nb_bs3_device ; i++)
    {
        if (bs3_devices[i] && bs3_devices[i]->getIRQstate)
        {
            if ((*(bs3_devices[i]->getIRQstate))())
            {
                bs3_bus_interrupt_enqueue(bs3_devices[i]->interruptNumber);
            }
        }
    }

    interrupt = bs3_bus_interrupt_dequeue();
    return interrupt;
}
