    include "bs3core.inc"
    include "bs3gfx.inc"

        ; ROM resources
    ; Screen shots
; ROM screen shot address    
rom_scr_addr        equ     $E000    

; Screen shot ROM bank number
rom_sk_scr_death    equ     0
rom_sk_scr_end      equ     1
rom_sk_scr_logo     equ     2
rom_sk_scr_hv       equ     3
rom_sk_scr_intro1   equ     4
rom_sk_scr_intro2   equ     5
rom_sk_scr_menu     equ     6

    ; Game maps
; map layer address
rom_map_layer0_addr equ     $E000
rom_map_layer1_addr equ     $E0F0
rom_map_layer2_addr equ     $E1E0

; Map ROM bank number
rom_sk_map0         equ     7
rom_sk_map1         equ     8
rom_sk_map2         equ     9   
rom_sk_map3         equ     10
rom_sk_map4         equ     11
rom_sk_map5         equ     12
rom_sk_map6         equ     13
rom_sk_map7         equ     14
rom_sk_map8         equ     15
rom_sk_map9         equ     16
rom_sk_mapa         equ     17
rom_sk_mapb         equ     18
rom_sk_mapc         equ     19
rom_sk_mapd         equ     20
rom_sk_mape         equ     21

    ; Sprites data and mask
rom_sk_spr_data     equ     22
rom_sk_spr_mask     equ     23

    ; Tiles data and mask
rom_sk_tile_data    equ     24
rom_sk_tile_mask    equ     25
rom_sk_fonts        equ     26

    ; Embedded ROMs
    embed   "sk_scr_death.bs3",     rom_sk_scr_death,   0
    embed   "sk_scr_end.bs3",       rom_sk_scr_end,     0
    embed   "sk_scr_logo.bs3",      rom_sk_scr_logo,    0
    embed   "sk_scr_hv.bs3",        rom_sk_scr_hv,      0
    embed   "sk_scr_intro1.bs3",    rom_sk_scr_intro1,  0
    embed   "sk_scr_intro2.bs3",    rom_sk_scr_intro2,  0
    embed   "sk_scr_menu.bs3",      rom_sk_scr_menu,    0
    embed   "sk_map0.bs3",          rom_sk_map0,        0
    embed   "sk_map1.bs3",          rom_sk_map1,        0
    embed   "sk_map2.bs3",          rom_sk_map2,        0   
    embed   "sk_map3.bs3",          rom_sk_map3,        0
    embed   "sk_map4.bs3",          rom_sk_map4,        0
    embed   "sk_map5.bs3",          rom_sk_map5,        0
    embed   "sk_map6.bs3",          rom_sk_map6,        0
    embed   "sk_map7.bs3",          rom_sk_map7,        0
    embed   "sk_map8.bs3",          rom_sk_map8,        0
    embed   "sk_map9.bs3",          rom_sk_map9,        0
    embed   "sk_mapa.bs3",          rom_sk_mapa,        0
    embed   "sk_mapb.bs3",          rom_sk_mapb,        0
    embed   "sk_mapc.bs3",          rom_sk_mapc,        0
    embed   "sk_mapd.bs3",          rom_sk_mapd,        0
    embed   "sk_mape.bs3",          rom_sk_mape,        0
    embed   "sk_spr_data.bs3",      rom_sk_spr_data,    0
    embed   "sk_spr_mask.bs3",      rom_sk_spr_mask,    0
    embed   "sk_tile_data.bs3",     rom_sk_tile_data,   0
    embed   "sk_tile_mask.bs3",     rom_sk_tile_mask,   0
    embed   "sk_fonts.bs3",         rom_sk_fonts,       0

; Entry point
            mbs3_bootat start
start:
            mbs3_gfx_reset
            mov             b0, rom_sk_scr_hv
            c               showromscreenshot
            mbs3_wait_input
            mov             b0, rom_sk_scr_logo
            c               showromscreenshot
            mbs3_wait_input
            mov             b0, rom_sk_scr_menu
            c               showromscreenshot
            mbs3_wait_input
            mov             b0, rom_sk_scr_intro1
            c               showromscreenshot
            mbs3_wait_input
            mov             b0, rom_sk_scr_intro2
            c               showromscreenshot
            mbs3_wait_input
            
            mbs3_gfx_end
            hlt

; show ROM screen shot in current view port
; use B0 as ROM bank number that
; contains the screen shot to show
showromscreenshot:
            ; save env
            pusha
            ld      b1, [lbs3_bank_rom]
            sr      b1, [.oldrom]
            ; set rom bank with screenshot rombank
            sr      b0, [lbs3_bank_rom]
            ; get current view port surface,coords,size
            mbs3_gfx_vpgetconf
            ; set GFX reg PW4 = PW3
            ld      w1, [lbs3_gfx_reg3]
            mbs3_gfx_setPW4 w1
            ; start screen address
            mov     w3, rom_scr_addr
            ; start pixel transfer
            C       sk_scrblitrans
            mbs3_gfx_refresh
            ; restore env
            ld      b1, [.oldrom]
            sr      b1, [lbs3_bank_rom]
            popa
            ret
.oldrom      db      0    

sk_scrblitrans
                mbs3_gfx_blitrans
                cmp                 b0, ebs3_gfx_code_ok ; is it ok ?
                jnz                 .endblitrans ; not ok then quit
.transferloop                
                ld                  b0, [lbs3_gfx_status]
                tst                 b0, ebs3_gfx_status_waitdata
                jz                  .endblitrans ; end of transfer
                ld                  b0, [w3]
                mov                 b1, b0
                and                 b0, $0F 
                shr                 b1
                shr                 b1
                shr                 b1
                shr                 b1
                mbs3_gfx_setPB3     b0
                call                lbs3_gfx_busywait
                mbs3_gfx_setPB3     b1
                call                lbs3_gfx_busywait
                inc                 w3
                j                   .transferloop
.endblitrans
                ret

; end of sk.asm                