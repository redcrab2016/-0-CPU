#include <pthread.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <errno.h>
#include <time.h>
#include <string.h>
#include <signal.h>

#include "bs3_bus.h"
#include "bs3gfxlogo.inc"

struct dev_bs3gfx_tile
{
    WORD metadata;                          /* custom data usable by BS3 CPU program */
    union 
    {
        WORD coordinates;                   /* tile coordinates(may be) for tile and sprite(if tileCoordinate == 1 ), or sprite absolute coordinates*/
        struct 
        {
            BYTE x;                         /* from 0 to 31 if tile coordinate , from 0 to 255 if absolute coordinate */
            BYTE y;                         /* from 0 to 31 if tile coordinate , from 0 to 255 if absolute coordinate */
        };
        
    };
    BYTE surface;                           /* Surface where the image tiles (including mask) is stored */
    BYTE keyColor;                          /* Key color to use when useKeyColor == 1 */
    BYTE mainTileIndex;                     /* Tile index for OR/XOR or keycolor */
    BYTE mainTileBank;                      /* Tile bank of mainTileIndex */
    BYTE auxTileIndex;                      /* Tile index for AND operation, ignored if useKeycolor == 1 or ( usekeycolor == 0 && useSpecialMask == 1 ) */
    BYTE auxTileBank;                       /* Tile bank of auxTileBank */
    union 
    {
        BYTE config;
        struct 
        {
            BYTE useKeyColor:1;             /* 0: OR & AND mask, 1: Key color */
            BYTE useSpecialMask:1;          /* valid if useKeycolor == 0, 0: use auxTile for AND mask , 1: use special mask useMaskFF */
            BYTE useMaskFF:1;               /* valid = useKeyColor == 0 && useSPecialMask ==1, 0: mask is full 00, 1: mask is full $FF */
            BYTE reserved1:5;
        };
        struct /* Dedicated for Tile map cell */
        {
            BYTE reserved2:3;
            BYTE ORasXOR:1;                 /* valid if useKeyColor == 0 */
            BYTE tileIsKeycolorFullbox:1;   /* valid if useKeyColor == 1 */
            BYTE reserved3:2;
            BYTE visible:1;                 /* 0: tile is not rendered, 1: tile is rendered */
        };
        struct /* Dedicated for Sprite */
        {
            BYTE reserved4:3;               
            BYTE cancollide:1;              /* 0: no collision detection, 1: collision detection (between two sprites that have cancollide == 1 )*/
            BYTE tileCoordinate:1;
            BYTE z:2;                       /* 00: sprite between layer 0 and 1, 01, sprite between layer 1 and 2, 10, sprite above layer 2, 11 sprite not rendered*/
            BYTE enabled:1;                 /* 0: disabled sprite (not rendered, no collision ... ), 1: enabled sprite*/
        };
    };
};

struct dev_bs3gfx_tilemap
{
    struct dev_bs3gfx_tile map[32][32]; /* map[Y][X] */
    BYTE visible;                       /* 0: invisible , 1 visible */
};


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
    BYTE videoram [3][65536];               /* 2 surfaces of 64KiB (256 width x 256 height  x 8bpp) + 1 surface layers composition result  */
    WORD viewport_location;                 /* MSB Y location (0 to 255), LSB X location (0 to 255) */
    WORD viewport_size;                     /* MSB Height (1 to 100) LSB Width (1 to 160)           */
    BYTE viewport_surface;
    BYTE viewport_shadow[16000];            /* shadow of what is on screen                          */
    BYTE colorPalette[256];
    BYTE tilebanksize[2][4];                /*[surface][banknumber] =  0: 8x8, 1: 16x16, 2: 32x32, 3: 64x64 */
    struct dev_bs3gfx_tilemap tilemap[2];   /* 2 tile maps */
    struct dev_bs3gfx_tile sprite[128];     /* 128 sprites */
    WORD spritecollision[256];
    BYTE spritecollisioncount;
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
#define BS3_GFX_COMMAND_TILE_BANK_CONFIG        0x0E
#define BS3_GFX_COMMAND_TILE_BANK_GETCONFIG     0x0F
#define BS3_GFX_COMMAND_TILE_MAP_RESET          0x10
#define BS3_GFX_COMMAND_TILE_MAP_CONFIG         0x11
#define BS3_GFX_COMMAND_TILE_MAP_CELL_CONFIG    0x12
#define BS3_GFX_COMMAND_TILE_MAP_CELL_GETCONFIG 0x13
#define BS3_GFX_COMMAND_SPRITE_RESET            0x14
#define BS3_GFX_COMMAND_SPRITE_CONFIG           0x15
#define BS3_GFX_COMMAND_SPRITE_GETCONFIG        0x16
#define BS3_GFX_COMMAND_SPRITE_COLLISION_COUNT  0x17
#define BS3_GFX_COMMAND_SPRITE_GETCOLLISION     0x18

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
#define BS3_GFX_STATUS_BADBANKID                0x06
#define BS3_GFX_STATUS_BADBANKSIZE              0x07
#define BS3_GFX_STATUS_BADTILEMAPID             0x08
#define BS3_GFX_STATUS_BADVISIBILITY            0x09
#define BS3_GFX_STATUS_BADCELLCOORDINATES       0x0A
#define BS3_GFX_STATUS_BADSPRITEID              0x0B
#define BS3_GFX_STATUS_BADTILEINDEX             0x0C
#define BS3_GFX_STATUS_BADCOLLIDEID             0x0D

#define BS3_TILE_BLIT_TILEMAP                   0
#define BS3_TILE_BLIT_SPRITE                    1
#define BS3_TILE_BLIT_KEYCOLOR                  0
#define BS3_TILE_BLIT_KEYCOLORTILE              1
#define BS3_TILE_BLIT_ANDOR                     2
#define BS3_TILE_BLIT_COPY                      3
#define BS3_TILE_BLIT_OR                        4
#define BS3_TILE_BLIT_XOR                       5


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

