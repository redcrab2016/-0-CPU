#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#include "bs3_bus.h"

struct dev_bs3gfx
{
    union 
    {
        BYTE pb[8];
        struct 
        {
            BYTE cmd; /* write only*/
            BYTE pb1;
            BYTE pb2;
            BYTE pb3;
            BYTE pb4;
            BYTE pb5;
            BYTE pb6;
            BYTE pb7;
        };
    };
    union 
    {
        WORD pw[8];
        struct 
        {
            WORD cmdw;
            WORD pw1;
            WORD pw2;
            WORD pw3;
            WORD pw4;
            WORD pw5;
            WORD pw6;
            WORD pw7;
        };
    };
    union 
    {
        BYTE status;
        struct 
        {
            BYTE COMMAND_STATUS_CODE:5;
            BYTE ENABLE:1;
            BYTE WAITFORDATA:1;
            BYTE BUSY:1;
        };
        
    };
    BYTE videoram [2][65536]; /* 2 surfaces of 64KiB (256 width x 256 height  x 8bpp)*/
    WORD viewport_location; /* MSB Y location (0 to 255), LSB X location (0 to 255)*/
    WORD viewport_size; /* MSB Height (1 to 100) LSB Width (1 to 160) */
    BYTE viewport_surface;
    BYTE viewport_shadow[16000]; /* shadow of what is on screen */
};

#define BS3_GFX_COMMAND_RESET                   0x00
#define BS3_GFX_COMMAND_END                     0xFF
#define BS3_GFX_COMMAND_REFRESH                 0x01
#define BS3_GFX_COMMAND_VIEWPORT_CONFIG         0x02
#define BS3_GFX_COMMAND_VIEWPORT_CLEAR          0x03
#define BS3_GFX_COMMAND_SURFACE_GETPIXEL        0x04
#define BS3_GFX_COMMAND_SURFACE_SETPIXEL        0x05
#define BS3_GFX_COMMAND_SURFACE_DRAW_HLINE      0x06
#define BS3_GFX_COMMAND_SURFACE_DRAW_VLINE      0x07
#define BS3_GFX_COMMAND_SURFACE_DRAW_BOX        0x08
#define BS3_GFX_COMMAND_SURFACE_DRAW_BOXFULL    0x09
#define BS3_GFX_COMMAND_SURFACE_BLIT_OPERATOR   0x0A
#define BS3_GFX_COMMAND_SURFACE_BLIT_KEYCOLOR   0x0B
#define BS3_GFX_COMMAND_SURFACE_BLIT_TRANSFER   0x0C



#define BS3_GFX_REGISTER_COMMAND    0x00
#define BS3_GFX_REGISTER_PB1        0x01
#define BS3_GFX_REGISTER_PB2        0x02
#define BS3_GFX_REGISTER_PB3        0x03
#define BS3_GFX_REGISTER_PB4        0x04
#define BS3_GFX_REGISTER_PB5        0x05
#define BS3_GFX_REGISTER_PB6        0x06
#define BS3_GFX_REGISTER_PB7        0x07

#define BS3_GFX_REGISTER_PW1        0x01
#define BS3_GFX_REGISTER_PW2        0x02
#define BS3_GFX_REGISTER_PW3        0x03
#define BS3_GFX_REGISTER_PW4        0x04
#define BS3_GFX_REGISTER_PW5        0x05
#define BS3_GFX_REGISTER_PW6        0x06
#define BS3_GFX_REGISTER_PW7        0x07

#define BS3_GFX_STATUS_OK               0x00
#define BS3_GFX_STATUS_UNKNOWN_CMD      0x1F
#define BS3_GFX_STATUS_NOTENABLED       0x01
#define BS3_GFX_STATUS_BADVPSIZE        0x02
#define BS3_GFX_STATUS_BADSURFACE       0x03
#define BS3_GFX_STATUS_BADOPERATOR      0x04
#define BS3_GFX_STATUS_BROKENTRANSFER   0x05

