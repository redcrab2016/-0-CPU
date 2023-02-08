#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h>
#include <time.h>
#include <signal.h>

#include "bs3_bus.h"

struct dev_bs3inout {
    union 
    {
        BYTE reg[8];
        struct 
        {
            BYTE IN;
            BYTE INSTATUS;
            BYTE OUT;
            BYTE OUTSTATUS;
            BYTE OUT2;
            BYTE OUT2STATUS;
            BYTE reserved1;
            BYTE reserved2;
        };
    };
};

struct dev_bs3inout reg_bs3inout;

int endIN; 
int endOUT;
int endOUT2;
pthread_cond_t condIN = PTHREAD_COND_INITIALIZER; 
pthread_cond_t condOUT = PTHREAD_COND_INITIALIZER; 
pthread_cond_t condOUT2 = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t lockIN = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lockOUT = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lockOUT2 = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lockINSTATUS = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lockOUTSTATUS = PTHREAD_MUTEX_INITIALIZER; 
pthread_mutex_t lockOUT2STATUS = PTHREAD_MUTEX_INITIALIZER; 

pthread_t thread_bs3in; 
int created_thread_bs3in = 0;
pthread_t thread_bs3out; 
int created_thread_bs3out = 0;
pthread_t thread_bs3out2; 
int created_thread_bs3out2 = 0;

#define QUEUE_SIZE 262144

BYTE queueOUT[QUEUE_SIZE];
long queueOUT_rear = -1;
long queueOUT_front = 0;
long queueOUT_count = 0;

BYTE queueOUT2[QUEUE_SIZE];
long queueOUT2_rear = -1;
long queueOUT2_front = 0;
long queueOUT2_count = 0;

long enqueueOUT(BYTE value)
{
    if (queueOUT_count >= QUEUE_SIZE) return -1;
    queueOUT_rear = (queueOUT_rear + 1) % QUEUE_SIZE;
    queueOUT[queueOUT_rear] = value;
    queueOUT_count++;
    return queueOUT_count;
}

long enqueueOUT2(BYTE value)
{
    if (queueOUT2_count >= QUEUE_SIZE) return -1;
    queueOUT2_rear = (queueOUT2_rear + 1) % QUEUE_SIZE;
    queueOUT2[queueOUT2_rear] = value;
    queueOUT2_count++;
    return queueOUT2_count;
}

long dequeueOUT()
{
    if (queueOUT_count == 0) return -1;
    BYTE item = queueOUT[queueOUT_front];
    queueOUT[queueOUT_front] = -1;/* clean zombi queue value */
    queueOUT_front = (queueOUT_front + 1) % QUEUE_SIZE;
    queueOUT_count--;
    return item;
}

long dequeueOUT2()
{
    if (queueOUT2_count == 0) return -1;
    BYTE item = queueOUT2[queueOUT2_front];
    queueOUT2[queueOUT2_front] = -1;/* clean zombi queue value */
    queueOUT2_front = (queueOUT2_front + 1) % QUEUE_SIZE;
    queueOUT2_count--;
    return item;
}

extern struct bs3_device dev_bs3inout;

BYTE dev_bs3inout_read(WORD address)
{
    int addr = address & 0x0007;
    BYTE value; 
    //  
    switch (addr)
    {
        case 0x0000 :
            pthread_mutex_lock(&lockINSTATUS);
            if (reg_bs3inout.INSTATUS == 0x00) /* waiting for consumption*/
            {
                value = reg_bs3inout.reg[addr];
                reg_bs3inout.INSTATUS = 0x01; /* input consummed */
                pthread_cond_signal(&condIN); /* signal input consumption */
            } 
            else value = reg_bs3inout.reg[addr];
            pthread_mutex_unlock(&lockINSTATUS);
            break;
        case 0x0001:
            value = reg_bs3inout.reg[addr];
            break;
        case 0x0002:
            value = reg_bs3inout.reg[addr];
            break;
        case 0x0003:
            value = reg_bs3inout.reg[addr];
            break;
        case 0x0004:
            value = reg_bs3inout.reg[addr];
            break;
        case 0x0005:
            value = reg_bs3inout.reg[addr];
            break;
        default:
            value = reg_bs3inout.reg[addr];
            break;
    }
    
    return value;
}

