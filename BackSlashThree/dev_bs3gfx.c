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
    BYTE colorPalette[256];
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
static pthread_cond_t condWakeUp = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t lockGfx = PTHREAD_MUTEX_INITIALIZER; 

/* ANSI commands */
#define _out(c) putc((int)c,stdout)
#define _outStr(str) fprintf(stdout,str)
#define _outFlush() fflush(stdout)
#define ansi_esc(e)         _out(27);_out(e)

/*    ## Single Shift Two */
#define _bs3_ansi_SS2()     ansi_esc('N')

/*    ##  Single Shift Three */
#define ansi_SS3()          ansi_esc('O')

/*    ## Device Control String */
#define ansi_DCS()          ansi_esc('P')

/*    ## Control Sequence Introducer */
#define ansi_CSI()          ansi_esc('[')

/*    ##  String Terminator */
#define ansi_ST()           ansi_esc('\\')

/*    ## Operating System Command */
#define ansi_OSC()          ansi_esc(']')

/*    ## Start of String */
#define ansi_SOS()          ansi_esc('X')

/*    ## Privacy Message */
#define ansi_PM()           ansi_esc('^')

/*    ##  Application Program Command */
#define ansi_APC()          ansi_esc('_')

/*    ## Reset to Initial State */
#define ansi_RIS()          ansi_esc('c')

/*####################
  # ANSI CSI sequences
  #################### */

#define _outByteStr(x) fprintf(stdout,"%d",(int)x)

/*    ## Cursor Up */
#define ansi_CUU(x)         ansi_CSI();_outByteStr(x);_out('A')

/*    ## Cursor Down */
#define ansi_CUD(x)         ansi_CSI();if ((x) > 1) _outByteStr(x);_out('B')

/*    ## Cursor Forward */
#define ansi_CUF(x)         ansi_CSI();_outByteStr(x);_out('C')

/*    ## Cursor Back */
#define ansi_CUB(x)         ansi_CSI();_outByteStr(x);_out('D')

/*    ## Cursor Next Line */
#define ansi_CNL(x)         ansi_CSI();_outByteStr(x);_out('E')

/*    ## Cursor Previous Line */
#define ansi_CPL(x)         ansi_CSI();_outByteStr(x);_out('F')

/*    ## Cursor Horizontal Absolute */
#define ansi_CHA(x)         ansi_CSI();_outByteStr(x);_out('G')

/*    ## Cursor Position {1}row, {2}column */
#define ansi_CUP(y,x)       ansi_CSI();_outByteStr(y);_out(';');_outByteStr(x);_out('H')

/*   ## Erase Display
     ## 0 : from cursor up to end of screen
     ## 1 : from cursor up to begin of screen
     ## 2 : clear all screen : keep back buffer
     ## 3 : clear all screen , erase back buffer */
#define ansi_ED(x)          ansi_CSI();_outByteStr(x);_out('J')

/*   ## Erase in Line, to cursor position change
     ## 0 : clear from cursor to end of line
     ## 1 : clear from cursor to begin of line
     ## 2 : clear all line */
#define ansi_EL(x)          ansi_CSI();_outByteStr(x);_out('K')

/*   ##  Scroll Up */
#define ansi_SU(x)          ansi_CSI();_outByteStr(x);_out('S')

/*   ## Scroll Down */
#define ansi_SD(x)          ansi_CSI();_outByteStr(x);_out('T')

/*   ## same as ansi_CUP : Cursor Position {1} row {2} column */
#define ansi_HVP(y,x)       ansi_CSI();_outByteStr(y);_out(';');_outByteStr(x);_out('f')

/*   ## Select Graphic Endition {1} */
#define ansi_SGR(x)         ansi_CSI();_outByteStr(x);_out('m')

/*   ## Select Graphic Rendition with 2 parameters {1}n, {2}p1, {3}p2 */
#define ansi_SGR3(n,p1,p2)  ansi_CSI();_outByteStr(n);_out(';');_outByteStr(p1);_out(';');_outByteStr(p2);_out('m')

/*    ## Select Graphic Rendition with 4 parameters  */
/*        {1} n, {2}p1, {3}p2, {4}p3, {5},p4 */
#define ansi_SGR5(n,p1,p2,p3,p4) ansi_CSI();_outByteStr(n);_out(';');_outByteStr(p1);_out(';');_outByteStr(p2);_out(';');_outByteStr(p3);_out(';');_outByteStr(p4);_out('m')

