#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h>

#include "bs3_type.h"

struct dev_rtc72421 {
    union 
    {
        BYTE reg[16];
        struct 
        {
            BYTE S1;
            BYTE S10;
            BYTE MI1;
            BYTE MI10;
            BYTE H1;
            union 
            {
                BYTE H10;
                struct
                {
                    BIT h10_20:2;
                    BIT AM_PM:1;
                    BIT unused_h3:1;
                    BIT reservedh4_7:4
                };
            };
            BYTE D1;
            BYTE D10;
            BYTE MO1;
            BYTE MO10;
            BYTE Y1;
            BYTE Y10;
            BYTE W;
            union 
            {
                BYTE CD;
                struct 
                {
                    BIT HOLD:1;
                    BIT BUSY:1;
                    BIT IRQ_FLAG:1;
                    BIT ADJ30s:1;
                    BIT reservedcd4_7:4
                };
            };
            union 
            {
                BYTE CE;
                struct 
                {
                    BIT MASK:1;
                    BIT ITRPT_STND:1;
                    BIT t0_1:2;
                    BIT reservedce4_7:4
                };
                
            };
            union 
            {
                BYTE CF;
                struct 
                {
                    BIT RESET:1;
                    BIT STOP:1;
                    BIT _24_12:1;
                    BIT TEST:1;
                    BIT reservedcf4_7:4;
                };
            };
            
        };
    };
};

struct dev_rtc72421 reg_RTC72421;
pthread_t thread_RTC72421; 


BYTE dev_rtc72421_read(WORD address)
{
    int addr = address & 0x00FF;
    BYTE value = reg_RTC72421.reg[addr] & 0x0F;
    if (addr == 0x0005 & reg_RTC72421._24_12) value &= 0x03; /* if 24h then remove AM/PM bit*/
    return value;
}

void dev_rtc72421_write(WORD address, BYTE data)
{
    int addr;
    BYTE value;
    addr = address & 0x00FF;
    value = data & 0x0F;
    if (reg_RTC72421.BUSY && addr < 0x0D) return; /* no RTC date/time change if busy */
    if (addr == 0x0D) /* keep busy bit */
    {
        value = (value & 0x0D) | (reg_RTC72421.reg[0x0D] & 2); /* TODO add mutex for busy bit*/
    }
    reg_RTC72421.reg[addr] = value;
}