void bs3_gfx_command_tile_bank_config();
void bs3_gfx_command_tile_bank_getconfig();
void bs3_gfx_command_tile_map_reset();
void bs3_gfx_command_tile_map_config();
void bs3_gfx_command_tile_map_cell_config();
void bs3_gfx_command_tile_map_cell_getconfig();
void bs3_gfx_command_sprite_reset();
void bs3_gfx_command_sprite_config();
void bs3_gfx_command_sprite_getconfig();
void bs3_gfx_command_sprite_collision_count();
void bs3_gfx_command_sprite_getcollision();


extern struct bs3_device dev_bs3gfx;
int endGfx;
pthread_t thread_bs3gfx; 
int created_thread_bs3gfx = 0;
struct dev_bs3gfx reg_bs3gfx;
static pthread_cond_t condWakeUp = PTHREAD_COND_INITIALIZER; 
pthread_mutex_t lockGfx = PTHREAD_MUTEX_INITIALIZER; 
static int logoshowed;

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


void _bs3_gfx_screenrender(BYTE chosenSurface)
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
    if (chosenSurface < 3) surface = chosenSurface;
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
WORD banknbtilespersize[]= {255 /* 8x8 */, 63 /* 16x16 */ , 15 /* 32x32*/ , 3 /* 64x64 */ };

