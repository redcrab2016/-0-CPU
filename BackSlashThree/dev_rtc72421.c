#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "bs3_bus.h"

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
                    BIT reservedh4_7:4;
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
                    BIT reservedcd4_7:4;
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
                    BIT reservedce4_7:4;
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
    BYTE hasIRQEventSignal;
    int pulseCount;
};

struct dev_rtc72421 reg_RTC72421;
pthread_t thread_RTC72421; 
int created_thread_RTC72421 = 0;
extern struct bs3_device dev_rtc72421;


BYTE dev_rtc72421_read(WORD address)
{
    int addr = address & 0x000F;
    BYTE value = reg_RTC72421.reg[addr] & 0x0F;
    if (addr == 0x0005 && reg_RTC72421._24_12) value &= 0x03; /* if 24h then remove AM/PM bit*/
    return value;
}

void dev_rtc72421_write(WORD address, BYTE data)
{
    int addr;
    BYTE value;
    addr = address & 0x000F;
    value = data & 0x0F;
    if (reg_RTC72421.BUSY && addr < 0x0D) return; /* no RTC date/time change if busy */
    if (addr == 0x0D) /* keep busy bit */
    {
        value = (value & 0x0D) | (reg_RTC72421.reg[0x0D] & 2); /* TODO add mutex for busy bit*/
        pthread_kill(thread_RTC72421, SIGUSR1);
    }
    reg_RTC72421.reg[addr] = value;
}


static void dev_rtc72421_sleep_tenthofmicroseconds(long microseconds)
{
    struct timespec req = {0};
    req.tv_sec = 0;
    req.tv_nsec = microseconds * 100;
    nanosleep(&req, NULL);
}

static void dev_rtc72421_fixedperiod()
{
    if (reg_RTC72421.IRQ_FLAG || reg_RTC72421.MASK ) return;
    reg_RTC72421.IRQ_FLAG = 1;
    reg_RTC72421.pulseCount = (reg_RTC72421.ITRPT_STND)?0:1; /* 1 for IRQ_FLAG will be down to 0 after half of 1/64 second when STND (pulse) mode*/
    bs3_bus_setinterrupt(dev_rtc72421.interruptNumber);
}