/*    ## AUX Port On */
#define ansi_AUXportOn()      ansi_CSI()_out('5');_out('i')

/*    ## AUX Port Off */
#define ansi_AUXportOff()     ansi_CSI();_out('4');_out('i')

/*    ## Device Status Report */
#define ansi_DSR()            ansi_CSI();_out('6');_out('n')

/*    ## Save Cursor Position */
#define ansi_SCP()            ansi_CSI();_out('s')
    
/*    ## Restore Cursor Position */
#define ansi_RCP()            ansi_CSI();_out('u')

/*    ## show cursor */
#define ansi_SCU()            ansi_CSI();_outStr("?25h")

/*    ## hide cursor */
#define ansi_HCU()            ansi_CSI();_outStr("?25l")

/*    ## enable alternate screen buffer */
#define ansi_EASB()           ansi_CSI();_outStr("?1049h")

/*    ## disable alternate screen buffer */
#define ansi_DASB()           ansi_CSI();_outStr("?1049l")

/*   ## Turn on bracketed paste mode  */
#define ansi_BPMon()          ansi_CSI();_outStr("?2004h")

/*    ## Turn off bracketed paste mode  */
#define ansi_BPMoff()         ansi_CSI();_outStr("?2004l")

/*#####################################
  # ANSI SGR (Select Graphic Rendition)
  ##################################### */

/*    ## Reset / Normal */
#define ansi_SGR_reset()     ansi_SGR(0)

/*    ## Bold or increased intensity */
#define ansi_SGR_bold()      ansi_SGR(1)

/*    ## Faint (decreased intensity) */
#define ansi_SGR_faint()     ansi_SGR(2)

/*    ## Italic */
#define ansi_SGR_italic()    ansi_SGR(3)

/*    ## Underline */
#define ansi_SGR_underline() ansi_SGR(4)

/*    ## Slow Blink */
#define ansi_SGR_sblink()    ansi_SGR(5)
    
/*    ## Rapid Blink */
#define ansi_SGR_rblink()    ansi_SGR(6)    

/*    ## Reverse Video */
#define ansi_SGR_reverse()   ansi_SGR(7)

/*    ## Conceal */
#define ansi_SGR_conceal()   ansi_SGR(8)    

/*    ## Crossed-out */
#define ansi_SGR_crossout()  ansi_SGR(9)    

/*    ## primary font */
#define ansi_SGR_defaultfont ansi_SGR(10)

/*    ## alternate font (1 to 9) , 0 same as primary */
#define ansi_SGR_font(x)     ansi_SGR((x+10))

/*    ## Fraktur */
#define ansi_SGR_fraktur()   ansi_SGR(20)

/*    ## Bold off or Double Underline */
#define ansi_SGR_doubleunderline()  ansi_SGR(21)

/*    ## Normal color or intensity (neither bold nor faint) */
#define ansi_SGR_normalcolorintensity() ansi_SGR(22)

/*    ## Not italic nor fraktur */
#define ansi_SGR_italicfrakturoff()  ansi_SGR(23)

/*    ## Underline off */
#define ansi_SGR_underlineoff()      ansi_SGR(24)

/*    ## blink off */
#define ansi_SGR_blinkoff()          ansi_SGR(25)

/*    ## Inverse off */
#define ansi_SGR_inverseoff()        ansi_SGR(27)

/*    ## Reveal / conceal off */
#define ansi_SGR_concealoff()        ansi_SGR(28)

/*    ## Not cross-out */
#define ansi_SGR_crossoutoff()       ansi_SGR(29)

/*    ## Set foreground color 0 to 7  n {1} */
#define ansi_SGR_fgcolor(x)          ansi_SGR((x+30))

/*    ## set foreground color , palette on 256 colors n {1} */
#define ansi_SGR_fg256(x)            ansi_SGR3(38,5,x)

/*    ## set foreground color, RGB 24 bits (3* 256 colors, RGB) */
/*        r {1}, g {2}, b {3} */
#define ansi_SGR_fgRGB(r,g,b)        ansi_SGR5(38,2,r,g,b)    

/*    ## set default foreground color */
#define ansi_SGR_fgdefault()         ansi_SGR(39)

/*    ## Set background color 0 to 7 n{1} */
#define ansi_SGR_bgcolor(x)          ansi_SGR((x+40))

/*    ## set background color , palette on 256 colors n{1} */
#define ansi_SGR_bg256(x)            ansi_SGR3(48,5,x)

