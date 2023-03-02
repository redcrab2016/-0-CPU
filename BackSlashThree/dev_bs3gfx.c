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
            WORD cmdw; /* write forbidden */
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
    BYTE videoram [2][65536];       /* 2 surfaces of 64KiB (256 width x 256 height  x 8bpp) */
    WORD viewport_location;         /* MSB Y location (0 to 255), LSB X location (0 to 255) */
    WORD viewport_size;             /* MSB Height (1 to 100) LSB Width (1 to 160)           */
    BYTE viewport_surface;
    BYTE viewport_shadow[16000];    /* shadow of what is on screen                          */
    BYTE colorPalette[256];
};

#define BS3_GFX_COMMAND_RESET                   0x00
#define BS3_GFX_COMMAND_END                     0xFF
#define BS3_GFX_COMMAND_REFRESH                 0x01
#define BS3_GFX_COMMAND_VIEWPORT_CONFIG         0x02
#define BS3_GFX_COMMAND_VIEWPORT_GETCONFIG      0x03
#define BS3_GFX_COMMAND_VIEWPORT_CLEAR          0x04
#define BS3_GFX_COMMAND_SURFACE_GETPIXEL        0x05
#define BS3_GFX_COMMAND_SURFACE_SETPIXEL        0x06
#define BS3_GFX_COMMAND_SURFACE_DRAW_HLINE      0x07
#define BS3_GFX_COMMAND_SURFACE_DRAW_VLINE      0x08
#define BS3_GFX_COMMAND_SURFACE_DRAW_BOX        0x09
#define BS3_GFX_COMMAND_SURFACE_DRAW_BOXFULL    0x0A
#define BS3_GFX_COMMAND_SURFACE_BLIT_OPERATOR   0x0B
#define BS3_GFX_COMMAND_SURFACE_BLIT_KEYCOLOR   0x0C
#define BS3_GFX_COMMAND_SURFACE_BLIT_TRANSFER   0x0D

#define BS3_GFX_COMMAND_BLIT_OPERATOR_COPY      0x00
#define BS3_GFX_COMMAND_BLIT_OPERATOR_OR        0x01
#define BS3_GFX_COMMAND_BLIT_OPERATOR_EOR       0x02
#define BS3_GFX_COMMAND_BLIT_OPERATOR_AND       0x03
#define BS3_GFX_COMMAND_BLIT_OPERATOR_ADD       0x04
#define BS3_GFX_COMMAND_BLIT_OPERATOR_SUB       0x05
#define BS3_GFX_COMMAND_BLIT_OPERATOR_MUL       0x06

#define BS3_GFX_REGISTER_COMMAND                0x00
#define BS3_GFX_REGISTER_STATUS                 0x00
#define BS3_GFX_REGISTER_PB1                    0x01
#define BS3_GFX_REGISTER_PB2                    0x02
#define BS3_GFX_REGISTER_PB3                    0x03
#define BS3_GFX_REGISTER_PB4                    0x04
#define BS3_GFX_REGISTER_PB5                    0x05
#define BS3_GFX_REGISTER_PB6                    0x06
#define BS3_GFX_REGISTER_PB7                    0x07

#define BS3_GFX_REGISTER_PW1                    0x01
#define BS3_GFX_REGISTER_PW2                    0x02
#define BS3_GFX_REGISTER_PW3                    0x03
#define BS3_GFX_REGISTER_PW4                    0x04
#define BS3_GFX_REGISTER_PW5                    0x05
#define BS3_GFX_REGISTER_PW6                    0x06
#define BS3_GFX_REGISTER_PW7                    0x07