void bs3_gfx_command_reset()
{
    memset(&reg_bs3gfx,0,sizeof(struct dev_bs3gfx));
    /* clear video ram surfaces */
    int i,j,k,v;
    for (i = 0 ; i < 3 ; i++)
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
    if (logoshowed == 0)
    {
        
        logoshowed =1;
        i=0;
        j=0;
        while (bs3gfxlogo[i])
        {
            for ( k = bs3gfxlogo[i++], v = bs3gfxlogo[i++] ; k ; k--)
            {
                reg_bs3gfx.videoram[0][j++] = v;
                if ((j & 0x00FF) == 160) j= j + (256-160);
                j= j & 0x0FFFF;
            }
        }
         bs3_gfx_command_refresh();
         sleep(3);
    }
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

/* compute tile coordinates by its index in a bank of a surface */
/* return 0xFFFF is index is invalid */
WORD bs3_gfx_bank_tile_coordinates(BYTE surface, BYTE bank, BYTE tileindex)
{
    WORD tileoffset;
    BYTE tileSize;
    WORD tileCoordinates;

    tileSize = reg_bs3gfx.tilebanksize[surface & 0x01][bank & 0x03] & 0x03; /* 0:8x8 or 1:16x16 or 2:32x32 or 3:64x64 */

    switch (tileSize) /* compute index bank offset on surface */
    {
        case 0: /* 8x8   : 32 x 8 tiles : tile index from 0 to 255 */ 
            tileoffset = (((WORD)tileindex & 0x1F) << 3) + (((WORD)tileindex & 0xE0) << 6);
            break;
        case 1: /* 16x16 : 16 x 4 tiles : tile index from 0 to 63 */
            if (tileindex > 63 ) return 0xFFFF;
            tileoffset = (((WORD)tileindex & 0x0F) << 4) + (((WORD)tileindex & 0x30) << 8);
            break;
        case 2: /* 32x32 : 8 x 2 tiles : tile index from 0 to 15 */
            if (tileindex > 15) return 0xFFFF;
            tileoffset = (((WORD)tileindex & 0x07) << 5) + (((WORD)tileindex & 0x08) << 10);
            break;
        case 3: /* 64x64 : 4 x 1 tiles : tile index from 0 to 3 */
            if (tileindex > 3) return 0xFFFF;
            tileoffset = (((WORD)tileindex & 0x03) << 6) ;
            break;
        default:
            return 0xFFFF;
    }
    tileCoordinates  = (((WORD)(bank & 0x03)) << 14) + tileoffset;
    return tileCoordinates;

}

void bs3_gfx_tile_blit(struct dev_bs3gfx_tile * ptile, BYTE * targetSurface, int isSprite /* 1 for sprite , 0 for tilemap */)
{
    WORD targetCoordinates;
    BYTE tileSize;
    BYTE * srcSurface;
    BYTE * mainTileAddr;
    BYTE * auxTileAddr;
    BYTE * targetAddr;
    BYTE * maxTargetAddr;
    WORD maintileCoordinates;
    WORD auxTileCoordinates;
    BYTE blitMode;
    BYTE keyColor;
    WORD offset;
    int x,y;
    /* Determine tile blit mode */
    if (isSprite)
    {
        if (ptile->useKeyColor) blitMode = BS3_TILE_BLIT_KEYCOLOR;
        else /* AND Mask */
        {
            if (ptile->useSpecialMask)
            {
                if (ptile->useMaskFF)  blitMode = BS3_TILE_BLIT_XOR;
                else blitMode = BS3_TILE_BLIT_COPY;
            }
            else blitMode = BS3_TILE_BLIT_ANDOR;
        }
    }
    else /* tile map */
    {
        if (ptile->useKeyColor)
        {
            if (ptile->tileIsKeycolorFullbox)
                blitMode = BS3_TILE_BLIT_KEYCOLORTILE;
            else
                blitMode = BS3_TILE_BLIT_KEYCOLOR;
        }
        else /* AND mask */
        {
            if (ptile->useSpecialMask)
            {
                if (ptile->useMaskFF)
                {
                    if (ptile->ORasXOR)
                        blitMode = BS3_TILE_BLIT_XOR;
                    else
                        blitMode = BS3_TILE_BLIT_OR;
                }
                else blitMode = BS3_TILE_BLIT_COPY;
            } 
            else blitMode = BS3_TILE_BLIT_ANDOR;
        }
    }

    keyColor      = ptile->keyColor;
    targetCoordinates = (isSprite == 0 || (isSprite == 1 && ptile->tileCoordinate == 1))?((ptile->coordinates & 0x1F1F) << 3):ptile->coordinates;
    tileSize = 1 << (3 + (reg_bs3gfx.tilebanksize[ptile->surface & 0x01][ptile->mainTileBank & 0x03] & 0x03) ); /* 8 or 16 or 32 or 64 */
    offset = 256 - tileSize;
    srcSurface = &reg_bs3gfx.videoram[ptile->surface & 0x01][0];
    maintileCoordinates = bs3_gfx_bank_tile_coordinates(ptile->surface, ptile->mainTileBank, ptile->mainTileIndex);
    auxTileCoordinates  = bs3_gfx_bank_tile_coordinates(ptile->surface, ptile->auxTileBank,  ptile->auxTileIndex);
    if (maintileCoordinates == 0xFFFF || auxTileCoordinates == 0xFFFF) return; /* something wrong with the tiles index */
    mainTileAddr = srcSurface    + maintileCoordinates;
    auxTileAddr  = srcSurface    + auxTileCoordinates;
    targetAddr   = targetSurface + targetCoordinates;
    maxTargetAddr= targetSurface + 65536;
    switch (blitMode)
    {
        case BS3_TILE_BLIT_KEYCOLOR:
            for (y = 0 ; y < tileSize; y++)
            {
                if (targetAddr >= maxTargetAddr) break;
                for (x = 0 ; x < tileSize; x++)
                {
                    if (targetAddr >= maxTargetAddr) break;
                    if (*mainTileAddr != keyColor) *targetAddr = *mainTileAddr;
                    mainTileAddr++;
                    targetAddr++;
                }
                mainTileAddr += offset;
                targetAddr   += offset;
            }        
            break;
        case BS3_TILE_BLIT_KEYCOLORTILE:
            for (y = 0 ; y < tileSize; y++)
            {
                if (targetAddr >= maxTargetAddr) break;
                for (x = 0 ; x < tileSize; x++)
                {
                    if (targetAddr >= maxTargetAddr) break;
                    *targetAddr = keyColor;
                    targetAddr++;
                }
                targetAddr   += offset;
            }        
            break;
        case BS3_TILE_BLIT_ANDOR:
            for (y = 0 ; y < tileSize; y++)
            {
                if (targetAddr >= maxTargetAddr) break;
                for (x = 0 ; x < tileSize; x++)
                {
                    if (targetAddr >= maxTargetAddr) break;
                    *targetAddr = ((*targetAddr) & (*auxTileAddr)) | (*mainTileAddr);
                    mainTileAddr++;
                    auxTileAddr++;
                    targetAddr++;
                }
                mainTileAddr += offset;
                auxTileAddr  += offset;
                targetAddr   += offset;
            }        
            break;
        case BS3_TILE_BLIT_COPY:
            for (y = 0 ; y < tileSize; y++)
            {
                if (targetAddr >= maxTargetAddr) break;
                for (x = 0 ; x < tileSize; x++)
                {
                    if (targetAddr >= maxTargetAddr) break;
                    *targetAddr = *mainTileAddr;
                    mainTileAddr++;
                    targetAddr++;
                }
                mainTileAddr += offset;
                targetAddr   += offset;
            }        
            break;
        case BS3_TILE_BLIT_OR:
            for (y = 0 ; y < tileSize; y++)
            {
                if (targetAddr >= maxTargetAddr) break;
                for (x = 0 ; x < tileSize; x++)
                {
                    if (targetAddr >= maxTargetAddr) break;
                    *targetAddr |= *mainTileAddr;
                    mainTileAddr++;
                    targetAddr++;
                }
                mainTileAddr += offset;
                targetAddr   += offset;
            }        
            break;
        case BS3_TILE_BLIT_XOR:
            for (y = 0 ; y < tileSize; y++)
            {
                if (targetAddr >= maxTargetAddr) break;
                for (x = 0 ; x < tileSize; x++)
                {
                    if (targetAddr >= maxTargetAddr) break;
                    *targetAddr ^= *mainTileAddr;
                    mainTileAddr++;
                    targetAddr++;
                }
                mainTileAddr += offset;
                targetAddr   += offset;
            }        
            break;
    }
}

void bs3_gfx_command_refresh()
{
    /* compute layer compositing */
    int spritecount;
    int i;
    int x,y;
    WORD mintilex, mintiley, maxtilex, maxtiley;
    spritecount = 0;
    mintilex = (reg_bs3gfx.viewport_location & 0x00FF) >> 3;
    mintiley = (((reg_bs3gfx.viewport_location >> 8) & 0x00FF)) >> 3;
    maxtilex = (((reg_bs3gfx.viewport_location + reg_bs3gfx.viewport_size) & 0x00FF) - 1) >> 3;
    maxtiley = ((((reg_bs3gfx.viewport_location + reg_bs3gfx.viewport_size)>> 8) & 0x00FF) - 1) >> 3;
    mintilex = (mintilex<8)?0:mintilex;
    mintiley = (mintilex<8)?0:mintilex;

    for (i = 0 ; i < 128; i++)
    {
        if (reg_bs3gfx.sprite[i].enabled && 
            reg_bs3gfx.sprite[i].z < 3  ) spritecount++;
    }
    /* if bitmap only mode then do not do any layer composition*/
    if (reg_bs3gfx.tilemap[0].visible == 0 && reg_bs3gfx.tilemap[1].visible == 0 && spritecount == 0) 
    {
        _bs3_gfx_screenrender(255); /* 255 means render the surface referenced by the view port */
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
        return;
    }
    /* we need to do layer composition */

    /* layer 0 : get surface referenced by the view port */
    memcpy(&reg_bs3gfx.videoram[2][0], &reg_bs3gfx.videoram[reg_bs3gfx.viewport_surface][0], 65536);
    /* between layer 0 and 1 : sprites with Z == 0 */
    for (i = 0; i < 128; i++)
    {
        if (reg_bs3gfx.sprite[i].enabled && reg_bs3gfx.sprite[i].z == 0 )
        {
            bs3_gfx_tile_blit(&reg_bs3gfx.sprite[i], &reg_bs3gfx.videoram[2][0],  BS3_TILE_BLIT_SPRITE);
        }
    }
    /* layer 1 : tilemap 0 */
    if (reg_bs3gfx.tilemap[0].visible)
    {
        for (y = 0; y < 32 ; y++)
        {
            if (y > maxtiley) break;
            if (y >= mintiley )
            {
                for (x = 0 ; x < 32; x++)
                {
                    if (x > maxtilex) break;
                    if (x >= mintilex)
                    {
                        if (reg_bs3gfx.tilemap[0].map[y][x].visible)
                        {
                            bs3_gfx_tile_blit(&reg_bs3gfx.tilemap[0].map[y][x], &reg_bs3gfx.videoram[2][0], BS3_TILE_BLIT_TILEMAP);
                        }
                    }
                }
            }
        }
    }
    /* between layer 1 and 2 : sprites with Z == 1 */
    for (i = 0; i < 128; i++)
    {
        if (reg_bs3gfx.sprite[i].enabled && reg_bs3gfx.sprite[i].z == 1 )
        {
            bs3_gfx_tile_blit(&reg_bs3gfx.sprite[i], &reg_bs3gfx.videoram[2][0],  BS3_TILE_BLIT_SPRITE);
        }
    }
    /* layer 2 : tilemap 1 */
    if (reg_bs3gfx.tilemap[1].visible)
    {
        for (y = 0; y < 32 ; y++)
        {
            if (y > maxtiley) break;
            if (y >= mintiley )
            {
                for (x = 0 ; x < 32; x++)
                {
                    if (x > maxtilex) break;
                    if (x >= mintilex)
                    {
                        if (reg_bs3gfx.tilemap[1].map[y][x].visible)
                        {
                            bs3_gfx_tile_blit(&reg_bs3gfx.tilemap[1].map[y][x], &reg_bs3gfx.videoram[2][0], BS3_TILE_BLIT_TILEMAP);
                        }
                    }
                }
            }
        }
    }
    /* above layer 2 : sprite with Z == 2 */
    for (i = 0; i < 128; i++)
    {
        if (reg_bs3gfx.sprite[i].enabled && reg_bs3gfx.sprite[i].z == 2 )
        {
            bs3_gfx_tile_blit(&reg_bs3gfx.sprite[i], &reg_bs3gfx.videoram[2][0], BS3_TILE_BLIT_SPRITE);
        }
    }

    _bs3_gfx_screenrender(2); /* 2 means render the compositing result surface */
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
    if ((reg_bs3gfx.pb1 & 0x7F) > 1) /* good new surface ? */
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if ((reg_bs3gfx.pb1 & 0x80) && (reg_bs3gfx.pw2 & 0xE0E0))
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADCELLCOORDINATES;
        return;
    }
    /* apply change */
    reg_bs3gfx.viewport_surface  = reg_bs3gfx.pb1 & 0x01;
    reg_bs3gfx.viewport_location = (reg_bs3gfx.pb1 & 0x80)?((reg_bs3gfx.pw2 & 0x1F1F) << 3): reg_bs3gfx.pw2;
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
    surface = reg_bs3gfx.pb1 & 0x7F;
    if (surface & 0x7E) 
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
    surface = reg_bs3gfx.pb1 & 0x7F;
    if (surface & 0x7E) 
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
    surface = reg_bs3gfx.pb1 & 0x7F;
    value = reg_bs3gfx.pb3;
    i = reg_bs3gfx.pb4;
    if (surface & 0x7E) 
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
    surface = reg_bs3gfx.pb1 & 0x7F;
    value = reg_bs3gfx.pb3;
    i = reg_bs3gfx.pb4;
    if (surface & 0x7E) 
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

    surface = reg_bs3gfx.pb1 & 0x7F;
    value = reg_bs3gfx.pb3;
    if (surface & 0x7E) 
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

    surface = reg_bs3gfx.pb1 & 0x7F;
    value = reg_bs3gfx.pb3;
    if (surface & 0x7E) 
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
    
    sourceSurface   = reg_bs3gfx.pb1 & 0x7F;
    targetSurface   = reg_bs3gfx.pb5 & 0x7F;
    sourceAddr      = (reg_bs3gfx.pb1 & 0x80)? bs3_gfx_bank_tile_coordinates(sourceSurface, (BYTE)((reg_bs3gfx.pw2 & 0xFF00)>>8), (BYTE)(reg_bs3gfx.pw2 & 0x00FF)):reg_bs3gfx.pw2;
    targetAddr      = (reg_bs3gfx.pb5 & 0x80)?(reg_bs3gfx.pw6 & 0x1F1F) << 3:reg_bs3gfx.pw6;
    if (reg_bs3gfx.pb1 & 0x80) /* if tile mode surface , then PW4 is ignored and PW2 express a bank (MSB) then it is use to determine the size */
    {
        size = reg_bs3gfx.tilebanksize[sourceSurface & 0x01][((reg_bs3gfx.pw2 & 0xFF00)>>8) & 0x03];
        size = ((1 << (3+size)) << 8) | (1 << (3+size)) ; 
    }
    else 
    {
        size            = reg_bs3gfx.pw4;
    }
    operator        = reg_bs3gfx.pb3;

    h               = (int)((size & 0xFF00) >> 8);
    w               = (int)( size & 0x00FF);
    h               = h?h:256;
    w               = w?w:256;
    offset          = 256 - w;
    if ((sourceSurface & 0x7E) || (targetSurface & 0x7E)) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if ((reg_bs3gfx.pb1 & 0x80) && ((sourceAddr == 0xFFFF)))
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEINDEX;
        return;
    }
    if ((reg_bs3gfx.pb5 & 0x80) && (reg_bs3gfx.pw6 & 0xE0E0))
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADCELLCOORDINATES;
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
    
    sourceSurface   = reg_bs3gfx.pb1 & 0x7F;
    targetSurface   = reg_bs3gfx.pb5 & 0x7F;
    sourceAddr      = (reg_bs3gfx.pb1 & 0x80)? bs3_gfx_bank_tile_coordinates(sourceSurface, (BYTE)((reg_bs3gfx.pw2 & 0xFF00)>>8), (BYTE)(reg_bs3gfx.pw2 & 0x00FF)):reg_bs3gfx.pw2;
    targetAddr      = (reg_bs3gfx.pb5 & 0x80)?(reg_bs3gfx.pw6 & 0x1F1F) << 3:reg_bs3gfx.pw6;
    if (reg_bs3gfx.pb1 & 0x80) /* if tile mode surface , then PW4 is ignored and PW2 express a bank (MSB) then it is use to determine the size */
    {
        size = reg_bs3gfx.tilebanksize[sourceSurface & 0x01][((reg_bs3gfx.pw2 & 0xFF00)>>8) & 0x03];
        size = ((1 << (3+size)) << 8) | (1 << (3+size)) ; 
    }
    else 
    {
        size            = reg_bs3gfx.pw4;
    }
    keycolor        = reg_bs3gfx.pb3;

    h               = (int)((size & 0xFF00) >> 8);
    w               = (int)( size & 0x00FF);
    h               = h?h:256;
    w               = w?w:256;
    offset          = 256 - w;
    if ((sourceSurface & 0x7E) || (targetSurface & 0x7E)) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if ((reg_bs3gfx.pb1 & 0x80) && ((sourceAddr == 0xFFFF)))
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEINDEX;
        return;
    }
    if ((reg_bs3gfx.pb5 & 0x80) && (reg_bs3gfx.pw6 & 0xE0E0))
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADCELLCOORDINATES;
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
    WORD coordinates;
    if (reg_bs3gfx.pb1 & 0x7E)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if ((reg_bs3gfx.pb1 & 0x80) && (reg_bs3gfx.pw2 & 0xFC00))
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKID;
        return;
    }
    coordinates = (reg_bs3gfx.pb1 & 0x80)? bs3_gfx_bank_tile_coordinates(reg_bs3gfx.pb1, (BYTE)((reg_bs3gfx.pw2 & 0xFF00)>>8), (BYTE)(reg_bs3gfx.pw2 & 0x00FF)):reg_bs3gfx.pw2;
    if (coordinates == 0xFFFF) 
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEINDEX;
        return;
    }
    reg_bs3gfx.WAITFORDATA = 1;
    reg_bs3gfx.pw6 =  coordinates;
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

    targetSurface   = reg_bs3gfx.pb1 & 0x01;
    if (reg_bs3gfx.pb1 & 0x80) /* if tile mode surface , then PW4 is ignored and PW2 express a bank (MSB) then it is use to determine the size */
    {
        size = reg_bs3gfx.tilebanksize[targetSurface & 0x01][((reg_bs3gfx.pw2 & 0xFF00)>>8) & 0x03];
        size = ((1 << (3+size)) << 8) | (1 << (3+size)) ; 
    }
    else 
    {
        size            = reg_bs3gfx.pw4;
    }
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