void dev_bs3inout_write(WORD address, BYTE data)
{
    int addr;
    BYTE value;
    int trylock;
    addr = address & 0x0007;
    value = data;
    switch(addr)
    {
        case 0: /* write to IN data */
        case 1: /* write to IN status */
        case 3: /* write to OUT status */
        case 5: /* write to OUT2 status */
            return; /* write is ignored */
        case 2:
            if ( !reg_bs3inout.OUTSTATUS)  /* ignore write when output is not ready for any reason */
            {
                pthread_mutex_lock(&lockOUTSTATUS);
                reg_bs3inout.reg[addr] = value;
                enqueueOUT(value);
                if (value == 10 || queueOUT_count == QUEUE_SIZE)
                {
                    reg_bs3inout.OUTSTATUS = 0x01; /* pending for sending */
                    pthread_mutex_lock(&lockOUT);
                    pthread_cond_signal(&condOUT);
                    pthread_mutex_unlock(&lockOUT);
                }
                pthread_mutex_unlock(&lockOUTSTATUS);
            }
            return;
        case 4:
            
            if ( !reg_bs3inout.OUT2STATUS)  /* ignore write when output2 is not ready for any reason */
            {
                pthread_mutex_lock(&lockOUT2STATUS);
                reg_bs3inout.reg[addr] = value;
                enqueueOUT2(value);
                if (value == 10 || queueOUT2_count == QUEUE_SIZE)
                {
                    reg_bs3inout.OUT2STATUS = 0x01; /* pending for sending */
                    pthread_mutex_lock(&lockOUT2);
                    pthread_cond_signal(&condOUT2);
                    pthread_mutex_unlock(&lockOUT2);
                }
                pthread_mutex_unlock(&lockOUT2STATUS);
            }
            
            return;
        default:
            reg_bs3inout.reg[addr] = value; /* reserved IO as memory*/
    }
    
}


/* this function is invoked by an independant thread */
static void * dev_bs3in_run(void * bs3_device_bus) /* thread dedicated function */
{
    ssize_t status;
    reg_bs3inout.INSTATUS = 0x01; /* at start we consider that program is waiting for input*/
    endIN = 0;
    while (!endIN)
    {
        switch( reg_bs3inout.INSTATUS )
        {
            case 0x01: /* input consummed*/
                status = read(0,& reg_bs3inout.IN, 1);
                switch (status)
                {
                case 1: /* read one byte */
                    pthread_mutex_lock(&lockINSTATUS);
                    reg_bs3inout.INSTATUS = 0x00; /* Data ready to be consummed */
                    bs3_bus_setinterrupt(dev_bs3inout.interruptNumber);
                    pthread_mutex_unlock(&lockINSTATUS);
                    break;
                case 0: /* EOF */
                    reg_bs3inout.INSTATUS = 0xFF; /* No input data available */
                    break;
                case -1: /* error */
                    switch (errno)
                    {
                        case EINTR:
                            /* do nothing , we've to retry*/
                            break;
                        default:
                            /* serious context : then invalidate the Core IN reading */
                            reg_bs3inout.INSTATUS = 0xFF; /* No input data available */
                            break;
                    }
                    break;
                }
                break;
            case 0x00:
                pthread_mutex_lock(&lockIN);
                pthread_cond_wait(&condIN, &lockIN); /* wait for input */
                pthread_mutex_unlock(&lockIN);
                break;
            case 0xFF:
                /* nothing to do when "No input data available "*/
                sleep(1);
            default:
                break;
        }
    } /* end of while forever */
    return NULL;
}

void dev_bs3out_flush()
{
    int status;
    long value;
    int retry;
    while ((value = dequeueOUT()) != -1) /* send the pending characters in FIFO queue*/
    {        
        do
        {
            retry = 0;
            status = fputc((int)value,stdout);
            switch (status)
            {
                case EOF:
                    switch(errno)
                    {
                        case EAGAIN : /* retry is needed */
                            retry = 1;
                            break;
                        default:
                            reg_bs3inout.OUTSTATUS = 0xFF; /* output not available anymore */
                    }
                    break;
                default:
                    break;
            }
        } while (retry);
    }

}

void dev_bs3out2_flush()
{
    int status;
    long value;
    int retry;
    while ((value = dequeueOUT2()) != -1) /* send the pending characters in FIFO queue*/
    {        
        do
        {
            retry = 0;
            status = fputc((int)value,stderr);
            switch (status)
            {
                case EOF:
                    switch(errno)
                    {
                        case EAGAIN : /* retry is needed */
                            retry = 1;
                            break;
                        default:
                            reg_bs3inout.OUT2STATUS = 0xFF; /* output not available anymore */
                    }
                    break;
                default:
                    break;
            }
        } while (retry);
    }

}


/* this function is invoked by an independant thread */
static void * dev_bs3out_run(void * bs3_device_bus) /* thread dedicated function */
{
    reg_bs3inout.OUTSTATUS = 0x00; /* ready to send something */
    endOUT = 0;
    while (!endOUT)
    {
        pthread_mutex_lock(&lockOUTSTATUS);
        switch (reg_bs3inout.OUTSTATUS)
        {
            case 0x01: /* there is somthing to send */
                dev_bs3out_flush();
                reg_bs3inout.OUTSTATUS = (reg_bs3inout.OUTSTATUS == 0x01)?0x00:reg_bs3inout.OUTSTATUS; /* character(s) sent , FIFO queue is empty*/
                break;
            case 0x00: /* wait for something to send*/
                pthread_mutex_lock(&lockOUT);
                pthread_mutex_unlock(&lockOUTSTATUS);
                if (!reg_bs3inout.OUTSTATUS ) pthread_cond_wait(&condOUT, &lockOUT); /* wait for input */
                pthread_mutex_unlock(&lockOUT);
                pthread_mutex_lock(&lockOUTSTATUS);
                break;
            case 0xFF: /* no output available */
                sleep (1);
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&lockOUTSTATUS);
    } /* end of while forever */
    return NULL;
}