#define BS3_GFX_STATUS_OK                       0x00
#define BS3_GFX_STATUS_UNKNOWN_CMD              0x1F
#define BS3_GFX_STATUS_NOTENABLED               0x01
#define BS3_GFX_STATUS_BADVPSIZE                0x02
#define BS3_GFX_STATUS_BADSURFACE               0x03
#define BS3_GFX_STATUS_BADOPERATOR              0x04
#define BS3_GFX_STATUS_BROKENTRANSFER           0x05

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
static pthread_cond_t condWakeUp = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t lockGfx = PTHREAD_MUTEX_INITIALIZER; 


BYTE dev_bs3gfx_readByte(WORD address)
{
    WORD addr;
    addr = address & 0x0007;
    if (addr == BS3_GFX_REGISTER_STATUS) return reg_bs3gfx.status;
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
        if (!reg_bs3gfx.WAITFORDATA                                    ||
            (reg_bs3gfx.WAITFORDATA && data == BS3_GFX_COMMAND_RESET)  ||
            (reg_bs3gfx.WAITFORDATA && data == BS3_GFX_COMMAND_END))
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
    pthread_mutex_unlock(&lockGfx);
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
    else /* Manage BLIT transfer */
    {
        if (reg_bs3gfx.WAITFORDATA && addr == BS3_GFX_REGISTER_PW3) /* break data transfer */
        {
            reg_bs3gfx.WAITFORDATA = 0;
            reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BROKENTRANSFER;
        } 
        else
        {   
            if (!reg_bs3gfx.WAITFORDATA)
            {
                reg_bs3gfx.pw[addr] = data;
            } /* else ignore write */
        }
    }
    pthread_mutex_unlock(&lockGfx);
}

/* Utils functions for BS3 GFX command execution */

/* Utils BS3 GFX functions for ANSI screen */
#include "ansi_esc.h"

void _bs3_gfx_invalidateshadow()
{
    int x,y;
    int w,h;
    WORD addrShadow;
    BYTE surface;
    WORD addrSurface;

    addrSurface = reg_bs3gfx.viewport_location;
    surface     = reg_bs3gfx.viewport_surface;
    addrShadow = 0;
    w = (int)(reg_bs3gfx.viewport_size & 0x00FE);
    h = (int)((reg_bs3gfx.viewport_size & 0xFE00) >> 8);

    for (y = 0; y < h ; y++)
    {
        for (x = 0; x < w ; x++)
        {
            reg_bs3gfx.viewport_shadow[addrShadow] = (BYTE)(reg_bs3gfx.videoram[surface][(addrSurface + x) & 0x0FFFF] + 1);
            addrShadow++;
        }
        addrSurface += 256;
        addrSurface = addrSurface & 0x0FFFF; /* stay in surface */
    }
}

void _bs3_gfx_setpalette()
{
    int i;
    for (i = 0 ; i < 256; i++)
        reg_bs3gfx.colorPalette[i] = (BYTE)i;
    /* IBM VGA 16 color palette
        index   Name (IBM)      Red (0..5)  Green(0..5) Blue(0..5)  (equivalent to ANSI 256 colors Index)
        $00     Black           0           0           0           16
        $01     Blue            0           0           2           18
        $02     Green           0           2           0           28
        $03     Cyan            0           2           2           30
        $04     Red             2           0           0           88
        $05     Magenta         2           0           2           90
        $06     Brown           2           1           0           94
        $07     White           2           2           2           247 instead of 102
        $08     Gray            1           1           1           240 instead of 59
        $09     Light Blue      1           1           5           63
        $0A     Light Green     1           5           1           83
        $0B     Light Cyan      1           5           5           87
        $0C     Light Red       5           1           1           203
        $0D     Light Magenta   5           1           5           207
        $0E     Yellow          5           5           1           227
        $0F     Bright White    5           5           5           231


    */

    reg_bs3gfx.colorPalette[0x00] = 16;
    reg_bs3gfx.colorPalette[0x01] = 18;
    reg_bs3gfx.colorPalette[0x02] = 28;
    reg_bs3gfx.colorPalette[0x03] = 30;
    reg_bs3gfx.colorPalette[0x04] = 88;
    reg_bs3gfx.colorPalette[0x05] = 90;
    reg_bs3gfx.colorPalette[0x06] = 94;
    reg_bs3gfx.colorPalette[0x07] = 247; /* instead of 102 */
    reg_bs3gfx.colorPalette[0x08] = 240; /* instead of 59  */
    reg_bs3gfx.colorPalette[0x09] = 63;
    reg_bs3gfx.colorPalette[0x0A] = 83;
    reg_bs3gfx.colorPalette[0x0B] = 87;
    reg_bs3gfx.colorPalette[0x0C] = 203;
    reg_bs3gfx.colorPalette[0x0D] = 207;
    reg_bs3gfx.colorPalette[0x0E] = 227;
    reg_bs3gfx.colorPalette[0x0F] = 231;

}