void bs3_gfx_command_tile_bank_config()
{
    BYTE surface;
    BYTE bank;
    BYTE banksize;

    surface     = reg_bs3gfx.pb1 & 0x7F;
    bank        = reg_bs3gfx.pb2;
    banksize    = reg_bs3gfx.pb3;

    if (surface & 0xFE )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if (bank & 0xFC)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKID;
        return;
    }
    if (banksize > 3)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKSIZE;
        return;
    }
    reg_bs3gfx.tilebanksize[surface][bank] = banksize;
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_tile_bank_getconfig()
{
    BYTE surface;
    BYTE bank;

    surface     = reg_bs3gfx.pb1 & 0x7F; /* Ignore bit 7 : surface 1 == surface $81 , surface 0 == surface $80 */
    bank        = reg_bs3gfx.pb2;

    if (surface & 0x7E )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if (bank & 0xFC)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKID;
        return;
    }
    reg_bs3gfx.pb3 = reg_bs3gfx.tilebanksize[surface][bank];
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;

}

void bs3_gfx_command_tile_map_reset()
{
    BYTE tilemapid;
    int x,y;
    tilemapid = reg_bs3gfx.pb1;
    if (tilemapid & 0xFE)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEMAPID;
        return;
    }
    reg_bs3gfx.tilemap[tilemapid].visible = 0; /* tile map invisible */
    for ( y=0; y < 32 ; y++ )
    {
        for ( x = 0; x < 32 ; x++ )
        {
            reg_bs3gfx.tilemap[tilemapid].map[y][x].visible = 0; /* all tile map cell invisible */
        }
    }
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_tile_map_config()
{
    BYTE tilemapid;
    BYTE visible;
    int x,y;

    tilemapid   = reg_bs3gfx.pb1;
    visible     = reg_bs3gfx.pb2; 

    if (tilemapid & 0xFE)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEMAPID;
        return;
    }
    if (visible & 0xFE)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADVISIBILITY;
        return;
    }

    reg_bs3gfx.tilemap[tilemapid].visible = visible; 
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;

}