/*    ## set background color, RGB 24 bits (3* 256 colors, RGB)
        r{1}, g{2}, b{3} */
#define ansi_SGR_bgRGB(r,g,b)        ansi_SGR5(48,2,r,g,b)    

/*    ## set default background color */
#define ansi_SGR_bgdefault()         ansi_SGR(49)

/*    ## Framed */
#define ansi_SGR_framed()            ansi_SGR(51)    

/*    ## Encircled   */
#define ansi_SGR_encircled()         ansi_SGR(52)

/*    ## Overlined */
#define ansi_SGR_overlined()         ansi_SGR(53)

/*    ## Not framed or encirlced */
#define ansi_SGR_framedoff()         ansi_SGR(54)

#define ansi_SGR_encircledoff()      ansi_SGR(54)

/*    ## Not overlined */
#define ansi_SGR_overlinedoff()      ansi_SGR(55)

/*    ## Foreground Black */
#define ansi_SGR_fg_black()          ansi_SGR_fgcolor(0)

/*    ## Foreground Red */
#define ansi_SGR_fg_red()            ansi_SGR_fgcolor(1)

/*   ## Foreground Green */
#define ansi_SGR_fg_green()          ansi_SGR_fgcolor(2)

/*    ## Foreground Yellow */
#define ansi_SGR_fg_yellow()         ansi_SGR_fgcolor(3)

/*   ## Foreground Blue */
#define ansi_SGR_fg_blue()           ansi_SGR_fgcolor(4)

/*   ## Foreground Magenta */
#define ansi_SGR_fg_magenta()        ansi_SGR_fgcolor(5)

/*    ## Foreground Cyan */
#define ansi_SGR_fg_cyan             ansi_SGR_fgcolor(6)

/*    ## Foreground White */
#define ansi_SGR_fg_white()          ansi_SGR_fgcolor(7)
    
/*    ## Background Black */
#define ansi_SGR_bg_black()          ansi_SGR_bgcolor(0)

/*    ## Background Red */
#define ansi_SGR_bg_red()            ansi_SGR_bgcolor(1)

/*    ## Background Green */
#define ansi_SGR_bg_green()          ansi_SGR_bgcolor(2)

/*    ## Background Yellow */
#define ansi_SGR_bg_yellow()         ansi_SGR_bgcolor(3)

/*    ## Background Blue */
#define ansi_SGR_bg_blue()           ansi_SGR_bgcolor(4)

/*    ## Background Magenta */
#define ansi_SGR_bg_magenta()        ansi_SGR_bgcolor(5)

/*    ## Background Cyan */          
#define ansi_SGR_bg_cyan()           ansi_SGR_bgcolor(6)

/*   ## Background White */
#define ansi_SGR_bg_white()          ansi_SGR_bgcolor(7)


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
                reg_bs3gfx.pb[addr] = data;
            } /* else ignore write */
        }
    }
    pthread_mutex_unlock(&lockGfx);
}

/* Utils functions for BS3 GFX command execution */
void _bs3_gfx_clearscreen()
{
    _outFlush();
    _outStr(".......");
    ansi_ED(2);
    ansi_ED(3);
    ansi_SGR_reset();
    ansi_SCU();
    ansi_CUP(1,1);
    _outFlush();
}

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
    /* reset color palette */
    for (i = 0 ; i < 256; i++)
        reg_bs3gfx.colorPalette[i] = (BYTE)i;
    /* IBM VGA 16 color palette
        index   Name (IBM)      Red (0..5)  Green(0..5) Blue(0..5)  (equivalent to ANSI 256 colors Index)
        $00     Black	        0	        0           0           16
        $01     Blue	        0	        0           3           19
        $02     Green	        0	        3           0           34
        $03     Cyan	        0	        3           3           37
        $04     Red             3	        0           0           124
        $05     Magenta	        3	        0           3           127
        $06     Brown* 	        3	        2           0           136
        $07     White	        3	        3           3           145
        $08     Gray	        2	        2           2           102
        $09     Light Blue      2	        2           5           105
        $0A     Light Green     2	        5           2           120
        $0B     Light Cyan      2	        5           5           123
        $0C     Light Red       5	        2           2           210
        $0D     Light Magenta	5	        2           5           213
        $0E     Yellow	        5	        5           2           228
        $0F     Bright White	5	        5           5           231
    */
    reg_bs3gfx.colorPalette[0x00] = 16;
    reg_bs3gfx.colorPalette[0x01] = 19;
    reg_bs3gfx.colorPalette[0x02] = 34;
    reg_bs3gfx.colorPalette[0x03] = 37;
    reg_bs3gfx.colorPalette[0x04] = 124;
    reg_bs3gfx.colorPalette[0x05] = 127;
    reg_bs3gfx.colorPalette[0x06] = 136;
    reg_bs3gfx.colorPalette[0x07] = 145;
    reg_bs3gfx.colorPalette[0x08] = 102;
    reg_bs3gfx.colorPalette[0x09] = 105;
    reg_bs3gfx.colorPalette[0x0A] = 120;
    reg_bs3gfx.colorPalette[0x0B] = 123;
    reg_bs3gfx.colorPalette[0x0C] = 210;
    reg_bs3gfx.colorPalette[0x0D] = 213;
    reg_bs3gfx.colorPalette[0x0E] = 228;
    reg_bs3gfx.colorPalette[0x0F] = 231;
    /* reset status */
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
    reg_bs3gfx.ENABLE = 1;
    reg_bs3gfx.WAITFORDATA = 0;
    /* reset screen + shadow */
    _bs3_gfx_clearscreen();
    ansi_HCU();
    _bs3_gfx_invalidateshadow();
    reg_bs3gfx.pb1 = 0;
    bs3_gfx_command_viewport_clear();
    bs3_gfx_command_refresh();
}