void _bs3_gfx_clearscreen()
{
    _outFlush();
    _outStr(".......");
    ansi_SGR_reset();
    ansi_ED(2);
    ansi_ED(3);
    ansi_CUP(1,1);
    _outFlush();
}

void _bs3_gfx_clearscreen_enter()
{
    _bs3_gfx_clearscreen();
    ansi_HCU();
    _bs3_gfx_invalidateshadow();
}

void _bs3_gfx_clearscreen_leave()
{
    _bs3_gfx_clearscreen();
    ansi_SCU();
}


void _bs3_gfx_screenrender()
{
    int y,x;
    int w,h;

    BYTE up_pixelshadow;
    BYTE down_pixelshadow;

    BYTE up_pixelSurface;
    BYTE down_pixelSurface;

    WORD addrShadow;
    WORD addrVideo;
    WORD addr;
    BYTE surface;
    BYTE lastColumn;
    BYTE lastRow;
    int lastForeColor;
    int lastBackColor;

    w = (int)(reg_bs3gfx.viewport_size & 0x00FE);
    h = (int)((reg_bs3gfx.viewport_size & 0xFE00) >> 8 ) / 2 ;
    surface = reg_bs3gfx.viewport_surface;
    addrShadow = 0;
    addrVideo = reg_bs3gfx.viewport_location-1; /* minus because for(x...) start with 1 and not 0 */
    lastColumn = 255;
    lastRow = 255;
    lastForeColor = -1;
    lastBackColor = -1;
    for (y = 1; y <= h; y++)  /* browse view port on screen */
    {
        for (x = 1; x <= w ; x++)
        {
            up_pixelshadow     = reg_bs3gfx.viewport_shadow[addrShadow     ];
            down_pixelshadow   = reg_bs3gfx.viewport_shadow[addrShadow + w ];

            up_pixelSurface    = reg_bs3gfx.videoram[surface][(addrVideo + x) & 0x0FFFF  ];
            down_pixelSurface  = reg_bs3gfx.videoram[surface][(addrVideo + x + 256) & 0x0FFFF ];

            if (up_pixelshadow != up_pixelSurface || down_pixelshadow != down_pixelSurface )
            {
                /* location of character */
                if (lastColumn != x && lastRow != y)
                {
                    ansi_CUP(y,x);
                    lastColumn = (BYTE)x;
                    lastRow = (BYTE)y;
                } 
                else
                {
                    if (lastColumn != x)
                    {
                        ansi_CHA(x);
                        lastColumn = (BYTE)x;
                    }
                    else
                    {
                        if (lastRow != y)
                        {
                           ansi_CUD((y-lastRow));
                           lastRow = (BYTE)y;
                        }
                    }
                }
                /* fore color : upper pixel*/
                if (((int)up_pixelSurface) != lastForeColor )
                {
                    ansi_SGR_fg256((reg_bs3gfx.colorPalette[up_pixelSurface]));
                    lastForeColor = (int)up_pixelSurface;
                }
                /* back color : lower pixel */
                if (((int)down_pixelSurface) != lastBackColor )
                {
                    ansi_SGR_bg256((reg_bs3gfx.colorPalette[down_pixelSurface]));
                    lastBackColor = (int)down_pixelSurface;
                }
                _out(0xE2);_out(0x96);_out(0x80); /* upper half block character in UTF8 */
                lastColumn++; /* writting a character make location move of one column to the right */
                /* update shadow */
                reg_bs3gfx.viewport_shadow[addrShadow    ]  = up_pixelSurface;
                reg_bs3gfx.viewport_shadow[addrShadow + w]  = down_pixelSurface;
            }
            addrShadow++;
        } /* for each double vertical pixel in a row */
        addrVideo  += 512;
        addrVideo  =  addrVideo & 0x0FFFF; /* stay on same surface */
        addrShadow += w;
    } /* for each double pixel row */
    _outFlush();
}