void bs3_gfx_command_tile_map_cell_config()
{
    BYTE tilemapid;
    WORD tilemapcellcoordinates;
    BYTE tilemapcelly;
    BYTE tilemapcellx;
    BYTE tilesurface;
    WORD tilebankindex;
    BYTE tilebank;
    BYTE tileindex;
    WORD tileauxbankindex;
    BYTE tileauxbank;
    BYTE tileauxindex;
    BYTE tilekeycolor;
    BYTE cellconfig;
    WORD cellmetadata;
    struct dev_bs3gfx_tile cell;

    /* get the information from the device registers */
    tilemapid               = reg_bs3gfx.pb1;
    tilemapcellcoordinates  = reg_bs3gfx.pw2;
    tilemapcellx            = (BYTE)(tilemapcellcoordinates & 0x00FF);
    tilemapcelly            = (BYTE)((tilemapcellcoordinates & 0xFF00) >> 8);
    tilesurface             = reg_bs3gfx.pb3 & 0x7F;  /* Ignore bit 7 : surface 1 == surface $81 , surface 0 == surface $80 */
    tilebankindex           = reg_bs3gfx.pw4;
    tilebank                = (BYTE)((tilebankindex & 0xFF00) >> 8);
    tileindex               = (BYTE)(tilebankindex & 0x00FF);
    tileauxbankindex        = reg_bs3gfx.pw5;
    tileauxbank             = (BYTE)((tileauxbankindex & 0xFF00) >> 8);
    tileauxindex            = (BYTE)(tileauxbankindex & 0x00FF);
    tilekeycolor            = reg_bs3gfx.pb5;
    cellconfig              = reg_bs3gfx.pb6;
    cellmetadata            = reg_bs3gfx.pw6;

    /* check value validity */
    if (tilemapid & 0xFE )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEMAPID;
        return;
    }
    if (tilemapcellx > 31 || tilemapcelly > 31)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADCELLCOORDINATES;
        return;
    }
    if (tilesurface & 0xFE )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if (tilebank > 3 )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKID;
        return;
    }
    if (tileindex > banknbtilespersize[reg_bs3gfx.tilebanksize[tilesurface][tilebank]] )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEINDEX;
        return;
    }
    cell.config = cellconfig;
    if (cell.useKeyColor == 0 && cell.useSpecialMask == 0) /* we've to check the auxtile index and bank*/
    {
        if (tileauxbank > 3 )
        {
            reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKID;
            return;
        }
        if (tileauxindex > banknbtilespersize[reg_bs3gfx.tilebanksize[tilesurface][tileauxbank]] )
        {
            reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEINDEX;
            return;
        }
    }
    if (cell.useKeyColor == 1 || (cell.useKeyColor == 0 && cell.useSpecialMask == 1 )) /* if aux tile not used then set default valid values */
    {
        tileauxbank     = 0;
        tileauxindex    = 0;
    }

    /* set the values in the tile map cell */
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].config            = cellconfig;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].coordinates       = tilemapcellcoordinates;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].surface           = tilesurface;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].mainTileBank      = tilebank;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].mainTileIndex     = tileindex;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].auxTileBank       = tileauxbank & 0x03;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].auxTileBank       = tileauxindex;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].keyColor          = tilekeycolor;
    reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].metadata          = cellmetadata;
    
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;

}