/* this function is invoked by an independant thread */
void dev_rtc72421_run(void * bs3_device_bus) /* thread dedicated function */
{
    int count;
    int trig;
    BYTE carry;
    count = 0;
    trig = 0;
    /* TODO : init with current host RTC 24h format */
    while (1)
    {
        sleep(1); /* 1/64 th seconds , with a count of 64 */
        count++;
        /* ADJ30s bit == 1, then adjust to closest minute */
        if (reg_RTC72421.ADJ30s)
        {
            int currSecond;
            currSecond = reg_RTC72421.S10 * 10 + reg_RTC72421.S1;
            if (currSecond <=29)
            {
                reg_RTC72421.S10 = 0;
                reg_RTC72421.S1 = 0;
                count = 0;
                reg_RTC72421.ADJ30s = 0;
            } 
            else
            {
                reg_RTC72421.S10 = 5;
                reg_RTC72421.S1 = 9;
                count = 0;
                trig = 1;
            }
        }
        if (count == 1) /* TODO use usleep and a max count > 1*/
        {
            trig = 1;
            count = 0;
        }
        if (!reg_RTC72421.HOLD)
        {
            reg_RTC72421.BUSY = 1;
            if (trig)
            {
                trig = 0;
                carry = ((++reg_RTC72421.S1)>9)?1:0; /*Incr 1 second, carry if above or equal 10 */
                if (carry)
                {
                    reg_RTC72421.S1 = 0; /* reset the unit second */
                    carry = ((++reg_RTC72421.S10)>5)?1:0; /*incr 10 seconds, carry if above or equal 60 */
                    if (carry)
                    {
                        reg_RTC72421.S10 = 0; /* reset the 10 seconds*/
                        carry = ((++reg_RTC72421.MI1)>9)?1:0; /*Incr 1 minute, carry if above or equal 10 */
                        if (carry) /* go over minute*/
                        {
                            reg_RTC72421.MI1 = 0;
                            carry = ((++reg_RTC72421.MI10)>5)?1:0; /*incr 10 minutes, carry if above or equal 60 */
                            if (carry) /* if go over hour */
                            {
                                reg_RTC72421.MI10 = 0;
                                if (reg_RTC72421._24_12) /* 24 h case */
                                {
                                    reg_RTC72421.AM_PM = 0; /* no meaning then 0 (fake AM) */
                                    if (reg_RTC72421.H1 <2) /* when hour 0..19*/
                                    {
                                        carry = ((++reg_RTC72421.H1)>9)?1:0;
                                        if (carry)
                                        {
                                            reg_RTC72421.H1 = 0;
                                            ++reg_RTC72421.H10;
                                        }
                                        carry = 0; /* There is no go over midnight */
                                    }
                                    else /* when hour 20 ... 23 */
                                    {
                                        carry = ((++reg_RTC72421.H1)>4)?1:0;
                                        if (carry) /* go over midnight*/
                                        {
                                            reg_RTC72421.H1 = 0;
                                            reg_RTC72421.H10 = 0;
                                        }
                                    }
                                } 
                                else /* 12 h AM/PM case */
                                {
                                    BYTE curr24h;
                                    curr24h = ((reg_RTC72421.H10 * 10 + reg_RTC72421.H1)+12)%12 + (reg_RTC72421.AM_PM?12:0);
                                    carry = ((++curr24h)>23)?1:0; /* next hour with carry */
                                    curr24h = curr24h % 24; /* normalize after increment */
                                    switch(curr24h)
                                    {
                                        case 0:
                                            curr24h = 12;
                                        case 1 ... 11:
                                            reg_RTC72421.AM_PM = 0;
                                            break;
                                        case 13 ... 23:
                                            curr24h -= 12;
                                        case 12:
                                            reg_RTC72421.AM_PM = 1;                 
                                            break;
                                    }
                                    reg_RTC72421.H1 = curr24h % 10;
                                    reg_RTC72421.H10 = curr24h / 10;
                                }
                                if (carry) /* when go over midnight */
                                {
                                    reg_RTC72421.W = (reg_RTC72421.W + 1) % 7; /* next week day */
                                    BYTE currMonth = reg_RTC72421.MO10 *  10 + reg_RTC72421.MO1;
                                    BYTE currDay = reg_RTC72421.D10 * 10 + reg_RTC72421.D1;
                                    BYTE currYear = reg_RTC72421.Y10 * 10 + reg_RTC72421.Y1;
                                    switch (currMonth)
                                    {
                                        case 1:  /* January  */
                                        case 3:  /* March    */
                                        case 5:  /* May      */
                                        case 7:  /* Jully    */
                                        case 8:  /* August   */
                                        case 10: /* October  */
                                        case 12: /* December  31 days months*/
                                            carry = ((++currDay)>31)?1:0;
                                            currDay = (carry)?1:currDay;
                                            break;
                                        case 2: /* February 28 or 29 days */
                                            if ((currYear & 0x03) == 0) /* leap year : 29 days*/
                                            {
                                                carry = ((++currDay)>29)?1:0;
                                                currDay = (carry)?1:currDay;
                                            }
                                            else /* 28 days*/
                                            {
                                                carry = ((++currDay)>28)?1:0;
                                                currDay = (carry)?1:currDay;
                                            }
                                            break;
                                        case 4:  /* April     */
                                        case 6:  /* June      */
                                        case 9:  /* September */
                                        case 11: /* November  30 days months */
                                            carry = ((++currDay)>30)?1:0;
                                            currDay = (carry)?1:currDay;
                                            break;                                            
                                    }
                                    reg_RTC72421.D10 = currDay / 10;
                                    reg_RTC72421.D1 = currDay % 10;
                                    if (carry) /* go over another month*/
                                    {
                                        carry = ((++currMonth)>12)?1:0;
                                        currMonth = (carry)?1:currMonth;
                                        reg_RTC72421.MO10 = currMonth / 10;
                                        reg_RTC72421.MO1 = currMonth % 10;
                                        if (carry) /* go over year */
                                        {
                                            carry = ((++currYear)>99)?1:0;
                                            currYear = (carry)?0:currYear;
                                            reg_RTC72421.Y10 = currYear / 10;
                                            reg_RTC72421.Y1 = currYear % 10;
                                        }
                                    }
                                } /* end if go over day */
                            }  /* end of go over hour*/
                        } /* end if go over minute */
                    }
                }
            }
            reg_RTC72421.ADJ30s = 0;
        }
        else reg_RTC72421.BUSY = 0;
    } /* end of while forever */
}


int dev_rtc72421_stop()
{
  int result = pthread_kill(thread_RTC72421, 0);
  if (result == ESRCH) {
    return result;
  } else if (result == 0) {
    pthread_cancel(thread_RTC72421); 
    pthread_join(thread_RTC72421, NULL);
    return 0;
  }
}

void dev_rtc72421_start(void * bs3_device_bus)
{
    dev_rtc72421_remove(); /* just to be sure */
    pthread_create(&thread_RTC72421, NULL, &dev_rtc72421_run, bs3_device_bus); 
}
