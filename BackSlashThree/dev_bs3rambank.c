#include "bs3_bus.h"


#define BANK_SIZE           64      /* 64 = 256 KiB  : 64 banks of 4 KiB each */
#define BANK_WIDTH          0x1000  /* Ram bank width is 4 KiB ( wisdth must be a power of two ) */
#define BANK_MASK           0x0FFF  /* to filter bus address in local device address*/
#define BANK_LASTADDR       0x0FFF  /* last local device address */
#define BANK_ADDR           0xD000  /* device bus first address */
#define BANK_ADDR_MASK      0xF000  /* device bus address mask (chip select address mask ) */
#define BANK_ADDR_SELECT    0x0107  /* address where to read and write the bank selection */

extern struct bs3_device dev_bs3_rambank;
extern struct bs3_device dev_bs3_rambankselect;

BYTE rambank[BANK_SIZE][BANK_WIDTH];

BYTE banknumber;

BYTE dev_bs3_rambank_readByte(WORD address)
{
    if (banknumber >= BANK_SIZE) return 0;
    return rambank[banknumber][address & BANK_MASK];
}

void dev_bs3_rambank_writeByte(WORD address, BYTE data)
{
    if (banknumber >= BANK_SIZE) return;
    rambank[banknumber][address & BANK_MASK] = data;
    return;
}

WORD dev_bs3_rambank_readWord(WORD address)
{
    if (banknumber >= BANK_SIZE) return 0;
    if ((address & BANK_MASK) == BANK_LASTADDR) return dev_bs3_rambank_readByte(address) | bs3_bus_readByte(address + 1);
    return *(WORD *)(&rambank[banknumber][address & 0x0FFF]);
}

void dev_bs3_rambank_writeWord(WORD address, WORD data)
{
    if (banknumber >= BANK_SIZE) return;
    if ((address & BANK_MASK) == BANK_LASTADDR) /* write a word on last address of the device */
    {
        dev_bs3_rambank_writeByte(address, (BYTE)(data & 0x00FF));
        bs3_bus_writeByte(address + 1, (BYTE)((data >> 8) & 0x00FF));
        return;
    }
    *(WORD *)(&rambank[banknumber][address & BANK_MASK]) =  data;
    return;
}

BYTE dev_bs3_rambankselect_readByte(WORD address)
{
    return banknumber;
}

void dev_bs3_rambankselect_writeByte(WORD address, BYTE data)
{
    banknumber = data;
}


int dev_bs3_rambank_start()
{
    /* reset all bank memory */
    banknumber = 0;
    int i,j ;
    for ( i = 0 ; i < BANK_SIZE; i++ )
        for ( j = 0 ; j < BANK_WIDTH ; j++ )
            rambank[i][j] = 0;
    return 0;
}

int dev_bs3_rambank_stop()
{
    /* Nothing to do*/
    return 0;
}

/* declare BS3 ram bank device */

struct bs3_device dev_bs3_rambank = 
{
    .name               = "BS3-RAM-BANK",
    .address            = BANK_ADDR,
    .mask               = BANK_ADDR_MASK,                       /* 4 KiB ram on selected bank : 256 bank * 4KiB = 1MiB */
    .startdevice        = &dev_bs3_rambank_start,
    .stopdevice         = &dev_bs3_rambank_stop,
    .readByte           = &dev_bs3_rambank_readByte,
    .writeByte          = &dev_bs3_rambank_writeByte,
    .readWord           = &dev_bs3_rambank_readWord,
    .writeWord          = &dev_bs3_rambank_writeWord,
    .interruptNumber    = 0                                     /* No interrupt  */
};

 /* declare BS3 ram bank seletor device */

struct bs3_device dev_bs3_rambankselect = 
{
    .name               = "BS3-RAM-BANK-select",
    .address            = BANK_ADDR_SELECT,                     /* ram bank select address */
    .mask               = 0xFFFF,                               /* 1 register to select the ram bank number  */
    .startdevice        = 0,                                    /* no start device : embeded in start bank device*/
    .stopdevice         = 0,                                    /* no stop device : embeded in stop bank device */
    .readByte           = &dev_bs3_rambankselect_readByte,
    .writeByte          = &dev_bs3_rambankselect_writeByte,
    .readWord           = 0,
    .writeWord          = 0,
    .interruptNumber    = 0                                     /* No interrupt  */
};