/* this function is invoked by an independant thread */
static void * dev_bs3out2_run(void * bs3_device_bus) /* thread dedicated function */
{
    reg_bs3inout.OUT2STATUS = 0x00; /* ready to send something */
    int status;
    long value;
    int retry;    
    endOUT2 = 0;
    while (!endOUT2)
    {
        pthread_mutex_lock(&lockOUT2STATUS);
        switch (reg_bs3inout.OUT2STATUS)
        {
            case 0x01: /* there is somthing to send */
                dev_bs3out2_flush();
                reg_bs3inout.OUT2STATUS = (reg_bs3inout.OUT2STATUS == 0x01)?0x01:reg_bs3inout.OUT2STATUS; /* character(s) sent , FIFO queue is empty*/
                break;
            case 0x00: /* wait for something to send*/
                pthread_mutex_lock(&lockOUT2);
                pthread_mutex_unlock(&lockOUT2STATUS);
                if (!reg_bs3inout.OUT2STATUS) pthread_cond_wait(&condOUT2, &lockOUT2); /* wait for input */
                pthread_mutex_unlock(&lockOUT2);
                pthread_mutex_lock(&lockOUT2STATUS);
                break;
            case 0xFF: /* no output available */
                sleep (1);
                break;
            default:
                break;
        }
        pthread_mutex_unlock(&lockOUT2STATUS);
    } /* end of while forever */
    return NULL;
}


int dev_bs3inout_stop()
{
  int resultin ;
  int resultout;
  int resultout2;
  
  if (created_thread_bs3in)   resultin   = pthread_kill(thread_bs3in, 0);
  if (created_thread_bs3out)  resultout  = pthread_kill(thread_bs3out, 0);
  if (created_thread_bs3out2) resultout2 = pthread_kill(thread_bs3out2, 0);

  if (resultin == 0 && created_thread_bs3in) 
  {
    endIN = 1;
    pthread_kill(thread_bs3in, 0);
    pthread_cond_signal(&condIN);
    //sleep(1);
    pthread_cancel(thread_bs3in); 
    pthread_join(thread_bs3in, NULL);
  }

  if (resultout == 0 && created_thread_bs3out) 
  {
    endOUT = 1;
    pthread_cond_signal(&condOUT);
    pthread_join(thread_bs3out, NULL);
    dev_bs3out_flush();
  }

  if (resultout2 == 0 && created_thread_bs3out2) 
  {
    endOUT2 = 1;
    pthread_cond_signal(&condOUT2);
    pthread_join(thread_bs3out2, NULL);
    dev_bs3out2_flush();
  }

  created_thread_bs3in = 0;
  created_thread_bs3out = 0;
  created_thread_bs3out2 = 0;

  if (resultin == ESRCH || resultout == ESRCH || resultout2 == ESRCH) 
  {
    return ESRCH;
  } 
  return 0;
}

int dev_bs3inout_start()
{
  int resultin ;
  int resultout;
  int resultout2;

    if (created_thread_bs3in || created_thread_bs3out || created_thread_bs3out2) dev_bs3inout_stop(); /* just to avoid double start without stop */
    resultin = pthread_create(&thread_bs3in, NULL, &dev_bs3in_run, NULL); 
    if (resultin == 0) created_thread_bs3in = 1;
    resultout = pthread_create(&thread_bs3out, NULL, &dev_bs3out_run, NULL); 
    if (resultout == 0) created_thread_bs3out = 1;
    resultout2 = pthread_create(&thread_bs3out2, NULL, &dev_bs3out2_run, NULL); 
    if (resultout2 == 0) created_thread_bs3out2 = 1;
    if (resultin) return resultin;
    if (resultout) return resultout;
    if (resultout2) return resultout2;
    return 0;
}

/* declare BS3 in-out-out2 device */

struct bs3_device dev_bs3inout = 
{
    .name="BS3-IN-OUT-OUT2",
    .address=0x0100,
    .mask=0xFFF8, /* 8 BS3 I/O registers (6 in/out + 2 reserved) */
    .startdevice = &dev_bs3inout_start,
    .stopdevice = &dev_bs3inout_stop,
    .readByte = &dev_bs3inout_read,
    .writeByte = &dev_bs3inout_write,
    .interruptNumber = 4 /* interrupt vector 4 */
};