void bs3_gfx_command_tile_map_cell_getconfig()
{
    BYTE tilemapid;
    WORD tilemapcellcoordinates;
    BYTE tilemapcelly;
    BYTE tilemapcellx;
   
    /* get the information from the device registers */
    tilemapid               = reg_bs3gfx.pb1;
    tilemapcellcoordinates  = reg_bs3gfx.pw2;
    tilemapcellx            = (BYTE)(tilemapcellcoordinates & 0x00FF);
    tilemapcelly            = (BYTE)((tilemapcellcoordinates & 0xFF00) >> 8);

    /* check value validity */
    if (tilemapid & 0xFE )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEMAPID;
        return;
    }
    if (tilemapcellx > 31 || tilemapcelly > 31)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADCELLCOORDINATES;
        return;
    }

    /* get the values in the tile map cell to set into the respective register */
    reg_bs3gfx.pb6 = reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].config;
    reg_bs3gfx.pb3 = reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].surface & 0x01;
    reg_bs3gfx.pw4 = (((WORD)reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].mainTileBank & 0x03) << 8) | (WORD)reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].mainTileIndex;
    reg_bs3gfx.pw5 = (((WORD)reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].auxTileBank & 0x03) << 8) | (WORD)reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].auxTileBank;
    reg_bs3gfx.pb5 = reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].keyColor;
    reg_bs3gfx.pw6 = reg_bs3gfx.tilemap[tilemapid].map[tilemapcelly][tilemapcellx].metadata;
    
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;

}

void bs3_gfx_command_sprite_reset()
{
    int i;
    for (i = 0 ; i < 128 ; i++)
    {
        reg_bs3gfx.sprite[i].config         = 0;
        reg_bs3gfx.sprite[i].coordinates    = 0;
        reg_bs3gfx.sprite[i].keyColor       = 0;
        reg_bs3gfx.sprite[i].mainTileBank   = 0;
        reg_bs3gfx.sprite[i].mainTileIndex  = 0;
        reg_bs3gfx.sprite[i].auxTileBank    = 0;
        reg_bs3gfx.sprite[i].auxTileIndex   = 0;
        reg_bs3gfx.sprite[i].metadata       = 0;
    }
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_sprite_config()
{
    BYTE spriteid;
    WORD spritecoordinates;
    BYTE spritey;
    BYTE spritex;
    BYTE tilesurface;
    WORD tilebankindex;
    BYTE tilebank;
    BYTE tileindex;
    WORD tileauxbankindex;
    BYTE tileauxbank;
    BYTE tileauxindex;
    BYTE tilekeycolor;
    BYTE spriteconfig;
    WORD spritemetadata;
    struct dev_bs3gfx_tile sprite;

    /* get the information from the device registers */
    spriteid                = reg_bs3gfx.pb1;
    spritecoordinates       = reg_bs3gfx.pw2;
    spritex                 = (BYTE)(spritecoordinates & 0x00FF);
    spritey                 = (BYTE)((spritecoordinates & 0xFF00) >> 8);
    tilesurface             = reg_bs3gfx.pb3 & 0x7F;  /* Ignore bit 7 : surface 1 == surface $81 , surface 0 == surface $80 */
    tilebankindex           = reg_bs3gfx.pw4;
    tilebank                = (BYTE)((tilebankindex & 0xFF00) >> 8);
    tileindex               = (BYTE)(tilebankindex & 0x00FF);
    tileauxbankindex        = reg_bs3gfx.pw5;
    tileauxbank             = (BYTE)((tileauxbankindex & 0xFF00) >> 8);
    tileauxindex            = (BYTE)(tileauxbankindex & 0x00FF);
    tilekeycolor            = reg_bs3gfx.pb5;
    spriteconfig            = reg_bs3gfx.pb6;
    spritemetadata          = reg_bs3gfx.pw6;
    sprite.config           = spriteconfig;

    /* check value validity */
    if (spriteid & 0x80 )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSPRITEID;
        return;
    }
    if ((spritex > 31 || spritey > 31) && sprite.tileCoordinate == 1)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADCELLCOORDINATES;
        return;
    }
    if (tilesurface & 0xFE )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSURFACE;
        return;
    }
    if (tilebank > 3 )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKID;
        return;
    }
    if (tileindex > banknbtilespersize[reg_bs3gfx.tilebanksize[tilesurface][tilebank]] )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEINDEX;
        return;
    }
    
    if (sprite.useKeyColor == 0 && sprite.useSpecialMask == 0) /* we've to check the auxtile index and bank*/
    {
        if (tileauxbank > 3 )
        {
            reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADBANKID;
            return;
        }
        if (tileauxindex > banknbtilespersize[reg_bs3gfx.tilebanksize[tilesurface][tileauxbank]])
        {
            reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADTILEINDEX;
            return;
        }
    }
    if (sprite.useKeyColor == 1 || (sprite.useKeyColor == 0 && sprite.useSpecialMask == 1 )) /* if aux tile not used then set default valid values */
    {
        tileauxbank     = 0;
        tileauxindex    = 0;
    }

    /* set the values in the sprite */
    reg_bs3gfx.sprite[spriteid].config          = spriteconfig;
    reg_bs3gfx.sprite[spriteid].coordinates     = spritecoordinates;
    reg_bs3gfx.sprite[spriteid].surface         = tilesurface;
    reg_bs3gfx.sprite[spriteid].mainTileBank    = tilebank;
    reg_bs3gfx.sprite[spriteid].mainTileIndex   = tileindex;
    reg_bs3gfx.sprite[spriteid].auxTileBank     = tileauxbank & 0x03;
    reg_bs3gfx.sprite[spriteid].auxTileBank     = tileauxindex;
    reg_bs3gfx.sprite[spriteid].keyColor        = tilekeycolor;
    reg_bs3gfx.sprite[spriteid].metadata        = spritemetadata;
    
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;

}