/* BS3 GFX command function implementation */
void bs3_gfx_command_reset()
{
    /* clear video ram surfaces */
    int i,j;
    for (i = 0 ; i < 2 ; i++)
        for (j = 0; j < 65536; j++)
            reg_bs3gfx.videoram[i][j]=0;
    /* default view port */
    reg_bs3gfx.viewport_location = 0x0000; /*( y= 0 , x = 0)  */
    reg_bs3gfx.viewport_size = 100 * 256 + 160; /* Height = 100, Width = 160 */
    reg_bs3gfx.viewport_surface = 0; /* first video ram */
    /* reset byte registers */
    for (i = 0 ; i < 8 ; i++)
    {
        reg_bs3gfx.pb[i]=0;
        reg_bs3gfx.pw[i]=0;
    }
    /* set color palette */
    _bs3_gfx_setpalette();
    /* reset status */
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
    reg_bs3gfx.ENABLE = 1;
    reg_bs3gfx.WAITFORDATA = 0;
    /* reset screen */
    _bs3_gfx_clearscreen_enter();
    reg_bs3gfx.pb1 = 0;
    bs3_gfx_command_viewport_clear();
    bs3_gfx_command_refresh();
}

void bs3_gfx_command_end()
{
    _bs3_gfx_clearscreen_leave();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
    reg_bs3gfx.ENABLE = 0;
    reg_bs3gfx.WAITFORDATA = 0;
}