/* set of BS3 GFX command function declaration*/
void bs3_gfx_command_reset();
void bs3_gfx_command_end();
void bs3_gfx_command_refresh();
void bs3_gfx_command_viewport_config();
void bs3_gfx_command_viewport_clear();
void bs3_gfx_command_surface_getpixel();
void bs3_gfx_command_surface_setpixel();
void bs3_gfx_command_surface_draw_hline();
void bs3_gfx_command_surface_draw_vline();
void bs3_gfx_command_surface_draw_box();
void bs3_gfx_command_surface_draw_boxfull();
void bs3_gfx_command_surface_blit_operator();
void bs3_gfx_command_surface_blit_keycolor();
void bs3_gfx_command_surface_blit_transfer();
void bs3_gfx_command_surface_blit_transfer_data();

extern struct bs3_device dev_bs3gfx;
int endGfx;
pthread_t thread_bs3gfx; 
int created_thread_bs3gfx = 0;
struct dev_bs3gfx reg_bs3gfx;
pthread_cond_t condWakeUp = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t lockGfx = PTHREAD_MUTEX_INITIALIZER; 

BYTE dev_bs3gfx_readByte(WORD address)
{
    WORD addr;
    addr = address & 0x0007;
    if (addr == 0) return reg_bs3gfx.status;
    return reg_bs3gfx.pb[addr];
}

void dev_bs3gfx_writeByte(WORD address, BYTE data)
{
    WORD addr;
    addr = address & 0x0007;
    if (reg_bs3gfx.BUSY) return; /* ignore write if busy */
    pthread_mutex_lock(&lockGfx);
    if (addr == BS3_GFX_REGISTER_COMMAND) /* write command case */
    {
        if (!reg_bs3gfx.WAITFORDATA || (reg_bs3gfx.WAITFORDATA && data == BS3_GFX_COMMAND_RESET))
        {
            reg_bs3gfx.cmd = data;
            reg_bs3gfx.BUSY = 1;
            pthread_cond_signal(&condWakeUp);
        }
    } 
    else /* Manage BLIT transfer */
    {
        if (reg_bs3gfx.WAITFORDATA && addr == BS3_GFX_REGISTER_PB3)
        {
           reg_bs3gfx.pb[addr] = data;
           reg_bs3gfx.BUSY = 1; 
           pthread_cond_signal(&condWakeUp); 
        }
        else
        {
            if (!reg_bs3gfx.WAITFORDATA)
            {
                reg_bs3gfx.pb[addr] = data;
            } /* else ignore write to register */
        }
    }
    pthread_mutex_lock(&lockGfx);

}

WORD dev_bs3gfx_readWord(WORD address)
{
    WORD addr;
    addr = address & 0x0007;
    if (addr == BS3_GFX_REGISTER_COMMAND) return (WORD)reg_bs3gfx.status; /* status in a word */
    return reg_bs3gfx.pw[addr];
}

void dev_bs3gfx_writeWord(WORD address, WORD data)
{
    WORD addr;
    addr = address & 0x0007;
    if (reg_bs3gfx.BUSY) return;
    pthread_mutex_lock(&lockGfx);
    if (addr == BS3_GFX_REGISTER_COMMAND)
    {
        return; /* no word write on command register : ignore*/
    } 
    else /* TODO manage BLIT transfer */
    {
        if (reg_bs3gfx.WAITFORDATA && addr == BS3_GFX_REGISTER_PW3)
        {
            reg_bs3gfx.WAITFORDATA = 0;
            reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BROKENTRANSFER;
        } 
        else
        {   
            if (!reg_bs3gfx.WAITFORDATA)
            {
                reg_bs3gfx.pb[addr] = data;
            } /* else ignore write */
        }
    }
    pthread_mutex_unlock(&lockGfx);
}

/* Utils functions for BS3 GFX command execution */


/* BS3 GFX command function implementation */
void bs3_gfx_command_reset()
{
    
}

void bs3_gfx_command_end()
{
    
}

void bs3_gfx_command_refresh()
{
    
}

void bs3_gfx_command_viewport_config()
{
    
}

void bs3_gfx_command_viewport_clear()
{
    
}

void bs3_gfx_command_surface_getpixel()
{
    
}

void bs3_gfx_command_surface_setpixel()
{
    
}

void bs3_gfx_command_surface_draw_hline()
{
    
}

void bs3_gfx_command_surface_draw_vline()
{
    
}

void bs3_gfx_command_surface_draw_box()
{
    
}

void bs3_gfx_command_surface_draw_boxfull()
{
    
}

void bs3_gfx_command_surface_blit_operator()
{
    
}

void bs3_gfx_command_surface_blit_keycolor()
{
    
}

