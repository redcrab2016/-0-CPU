#include <pthread.h>
#include <stdio.h>
#include "bs3_bus.h"

struct bs3_device * bs3_devices[256];
int nb_bs3_device = 0;
struct bs3_device * bs3_bus_addresses[65536];
pthread_mutex_t lockbus = PTHREAD_MUTEX_INITIALIZER;
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
