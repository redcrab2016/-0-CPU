#include "bs3_bus.h"


#define BANK_SIZE           32      /* 32 = 256 KiB  : 32 banks of 8 KiB each */
#define BANK_WIDTH          0x2000  /* rom bank width is 8 KiB ( wisdth must be a power of two ) */
#define BANK_MASK           0x1FFF  /* to filter bus address in local device address*/
#define BANK_LASTADDR       0x1FFF  /* last local device address */
#define BANK_ADDR           0xE000  /* device bus first address */
#define BANK_ADDR_MASK      0xE000  /* device bus address mask (chip select address mask ) */
#define BANK_ADDR_SELECT    0x0106  /* address where to read and write the bank selection */

extern struct bs3_device dev_bs3_rombank;
extern struct bs3_device dev_bs3_rombankselect;

BYTE rombank[BANK_SIZE][BANK_WIDTH];

BYTE banknumber;

BYTE dev_bs3_rombank_readByte(WORD address)
{
    if (banknumber >= BANK_SIZE) return 0;
    return rombank[banknumber][address & BANK_MASK];
}

void dev_bs3_rombank_writeByte(WORD address, BYTE data)
{
    if (!bs3_bus_romflash()) return;
    if (banknumber >= BANK_SIZE) return;
    rombank[banknumber][address & BANK_MASK] = data;
    return;
}

WORD dev_bs3_rombank_readWord(WORD address)
{
    if (banknumber >= BANK_SIZE) return 0;
    if ((address & BANK_MASK) == BANK_LASTADDR) return dev_bs3_rombank_readByte(address) | bs3_bus_readByte(address + 1);
    return *(WORD *)(&rombank[banknumber][address & 0x0FFF]);
}

void dev_bs3_rombank_writeWord(WORD address, WORD data)
{
    if (!bs3_bus_romflash()) return;
    if (banknumber >= BANK_SIZE) return;
    if ((address & BANK_MASK) == BANK_LASTADDR) /* write a word on last address of the device */
    {
        dev_bs3_rombank_writeByte(address, (BYTE)(data & 0x00FF));
        bs3_bus_writeByte(address + 1, (BYTE)((data >> 8) & 0x00FF));
        return;
    }
    *(WORD *)(&rombank[banknumber][address & BANK_MASK]) =  data;
    return;
}

BYTE dev_bs3_rombankselect_readByte(WORD address)
{
    return banknumber;
}

void dev_bs3_rombankselect_writeByte(WORD address, BYTE data)
{
    banknumber = data;
}


int dev_bs3_rombank_start()
{
    /* reset all bank memory */
    banknumber = 0;
    int i,j ;
    for ( i = 0 ; i < BANK_SIZE; i++ )
        for ( j = 0 ; j < BANK_WIDTH ; j++ )
            rombank[i][j] = 0;
}

int dev_bs3_rombank_stop()
{
    /* Nothing to do*/
}

/* declare BS3 rom bank device */

struct bs3_device dev_bs3_rombank = 
{
    .name               = "BS3-ROM-BANK",
    .address            = BANK_ADDR,
    .mask               = BANK_ADDR_MASK,                       /* 4 KiB rom on selected bank : 256 bank * 4KiB = 1MiB */
    .startdevice        = &dev_bs3_rombank_start,
    .stopdevice         = &dev_bs3_rombank_stop,
    .readByte           = &dev_bs3_rombank_readByte,
    .writeByte          = &dev_bs3_rombank_writeByte,
    .readWord           = &dev_bs3_rombank_readWord,
    .writeWord          = &dev_bs3_rombank_writeWord,
    .interruptNumber    = 0                                     /* No interrupt  */
};

 /* declare BS3 rom bank seletor device */

struct bs3_device dev_bs3_rombankselect = 
{
    .name               = "BS3-ROM-BANK-select",
    .address            = BANK_ADDR_SELECT,                     /* rom bank select address */
    .mask               = 0xFFFF,                               /* 1 register to select the rom bank number  */
    .startdevice        = 0,                                    /* no start device : embeded in start bank device*/
    .stopdevice         = 0,                                    /* no stop device : embeded in stop bank device */
    .readByte           = &dev_bs3_rombankselect_readByte,
    .writeByte          = &dev_bs3_rombankselect_writeByte,
    .readWord           = 0,
    .writeWord          = 0,
    .interruptNumber    = 0                                     /* No interrupt  */
};