void bs3_gfx_command_surface_blit_transfer()
{
    
}

void bs3_gfx_command_surface_blit_transfer_data()
{
    
}

/* function executed by a dedicated thread to perform the device activities */
static void * dev_bs3gfx_run(void * bs3_device_bus)
{
    endGfx = 0;
    pthread_mutex_lock(&lockGfx);
    while (!endGfx)
    {
        reg_bs3gfx.BUSY = 0;
        pthread_cond_wait(&condWakeUp,&lockGfx);
        if (reg_bs3gfx.WAITFORDATA && reg_bs3gfx.cmd != BS3_GFX_COMMAND_RESET)
        {
            switch (reg_bs3gfx.cmd)
            {
                case BS3_GFX_COMMAND_SURFACE_BLIT_TRANSFER:
                    bs3_gfx_command_surface_blit_transfer_data();
                    break;
                default: /* should never occur */
                    reg_bs3gfx.WAITFORDATA = 0;
                    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BROKENTRANSFER;                
            }
        }
        else /* normal command excution */
        {
            if (reg_bs3gfx.ENABLE)
            {
                switch (reg_bs3gfx.cmd)
                {
                    case BS3_GFX_COMMAND_RESET:
                        bs3_gfx_command_reset();
                        break;
                    case BS3_GFX_COMMAND_END:
                        bs3_gfx_command_end();
                        break;
                    case BS3_GFX_COMMAND_REFRESH:
                        bs3_gfx_command_refresh();
                        break;
                    case BS3_GFX_COMMAND_VIEWPORT_CONFIG:
                        bs3_gfx_command_viewport_config();
                        break;
                    case BS3_GFX_COMMAND_VIEWPORT_CLEAR:
                        bs3_gfx_command_viewport_clear();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_GETPIXEL:
                        bs3_gfx_command_surface_getpixel();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_SETPIXEL:
                        bs3_gfx_command_surface_setpixel();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_DRAW_HLINE:
                        bs3_gfx_command_surface_draw_hline();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_DRAW_VLINE:
                        bs3_gfx_command_surface_draw_vline();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_DRAW_BOX:
                        bs3_gfx_command_surface_draw_box();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_DRAW_BOXFULL:
                        bs3_gfx_command_surface_draw_boxfull();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_BLIT_OPERATOR:
                        bs3_gfx_command_surface_blit_operator();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_BLIT_KEYCOLOR:
                        bs3_gfx_command_surface_blit_keycolor();
                        break;
                    case BS3_GFX_COMMAND_SURFACE_BLIT_TRANSFER:
                        bs3_gfx_command_surface_blit_transfer();
                        break;
                    default:
                        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_UNKNOWN_CMD;
                }
            }
            else
            {
                reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_NOTENABLED;
            }
        }
    }
    pthread_mutex_unlock(&lockGfx);
    return NULL;
}

int dev_bs3gfx_stop()
{
    int resultGfx ;
    if (created_thread_bs3gfx)   resultGfx = pthread_kill(thread_bs3gfx, 0);

    if (resultGfx == 0 && created_thread_bs3gfx) 
    {
        endGfx = 1;
        
        pthread_join(thread_bs3gfx, NULL);
    }
    created_thread_bs3gfx = 0;
    return resultGfx;}

int dev_bs3gfx_start()
{
    int resultGFX;
    if (created_thread_bs3gfx) dev_bs3gfx_stop();
    memset(&reg_bs3gfx,0, sizeof(struct dev_bs3gfx)); /* set to 0 the registers */
    resultGFX = pthread_create(&thread_bs3gfx, NULL, &dev_bs3gfx_run, NULL); 
    if (resultGFX == 0) created_thread_bs3gfx = 1;
    if (resultGFX) return resultGFX;
    return 0;
}

struct bs3_device dev_bs3gfx = 
{
    .name="BS3-GFX",
    .address=0x0120,
    .mask=0xFFF8, /* 8 BS3 GFX I/O registers */
    .startdevice = &dev_bs3gfx_start,
    .stopdevice = &dev_bs3gfx_stop,
    .readByte = &dev_bs3gfx_readByte,
    .writeByte = &dev_bs3gfx_writeByte,
    .readWord = &dev_bs3gfx_readWord,
    .writeWord = &dev_bs3gfx_writeWord,
    .interruptNumber = 0 /* no interrupt vector */
};