void bs3_gfx_command_sprite_getconfig()
{
    BYTE spriteid;

    /* get the information from the device registers */
    spriteid = reg_bs3gfx.pb1;

    /* check value validity */
    if (spriteid & 0x80 )
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADSPRITEID;
        return;
    }
   
    /* set the values in the sprite */
    reg_bs3gfx.pb6 = reg_bs3gfx.sprite[spriteid].config;
    reg_bs3gfx.pw2 = reg_bs3gfx.sprite[spriteid].coordinates;
    reg_bs3gfx.pb3 = reg_bs3gfx.sprite[spriteid].surface & 0x01;
    reg_bs3gfx.pw4 = (((WORD)reg_bs3gfx.sprite[spriteid].mainTileBank) << 8) | (WORD)reg_bs3gfx.sprite[spriteid].mainTileIndex;
    reg_bs3gfx.pw5 = (((WORD)reg_bs3gfx.sprite[spriteid].auxTileBank)  << 8) | (WORD)reg_bs3gfx.sprite[spriteid].auxTileBank;
    reg_bs3gfx.pb5 = reg_bs3gfx.sprite[spriteid].keyColor;
    reg_bs3gfx.pw6 = reg_bs3gfx.sprite[spriteid].metadata;
    
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

BYTE bs3_gfx_isspritepixelvisible(BYTE sprite, WORD inCoords) 
{
    BYTE value = 0;
    WORD tileCoords;
    BYTE * pSurface;
    BYTE surface;
    struct dev_bs3gfx_tile * pSprite;

    surface = pSprite->surface & 0x01;
    pSprite = &reg_bs3gfx.sprite[sprite];
    pSurface = &reg_bs3gfx.videoram[surface][0];

    if (pSprite->useKeyColor)
    {
        tileCoords = bs3_gfx_bank_tile_coordinates(surface, pSprite->mainTileBank, pSprite->mainTileIndex) + inCoords;
        if (*(pSurface + tileCoords) != pSprite->keyColor ) return 1;
        return 0;
    }
    else if (pSprite->useSpecialMask)
    {
        if (pSprite->useMaskFF) /* OR / XOR only : then test pixel in mainTile*/
        {
            tileCoords = bs3_gfx_bank_tile_coordinates(surface, pSprite->mainTileBank, pSprite->mainTileIndex) + inCoords;
            if (*(pSurface + tileCoords)) return 1;
            return 0;            
        }
        else  /* COPY : then any pixel is visible*/
        {
            return 1;
        }
    }
    else /* AND+OR tile */
    {
        tileCoords = bs3_gfx_bank_tile_coordinates(surface, pSprite->auxTileBank, pSprite->auxTileIndex)   + inCoords;
        if (*(pSurface + tileCoords)) return 0;
        return 1;
    }

    return value; 
}