void bs3_gfx_command_refresh()
{
    _bs3_gfx_screenrender();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_viewport_config()
{
    /* check if parameters are ok */
    BYTE w,h;
    w = (BYTE)(reg_bs3gfx.pw3 & 0x00FE);
    h = (BYTE)((reg_bs3gfx.pw3 & 0xFE00) >> 8);
    if (w < 2 || w > 160 || h < 2 || h > 100) /* good new size ?*/
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADVPSIZE;
        return;
    }    
    if (reg_bs3gfx.pb1 > 1) /* good new surface ? */
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    /* apply change */
    reg_bs3gfx.viewport_surface  = reg_bs3gfx.pb1;
    reg_bs3gfx.viewport_location = reg_bs3gfx.pw2;
    if (reg_bs3gfx.viewport_size != reg_bs3gfx.pw3)
    {
        reg_bs3gfx.viewport_size = reg_bs3gfx.pw3;
        _bs3_gfx_clearscreen_enter();
        bs3_gfx_command_refresh();
    }
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_viewport_getconfig()
{
    /* get config apply change */
    reg_bs3gfx.pb1 = reg_bs3gfx.viewport_surface;
    reg_bs3gfx.pw2 = reg_bs3gfx.viewport_location;
    reg_bs3gfx.pw3 = reg_bs3gfx.viewport_size;
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_viewport_clear()
{
    int x,y;
    int w,h;
    BYTE surface;
    WORD addrSurface;
    BYTE value;

    value = reg_bs3gfx.pb1;
    addrSurface = reg_bs3gfx.viewport_location;
    surface     = reg_bs3gfx.viewport_surface;
    w = (int)(reg_bs3gfx.viewport_size & 0x00FE);
    h = (int)((reg_bs3gfx.viewport_size & 0xFE00) >> 8);

    for (y = 0; y < h ; y++)
    {
        for (x = 0; x < w ; x++)
        {
            reg_bs3gfx.videoram[surface][(addrSurface + x) & 0x0FFFF] = value;
        }
        addrSurface += 256;
        addrSurface = addrSurface & 0x0FFFF; /* stay in surface */
    }
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK; 
}

void bs3_gfx_command_surface_getpixel()
{
    BYTE surface;
    WORD addrSurface;

    addrSurface = reg_bs3gfx.pw2;
    surface = reg_bs3gfx.pb1;
    if (surface & 0xFE) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    reg_bs3gfx.pb3 = reg_bs3gfx.videoram[surface][addrSurface];
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_setpixel()
{
    BYTE surface;
    WORD addrSurface;

    addrSurface = reg_bs3gfx.pw2;
    surface = reg_bs3gfx.pb1;
    if (surface & 0xFE) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    reg_bs3gfx.videoram[surface][addrSurface] = reg_bs3gfx.pb3 ;
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_draw_hline()
{
    BYTE surface;
    WORD addrSurface;
    BYTE i;
    BYTE value;
    addrSurface = reg_bs3gfx.pw2;
    surface = reg_bs3gfx.pb1;
    value = reg_bs3gfx.pb3;
    i = reg_bs3gfx.pb4;
    if (surface & 0xFE) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    do
    {
        reg_bs3gfx.videoram[surface][addrSurface] = value ;
        addrSurface++;
        i--;
    } while (i);
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_draw_vline()
{
    BYTE surface;
    WORD addrSurface;
    BYTE i;
    BYTE value;

    addrSurface = reg_bs3gfx.pw2;
    surface = reg_bs3gfx.pb1;
    value = reg_bs3gfx.pb3;
    i = reg_bs3gfx.pb4;
    if (surface & 0xFE) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    do
    {
        reg_bs3gfx.videoram[surface][addrSurface] = value ;
        addrSurface+=256;
        i--;
    } while (i);
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_draw_box()
{
    BYTE surface;
    WORD addrSurface;
    BYTE i;
    BYTE value;
    WORD offset;

    surface = reg_bs3gfx.pb1;
    value = reg_bs3gfx.pb3;
    if (surface & 0xFE) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    /* top and bottom horizontal lines */
    addrSurface = reg_bs3gfx.pw2;
    i = (BYTE)(reg_bs3gfx.pw4 & 0x00FF);
    offset = (reg_bs3gfx.pw4 & 0xFF00) - 0x0100;
    do
    {
        reg_bs3gfx.videoram[surface][addrSurface         ] = value ;
        reg_bs3gfx.videoram[surface][addrSurface + offset] = value ;
        addrSurface++;
        i--;
    } while (i);
    /* left and right vertical lines */
    addrSurface = reg_bs3gfx.pw2;
    i = (BYTE)((reg_bs3gfx.pw4 & 0xFF00) >> 8);
    offset = (reg_bs3gfx.pw4 & 0x00FF) - 1;
    do
    {
        reg_bs3gfx.videoram[surface][addrSurface         ] = value ;
        reg_bs3gfx.videoram[surface][addrSurface + offset] = value ;
        addrSurface += 256;
        i--;
    } while (i);
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_draw_boxfull()
{
    BYTE surface;
    WORD addrSurface;
    WORD offset;
    BYTE i;
    WORD j;
    BYTE value;

    surface = reg_bs3gfx.pb1;
    value = reg_bs3gfx.pb3;
    if (surface & 0xFE) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }

    offset = 0;
    j = reg_bs3gfx.pw4 & 0xFF00;
    do
    {
        addrSurface = reg_bs3gfx.pw2 + offset;
        i = (BYTE)(reg_bs3gfx.pw4 & 0x00FF);
        do
        {
            reg_bs3gfx.videoram[surface][addrSurface] = value ;
            addrSurface++;
            i--;
        } while (i);
        offset += 0x0100;
        j      -= 0x0100;
    } while (j);
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_operator()
{
    BYTE sourceSurface;
    BYTE targetSurface;
    WORD sourceAddr;
    WORD targetAddr;
    WORD size;
    BYTE operator;
    WORD offset;
    int x,y;
    int h,w;
    
    sourceSurface   = reg_bs3gfx.pb1;
    targetSurface   = reg_bs3gfx.pb5;
    sourceAddr      = reg_bs3gfx.pw2;
    targetAddr      = reg_bs3gfx.pw6;
    size            = reg_bs3gfx.pw4;
    operator        = reg_bs3gfx.pb3;

    h               = (int)((size & 0xFF00) >> 8);
    w               = (int)( size & 0x00FF);
    h               = h?h:256;
    w               = w?w:256;
    offset          = 256 - w;
    if ((sourceSurface & 0xFE) || (targetSurface & 0xFE)) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }

    if (operator > 6 )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADOPERATOR;
        return;        
    }

    for (y = 0; y < h ; y++)
    {
        for (x = 0; x < w ; x++)
        {
            switch (operator)
            {
                case BS3_GFX_COMMAND_BLIT_OPERATOR_COPY:
                    reg_bs3gfx.videoram[targetSurface][targetAddr] =  reg_bs3gfx.videoram[sourceSurface][sourceAddr];
                    break;
                case BS3_GFX_COMMAND_BLIT_OPERATOR_OR:
                    reg_bs3gfx.videoram[targetSurface][targetAddr] |= reg_bs3gfx.videoram[sourceSurface][sourceAddr];
                    break;
                case BS3_GFX_COMMAND_BLIT_OPERATOR_EOR:
                    reg_bs3gfx.videoram[targetSurface][targetAddr] ^= reg_bs3gfx.videoram[sourceSurface][sourceAddr];
                    break;
                case BS3_GFX_COMMAND_BLIT_OPERATOR_AND:
                    reg_bs3gfx.videoram[targetSurface][targetAddr] &= reg_bs3gfx.videoram[sourceSurface][sourceAddr];
                    break;
                case BS3_GFX_COMMAND_BLIT_OPERATOR_ADD:
                    reg_bs3gfx.videoram[targetSurface][targetAddr] += reg_bs3gfx.videoram[sourceSurface][sourceAddr];
                    break;
                case BS3_GFX_COMMAND_BLIT_OPERATOR_SUB:
                    reg_bs3gfx.videoram[targetSurface][targetAddr] -= reg_bs3gfx.videoram[sourceSurface][sourceAddr];
                    break;
                case BS3_GFX_COMMAND_BLIT_OPERATOR_MUL:
                    reg_bs3gfx.videoram[targetSurface][targetAddr] *= reg_bs3gfx.videoram[sourceSurface][sourceAddr];
                    break;
            }
            targetAddr++;
            sourceAddr++;
        }
        targetAddr += offset;
        sourceAddr += offset;
    }
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_keycolor()
{
    BYTE sourceSurface;
    BYTE targetSurface;
    WORD sourceAddr;
    WORD targetAddr;
    WORD size;
    BYTE keycolor;
    BYTE value;
    WORD offset;
    int x,y;
    int h,w;
    
    sourceSurface   = reg_bs3gfx.pb1;
    targetSurface   = reg_bs3gfx.pb5;
    sourceAddr      = reg_bs3gfx.pw2;
    targetAddr      = reg_bs3gfx.pw6;
    size            = reg_bs3gfx.pw4;
    keycolor        = reg_bs3gfx.pb3;

    h               = (int)((size & 0xFF00) >> 8);
    w               = (int)( size & 0x00FF);
    h               = h?h:256;
    w               = w?w:256;
    offset          = 256 - w;
    if ((sourceSurface & 0xFE) || (targetSurface & 0xFE)) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }

    for (y = 0; y < h ; y++)
    {
        for (x = 0; x < w ; x++)
        {
            value = reg_bs3gfx.videoram[sourceSurface][sourceAddr];
            if (value != keycolor)
                reg_bs3gfx.videoram[targetSurface][targetAddr] = value;
            targetAddr++;
            sourceAddr++;
        }
        targetAddr += offset;
        sourceAddr += offset;
    }
    if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_transfer()
{
    if (reg_bs3gfx.pb1 & 0xFE)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    reg_bs3gfx.WAITFORDATA = 1;
    reg_bs3gfx.pw6 = reg_bs3gfx.pw2; /* current surface coordinate */
    reg_bs3gfx.pw5 = 0; /* current progress X */
    reg_bs3gfx.pw1 = 0; /* current progress Y */
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_transfer_data()
{
    BYTE targetSurface;
    WORD currAddr;
    BYTE value;
    WORD size;
    WORD currX;
    WORD currY;
    WORD w,h;
    WORD offset;

    targetSurface   = reg_bs3gfx.pb1;
    size            = reg_bs3gfx.pw4;
    currAddr        = reg_bs3gfx.pw6;
    value           = reg_bs3gfx.pb3;
    currX           = reg_bs3gfx.pw5;
    currY           = reg_bs3gfx.pw1;
    w               =  size & 0x00FF;
    h               = (size & 0xFF00) >> 8;
    w               = w?w:0x0100;
    h               = h?h:0x0100;
    offset          = 0x0100 - w;

    reg_bs3gfx.videoram[targetSurface][currAddr] = value;
    currX++;
    currAddr++;
    if (currX >= w)
    {
        currAddr += offset;
        currX = 0;
        currY++;
        if (currY >= h)
        {
            reg_bs3gfx.WAITFORDATA = 0;
            if (reg_bs3gfx.pb7) bs3_gfx_command_refresh();
            reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
            return;
        }
    }
    reg_bs3gfx.pw5 = currX;
    reg_bs3gfx.pw1 = currY;
    reg_bs3gfx.pw6 = currAddr;

    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
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
        if (endGfx) break;
        if (reg_bs3gfx.WAITFORDATA && reg_bs3gfx.cmd != BS3_GFX_COMMAND_RESET && reg_bs3gfx.cmd != BS3_GFX_COMMAND_END) /* data transfer */
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
            if (reg_bs3gfx.ENABLE || (!reg_bs3gfx.ENABLE && reg_bs3gfx.cmd == BS3_GFX_COMMAND_RESET)) /* id BS3 GFX device is enable */
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
                    case BS3_GFX_COMMAND_VIEWPORT_GETCONFIG:
                        bs3_gfx_command_viewport_getconfig();
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
            else /* device is not enabled (no RESET command has been submitted or END command has been executed)*/
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
    if (created_thread_bs3gfx)
        resultGfx = pthread_kill(thread_bs3gfx, 0);
    else
    {
        endGfx = 1;
        return 1;
    }            

    if (resultGfx == 0 && created_thread_bs3gfx) 
    {
        endGfx = 1;
        pthread_cond_signal(&condWakeUp);
        pthread_join(thread_bs3gfx, NULL);
    }
    created_thread_bs3gfx = 0;
    return resultGfx;
}

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
    .mask=0xFFF8, /* 8 BS3 GFX I/O registers 8/16 bits */
    .startdevice = &dev_bs3gfx_start,
    .stopdevice = &dev_bs3gfx_stop,
    .readByte = &dev_bs3gfx_readByte,
    .writeByte = &dev_bs3gfx_writeByte,
    .readWord = &dev_bs3gfx_readWord,
    .writeWord = &dev_bs3gfx_writeWord,
    .interruptNumber = 0 /* no interrupt vector */
};
