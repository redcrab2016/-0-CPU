#include "bs3_type.h"

#ifndef _BS3_BUS_H
#define _BS3_BUS_H 1

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
BYTE bs3_bus_readByte(WORD address);
void bs3_bus_writeByte(WORD address, BYTE data);
WORD bs3_bus_readWord(WORD address);
void bs3_bus_writeWord(WORD address, WORD data);
int  bs3_bus_setinterrupt(int interrupt);
int  bs3_bus_getinterrupt();
#endif