/* this function is invoked by an independant thread */
static void * dev_rtc72421_run(void * bs3_device_bus) /* thread dedicated function */
{
    int count;
    int trig;
    int prevSecond;
    BYTE carry;
    count = 0;
    trig = 0;

    /* init with current host RTC 24h format */
    time_t t = time(NULL);
    struct tm *ptr_tm = localtime(&t);
    /* register D */
    reg_RTC72421.ADJ30s     = 0; /* no 30 second adjustment */
    reg_RTC72421.IRQ_FLAG   = 0; /* no active interupt signal */
    reg_RTC72421.BUSY       = 1; /* alway busy until HOLD == 1 */
    reg_RTC72421.HOLD       = 0; /* no request for stop time/date clock register external update */
    /* register E */
    reg_RTC72421.t0_1       = 0; /* 0== 1/64 second, 1== 1 second, 2== 1 minute, 3 = 1 hour */
    reg_RTC72421.ITRPT_STND = 0; /* 1== ITRPT mode, 0 == STND mode  */
    reg_RTC72421.MASK       = 1; /* 0 == ITRPT_STND enabled, 1 = disabled : default no signal or interrupt */
    /* register F */
    reg_RTC72421.TEST       = 0; /* must be always 0 */
    reg_RTC72421._24_12     = 1; /* 1 == 24 h, 0 = 12 h AM/PM */
    reg_RTC72421.STOP       = 0; /* 0 == clock running, 1 = Clock stop */
    reg_RTC72421.RESET      = 0; /* 0 == no reset, 1== reset requested */
    /* S1 to W registers */
    reg_RTC72421.S1         = (BYTE)( ptr_tm->tm_sec         % 10);
    reg_RTC72421.S10        = (BYTE)( ptr_tm->tm_sec         / 10);
    reg_RTC72421.MI1        = (BYTE)( ptr_tm->tm_min         % 10);
    reg_RTC72421.MI10       = (BYTE)( ptr_tm->tm_min         / 10);
    reg_RTC72421.H1         = (BYTE)( ptr_tm->tm_hour        % 10);
    reg_RTC72421.H10        = (BYTE)( ptr_tm->tm_hour        / 10);
    reg_RTC72421.D1         = (BYTE)( ptr_tm->tm_mday        % 10);
    reg_RTC72421.D10        = (BYTE)( ptr_tm->tm_mday        / 10);
    reg_RTC72421.MO1        = (BYTE)((ptr_tm->tm_mon  + 1)   % 10);
    reg_RTC72421.MO1        = (BYTE)((ptr_tm->tm_mon  + 1)   / 10);
    reg_RTC72421.Y1         = (BYTE)((ptr_tm->tm_year % 100) % 10);
    reg_RTC72421.Y1         = (BYTE)((ptr_tm->tm_year % 100) / 10);
    reg_RTC72421.W          = (BYTE)( ptr_tm->tm_wday            );

    prevSecond = ptr_tm->tm_sec;
    while (1)
    {
       
        dev_rtc72421_sleep_tenthofmicroseconds(78125); /* half of 1/64 second sleep ? 122070312 nano ? 1/8192 second ? */
        if (reg_RTC72421.STOP || reg_RTC72421.RESET) continue;
        if (reg_RTC72421.pulseCount)
        {
            reg_RTC72421.pulseCount--;
            if (!reg_RTC72421.pulseCount) reg_RTC72421.IRQ_FLAG = 0;
        }
        count++;
        if ((count & 0x0003) == 2 && reg_RTC72421.t0_1 == 0 ) dev_rtc72421_fixedperiod(); /* may be */
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
        if (count == 32) 
        {
            t = time(NULL);
            ptr_tm = localtime(&t);
            if (ptr_tm->tm_sec != prevSecond)
            {
                trig = 1;
                prevSecond = ptr_tm->tm_sec;
            }
            count = 0;
        }
        if (!reg_RTC72421.HOLD)
        {
            reg_RTC72421.BUSY = 1;
            if (trig)
            {
                trig = 0;
                if (reg_RTC72421.t0_1 == 1 ) dev_rtc72421_fixedperiod(); /* second fixed period */
                carry = ((++reg_RTC72421.S1)>9)?1:0; /*Incr 1 second, carry if above or equal 10 */
                if (carry)
                {
                    reg_RTC72421.S1 = 0; /* reset the unit second */
                    carry = ((++reg_RTC72421.S10)>5)?1:0; /*incr 10 seconds, carry if above or equal 60 */
                    if (carry)
                    {
                        if (reg_RTC72421.t0_1 == 2 ) dev_rtc72421_fixedperiod(); /* Minute fixed period*/
                        reg_RTC72421.S10 = 0; /* reset the 10 seconds*/
                        carry = ((++reg_RTC72421.MI1)>9)?1:0; /*Incr 1 minute, carry if above or equal 10 */
                        if (carry) /* go over minute*/
                        {
                            reg_RTC72421.MI1 = 0;
                            carry = ((++reg_RTC72421.MI10)>5)?1:0; /*incr 10 minutes, carry if above or equal 60 */
                            if (carry) /* if go over hour */
                            {
                                if (reg_RTC72421.t0_1 == 3 ) dev_rtc72421_fixedperiod(); /* hour fixed period*/
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
    return NULL;
}


int dev_rtc72421_stop()
{
  int result = pthread_kill(thread_RTC72421, 0);
  created_thread_RTC72421 = 0;
  if (result == ESRCH) {
    return result;
  } else if (result == 0) {
    pthread_cancel(thread_RTC72421); 
    pthread_join(thread_RTC72421, NULL);
    return 0;
  }
}

int dev_rtc72421_start()
{
    if (created_thread_RTC72421) dev_rtc72421_stop(); /* just to avoid double start without stop */
    int result = pthread_create(&thread_RTC72421, NULL, &dev_rtc72421_run, NULL); 
    if (result == 0) created_thread_RTC72421 = 1;
    return result;
}

/* declare RTC-72421 device */

struct bs3_device dev_rtc72421 = 
{
    .name="RTC-72421",
    .address=0x0110,
    .mask=0xFFF0, /* 16 RTC I/O registers */
    .startdevice = &dev_rtc72421_start,
    .stopdevice = &dev_rtc72421_stop,
    .readByte = &dev_rtc72421_read,
    .writeByte = &dev_rtc72421_write,
    .interruptNumber = 8 /* interrupt vector 8 */
};
