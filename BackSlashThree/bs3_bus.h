#include "bs3_type.h"

#ifndef _BS3_BUS_H
#define _BS3_BUS_H 1

#define BS3_BUS_NOPORT_ADDR 0xFFFF
#define BS3_BUS_NOPORT_MASK 0x0000

struct bs3_device
{
    const char * name;
    WORD address;
    WORD mask;
    int (*startdevice)();
    int (*stopdevice)();
    BYTE (*readByte)(WORD address);
    void (*writeByte)(WORD address, BYTE data);
    WORD (*readWord)(WORD address);
    void (*writeWord)(WORD address, WORD data);
    BYTE interruptNumber;
};

void bs3_bus_start();
void bs3_bus_stop();
int  bs3_bus_plugdevice(struct bs3_device * ptrdevice);
void bs3_bus_clock_wait();
BYTE bs3_bus_readByte(WORD address);
void bs3_bus_writeByte(WORD address, BYTE data);
WORD bs3_bus_readWord(WORD address);
void bs3_bus_writeWord(WORD address, WORD data);
int  bs3_bus_setinterrupt(int interrupt);
int  bs3_bus_getinterrupt();
void bs3_bus_romflash_enable();
void bs3_bus_romflash_disable();
int  bs3_bus_romflash();
#endif