/* test if two sprites collide */
/* return 0 for non collision */
/* return 1 for collision */
int bs3_gfx_sprite_collide(BYTE spriteFrom , BYTE spriteTo)
{
    WORD spriteFromCoordTopLeft;
    WORD spriteToCoordTopLeft;
    WORD spriteFromCoordBottomRight;
    WORD spriteToCoordBottomRight;
    BYTE spriteFromSize;
    BYTE spriteToSize;
    WORD xmin1,ymin1,xmax1,ymax1,xmin2,ymin2,xmax2,ymax2;
    WORD x,y;
    BYTE spriteSmall;
    BYTE spriteBig;
    BYTE spriteSmallSize;
    BYTE spriteBigSize;
    BYTE pixelSpriteSmall;
    BYTE pixelSpriteBig;

    spriteFromSize             = 1 << (3 + reg_bs3gfx.tilebanksize[reg_bs3gfx.sprite[spriteFrom].surface & 0x01][reg_bs3gfx.sprite[spriteFrom].mainTileBank & 0x03] & 0x03); 
    spriteToSize               = 1 << (3 + reg_bs3gfx.tilebanksize[reg_bs3gfx.sprite[spriteTo  ].surface & 0x01][reg_bs3gfx.sprite[spriteTo  ].mainTileBank & 0x03] & 0x03); 
    spriteFromCoordTopLeft     = reg_bs3gfx.sprite[spriteFrom].coordinates; 
    spriteToCoordTopLeft       = reg_bs3gfx.sprite[spriteTo  ].coordinates; 
    spriteFromCoordBottomRight = spriteFromCoordTopLeft + spriteFromSize  - 0x0101;
    spriteToCoordBottomRight   = spriteToCoordTopLeft   + spriteToSize    - 0x0101;
    /* if sprite wrap over horizontally or vertically then no collision (convention to avoid to handle torus) : sprite too to the righ or too to the bottom */
    if ((spriteFromCoordBottomRight & 0xFF00) < (spriteFromCoordTopLeft & 0xFF00)) return 0;
    if ((spriteFromCoordBottomRight & 0x00FF) < (spriteFromCoordTopLeft & 0x00FF)) return 0;
    if ((spriteToCoordBottomRight & 0xFF00)   < (spriteToCoordTopLeft & 0xFF00))   return 0;
    if ((spriteToCoordBottomRight & 0x00FF)   < (spriteToCoordTopLeft & 0x00FF))   return 0;
    /* sprite box overlapped ? */
    
    if (spriteFromSize < spriteToSize)
    {
        spriteSmall     = spriteFrom;
        spriteSmallSize = spriteFromSize;
        spriteBig       = spriteTo;
        spriteBigSize   = spriteToSize;
        xmin1           = spriteFromCoordTopLeft     & 0x00FF;
        ymin1           = spriteFromCoordTopLeft     & 0xFF00;
        xmax1           = spriteFromCoordBottomRight & 0x00FF;
        ymax1           = spriteFromCoordBottomRight & 0xFF00;
        xmin2           = spriteToCoordTopLeft       & 0x00FF;
        ymin2           = spriteToCoordTopLeft       & 0xFF00;
        xmax2           = spriteToCoordBottomRight   & 0x00FF;
        ymax2           = spriteToCoordBottomRight   & 0xFF00;
    }
    else
    {
        spriteSmall     = spriteTo;
        spriteSmallSize = spriteToSize;
        spriteBig       = spriteFrom;
        spriteBigSize   = spriteFromSize;
        xmin1           = spriteToCoordTopLeft       & 0x00FF;
        ymin1           = spriteToCoordTopLeft       & 0xFF00;
        xmax1           = spriteToCoordBottomRight   & 0x00FF;
        ymax1           = spriteToCoordBottomRight   & 0xFF00;
        xmin2           = spriteFromCoordTopLeft     & 0x00FF;
        ymin2           = spriteFromCoordTopLeft     & 0xFF00;
        xmax2           = spriteFromCoordBottomRight & 0x00FF;
        ymax2           = spriteFromCoordBottomRight & 0xFF00;
    }

    if (ymax1 < ymin2 || ymax2 < ymin1 ) return 0; /* sprite boxes can't collide verticaly */
    if (xmax1 < xmin2 || xmax2 < xmin1 ) return 0; /* sprite boxes can't collide horitonaly */

    /* sprite box do overlap : collision possibility : do pixel perfect collision detection */
    /* todo : optimize if collisiton from left/right top or bottom to optimize browsing of the small sprite*/
    for (x = xmin1 ; x <= xmax1 ; x++) /* browse the small sprite horizontaly */
    {
        if (x > xmax2) break;
        if (x >= xmin2)
        {
            for (y = ymin1 ; y <= ymax1 ; y+=0x0100 ) /* browse the small sprite verticaly*/
            {
                if (y > ymax2) break;
                if (y >= ymin2)
                {
                    /* get pixel overlap mask value of spriteSmall*/
                    pixelSpriteSmall = bs3_gfx_isspritepixelvisible(spriteSmall, (y-ymin1) | (x-xmin1)); 
                    if (pixelSpriteSmall) /* small sprite pixel visible then check big sprite pixel visibility */
                    {
                    /* get pixel overlap mask value of spriteBig */
                        pixelSpriteBig = bs3_gfx_isspritepixelvisible(spriteBig, (y-ymin2) | (x-xmin2));
                        if (pixelSpriteBig) return 1; /* collision */
                    }
                }
            }
        }
    }
    return 0;    
}

void bs3_gfx_command_sprite_collision_count()
{
    int i,j;
    reg_bs3gfx.spritecollisioncount = 0;
    for (i = 128; i > 0; i--)
    {
        if (reg_bs3gfx.sprite[i].enabled && reg_bs3gfx.sprite[i].cancollide)
        {
            for (j = i-1; j > -1 ; j--)
            {
                if (reg_bs3gfx.sprite[j].enabled && reg_bs3gfx.sprite[j].cancollide)
                {
                    if (bs3_gfx_sprite_collide((BYTE)i,(BYTE)j))
                    {
                        reg_bs3gfx.spritecollision[reg_bs3gfx.spritecollisioncount++] = ((i & 0x00FF) << 8) | (j & 0x00FF);
                        reg_bs3gfx.spritecollision[reg_bs3gfx.spritecollisioncount++] = ((j & 0x00FF) << 8) | (i & 0x00FF);
                        if (reg_bs3gfx.spritecollisioncount >= 255) 
                        {
                            j = 128; i = 127; /* soft break; when collision buffer is full */
                        }
                    }
                }
            }
        }
    }
    reg_bs3gfx.pb1 = reg_bs3gfx.spritecollisioncount;
    reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_OK;
}

void bs3_gfx_command_sprite_getcollision()
{
    if (reg_bs3gfx.pb1 >= reg_bs3gfx.spritecollisioncount)
    {
        reg_bs3gfx.COMMAND_STATUS_CODE = BS3_GFX_STATUS_BADCOLLIDEID;
        return;
    }
    reg_bs3gfx.pw2 = reg_bs3gfx.spritecollision[reg_bs3gfx.pb1];
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
                    case BS3_GFX_COMMAND_TILE_BANK_CONFIG:
                        bs3_gfx_command_tile_bank_config();
                        break;
                    case BS3_GFX_COMMAND_TILE_BANK_GETCONFIG:
                        bs3_gfx_command_tile_bank_getconfig();
                        break;
                    case  BS3_GFX_COMMAND_TILE_MAP_RESET:
                        bs3_gfx_command_tile_map_reset();
                        break;
                    case BS3_GFX_COMMAND_TILE_MAP_CONFIG:
                        bs3_gfx_command_tile_map_config();
                        break;
                    case BS3_GFX_COMMAND_TILE_MAP_CELL_CONFIG:
                        bs3_gfx_command_tile_map_cell_config();
                        break;
                    case BS3_GFX_COMMAND_TILE_MAP_CELL_GETCONFIG:
                        bs3_gfx_command_tile_map_cell_getconfig();
                        break;
                    case BS3_GFX_COMMAND_SPRITE_RESET:
                        bs3_gfx_command_sprite_reset();
                        break;
                    case BS3_GFX_COMMAND_SPRITE_CONFIG:
                        bs3_gfx_command_sprite_config();
                        break;
                    case BS3_GFX_COMMAND_SPRITE_GETCONFIG:
                        bs3_gfx_command_sprite_getconfig();
                        break;
                    case BS3_GFX_COMMAND_SPRITE_COLLISION_COUNT:
                        bs3_gfx_command_sprite_collision_count();
                        break;
                    case BS3_GFX_COMMAND_SPRITE_GETCOLLISION:                        
                        bs3_gfx_command_sprite_getcollision();
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
    logoshowed = 0;
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