void bs3_gfx_command_end()
{
    _bs3_gfx_clearscreen();
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
    reg_bs3gfx.ENABLE = 0;
    reg_bs3gfx.WAITFORDATA = 0;
}

void bs3_gfx_command_refresh()
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
    for (y = 1; y <= h; y++) ; /* browse view port on screen */
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
                    lastColumn = x;
                    lastRow = y;
                } 
                else
                {
                    if (lastColumn != x)
                    {
                        ansi_CHA(x);
                        lastColumn = x;
                    }
                    else
                    {
                        if (lastRow != y)
                        {
                           ansi_CUD((y-lastRow));
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
        _bs3_gfx_clearscreen();
        ansi_HCU();
        _bs3_gfx_invalidateshadow();
        bs3_gfx_command_refresh();
    }
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
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK; 
}

void bs3_gfx_command_surface_getpixel()
{
    BYTE surface;
    surface = reg_bs3gfx.pb1;
    WORD addrSurface = reg_bs3gfx.pw2;
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
    surface = reg_bs3gfx.pb1;
    WORD addrSurface = reg_bs3gfx.pw2;
    if (surface & 0xFE) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    reg_bs3gfx.videoram[surface][addrSurface] = reg_bs3gfx.pb3 ;
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_draw_hline()
{
    BYTE surface;
    surface = reg_bs3gfx.pb1;
    WORD addrSurface = reg_bs3gfx.pw2;
    BYTE i;
    BYTE value;
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
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_draw_vline()
{
    BYTE surface;
    surface = reg_bs3gfx.pb1;
    WORD addrSurface = reg_bs3gfx.pw2;
    BYTE i;
    BYTE value;
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
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_draw_box()
{
    BYTE surface;
    surface = reg_bs3gfx.pb1;
    WORD addrSurface;
    BYTE i;
    BYTE value;
    WORD offset;
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
    i = (BYTE)((reg_bs3gfx.pw4 & 0xFF) >> 8);
    offset = (reg_bs3gfx.pw4 & 0x00FF) - 1;
    do
    {
        reg_bs3gfx.videoram[surface][addrSurface         ] = value ;
        reg_bs3gfx.videoram[surface][addrSurface + offset] = value ;
        addrSurface += 256;
        i--;
    } while (i);

    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;

}

void bs3_gfx_command_surface_draw_boxfull()
{
    BYTE surface;
    surface = reg_bs3gfx.pb1;
    WORD addrSurface;
    WORD offset;
    BYTE i;
    WORD j;
    BYTE value;
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
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_operator()
{
    /* TODO */
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_keycolor()
{
    /* TODO */
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_transfer()
{
    /* TODO */
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_surface_blit_transfer_data()
{
    /* TODO */
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
        if (reg_bs3gfx.WAITFORDATA && reg_bs3gfx.cmd != BS3_GFX_COMMAND_RESET) /* data transfer */
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
            if (reg_bs3gfx.ENABLE) /* id BS3 GFX device is enable */
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
