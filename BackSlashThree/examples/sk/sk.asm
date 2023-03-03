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

sk_spr_sword            equ     0   ; Sword
sk_spr_heart_red        equ     1   ; red heart
sk_spr_heart_empty      equ     2   ; empty heart
sk_spr_key_gray         equ     3   ; gray key
sk_spr_key_green        equ     4   ; green key
sk_spr_key_red          equ     5   ; red key
sk_spr_key_blue         equ     6   ; blue key
sk_spr_key_purple       equ     7   ; purple key
sk_spr_key_yellow       equ     8   ; yellow key
sk_spr_hero_right1      equ     9   ; hero face to right open legs
sk_spr_hero_right2      equ     10  ; hero face to right close legs
sk_spr_hero_left1       equ     11  ; hero face to left open legs
sk_spr_hero_left2       equ     12  ; hero face to left close legs
sk_spr_heart_yellow     equ     13  ; yellow heart
sk_spr_bottle_gray      equ     14  ; gray bottle
sk_spr_horse_right1     equ     15  ; horse face to right close legs
sk_spr_horse_right2     equ     16  ; horse face to right open legs
sk_spr_hero_right_ride  equ     17  ; hero face to right riding 
sk_spr_bat_sleep        equ     18  ; bat sleep
sk_spr_bat_fly1         equ     19  ; bat fly wings up
sk_spr_bat_fly2         equ     20  ; bat fly wings down
sk_spr_skelguard_right1 equ     21  ; sword skeleton right open legs
sk_spr_skelguard_right2 equ     22  ; sword skeleton right close legs
sk_spr_skelguard_left1  equ     23  ; sword skeleton left open legs
sk_spr_skelguard_left2  equ     24  ; sword skeleton left close legs
sk_spr_guard_right1     equ     25  ; sword guard right open legs
sk_spr_guard_right2     equ     26  ; sword guard right close legs
sk_spr_guard_left1      equ     27  ; sword guard left open legs
sk_spr_guard_left2      equ     28  ; sword guard left close legs
sk_spr_picks_ground     equ     29  ; surface small picks
sk_spr_skeleton_right1  equ     30  ; Skeleton face to right open legs
sk_spr_skeleton_right2  equ     31  ; Skeleton face to left close legs
sk_spr_skeleton_left1   equ     32  ; Skeleton face to left open legs
sk_spr_skeleton_left2   equ     33  ; Skeleton face to left close legs
sk_spr_fireskull_right  equ     34  ; fire skull face to right
sk_spr_fireskull_left   equ     35  ; fire skull face to left
sk_spr_herosword_right  equ     36  ; Hero sword attack to right
sk_spr_herosword_left   equ     37  ; Hero sword attack to left
sk_spr_fireball_right   equ     38  ; fireball to right
sk_spr_fireball_left    equ     39  ; fireball to left
sk_spr_whiteball        equ     40  ; white ball
sk_spr_circle_small     equ     41  ; small gray circle
sk_spr_circle_medium    equ     42  ; medium gray circle
sk_spr_circle_large     equ     43  ; large gray circle
sk_spr_fire_left        equ     44  ; furnace fire danse left
sk_spr_fire_right       equ     45  ; furnace fire danse right
sk_spr_boots            equ     46  ; boots
sk_spr_book             equ     47  ; spell book



; Entry point
            mbs3_bootat start
start:
            mbs3_gfx_reset
            ; load tiles and sprite into gfx device
            call            transromTileSpriteToGFX
            mov             b0, rom_sk_scr_hv
            call            showromscreenshot
            mbs3_gfx_refresh
            mbs3_wait_input
            mov             b0, rom_sk_scr_logo
            call            showromscreenshot
            mbs3_gfx_refresh
            mbs3_wait_input
            mov             b0, rom_sk_scr_menu
            call            showromscreenshot
            mbs3_gfx_refresh
            mbs3_wait_input
            mov             b0, rom_sk_scr_intro1
            call            showromscreenshot
            mbs3_gfx_refresh
            mbs3_wait_input
            mov             b0, rom_sk_scr_intro2
            call            showromscreenshot
            mbs3_gfx_refresh
            mbs3_wait_input
   
            ; shows all maps
            mov             b1, 0
.cont            
            ; clear viewport
            mbs3_gfx_setPB1     16
            mbs3_gfx_vpclear
            ; show map "b1"
            mov             b0,        b1 ; map "b1"
            call            sk_map_blit
            mbs3_gfx_refresh
            mbs3_wait_input
            inc             b1
            cmp             b1, 15
            jnz             .cont

            ; below is code to show the surface 1 (tile and spr banks)
            mbs3_gfx_vpgetconf
            mbs3_gfx_setPB1 1
            mbs3_gfx_vpconfig
            mbs3_gfx_refresh
            mbs3_wait_input

            mbs3_gfx_vpgetconf
            mbs3_gfx_setPB1 1
            mbs3_gfx_setPW2 $0060
            mbs3_gfx_vpconfig
            mbs3_gfx_refresh
            mbs3_wait_input


.byebye            
            mbs3_gfx_end
            hlt

; show ROM screen shot in current view port
; use B0 as ROM bank number that
; contains the screen shot to show
; after calling this routine, a GFX refresh is needed
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

; draw a sk map
; parameter
;    b0 : map number
sk_map_blit
                ; save env
                pusha
                ld                  b1, [lbs3_bank_rom]
                sr                  b1, [.oldrom]
                ; select rom map
                add                 b0, rom_sk_map0
                sr                  b0, [lbs3_bank_rom]
                ; select gfx image bank
                mov                 b1, 0 ; tile gfx bank
                ; first tile index address
                mov                 w2, rom_map_layer0_addr
                ; first gfx coordinate
                mov                 w1, $0000
                ; get tile index
.blit           ld                  b0, [w2] ; get image index
                ; blit tile b0 at w1(b3b2) coordinate on surface 0
                c                   blit8x8frombank ; blt tile b0 @ w1
                inc                 w2 ; next tile address
                add                 b2, 8
                cmp                 b2, 160
                jnz                 .blit
                eor                 b2, b2
                add                 b3, 8
                cmp                 b3, 96
                jnz                 .blit
                ; restore env
                ld                  b1, [.oldrom]
                sr                  b1, [lbs3_bank_rom]
                popa
                ret
.oldrom         db                  0

; blit a 8x8 gfx image bank
; parameters (registers)
;   b0 : gfx image index
;   b1 : gfx image bank
;   w1 : target coordinates on surface 0
blit8x8frombank
                pusha
                ; compute surface 1 coordinates (source)
                ;   compute image bank address
                eor         w2, w2
                mov         b5, b1
                shl         b5
                shl         b5
                shl         b5
                shl         b5
                shl         b5
                shl         b5  ; w2 = image row in bank
                ;   compute imnage address in bank
                eor         w3, w3
                mov         b6, b0
                and         b6, $1F
                shl         b6
                shl         b6
                shl         b6
                mov         b7, b0
                and         b7, $E0
                shr         b7
                shr         b7 ; w3 = offset addr in img bank
                ; w2 = address in surface (bank addr + offs img addr)
                add         w2, w3

                mbs3_gfx_setPB1     1       ; source surface
                mbs3_gfx_setPW2     w2      ; source coordinates
                mbs3_gfx_setPB3     232     ; key color (black)
                mbs3_gfx_setPW4     $0808   ; 8x8 image size
                mbs3_gfx_setPB5     0       ; target surface
                mbs3_gfx_setPW6     w1      ; target coordinates
                mbs3_gfx_blitkcolor

                popa
                ret


; transfer rom sk tiles to gfx surface
transromTileSpriteToGFX
                ; transfer tiles
                mov                 b0, rom_sk_tile_data
                mov                 b1, rom_sk_tile_mask
                mov                 b4, 0 ; tile gfx image bank
                mov                 b2, 0 ; rom img idx 
                mov                 b3, 0 ; gfx img idx 
.transOneTile   c                   trans8x8toGFX
                add                 w1, $0101
                cmp                 b2, 42
                jnz                 .transOneTile
                ; transfer sprite
                mov                 b0, rom_sk_spr_data
                mov                 b1, rom_sk_spr_mask
                mov                 b4, 1 ; tile gfx image bank
                mov                 b2, 0 ; rom img idx 
                mov                 b3, 0 ; gfx img idx 
.transOneSpr    c                   trans8x8toGFX
                add                 w1, $0101
                cmp                 b2, 48
                jnz                 .transOneSpr
                ret


; transfer  to GFX surface number 1
; one 8x8 image with "mask to key color" transform
; to GFX surface number 1
; parameters
;      b0 : rom bank number for data
;      b1 : rom bank number for mask
;      b2 : rom 8x8 image index (0 to 255)
;      b3 : GFX 8x8 image index in image bank
;      b4 : GFX 8x8 image bank number (0: tile, 1: sprite, ... 4)
trans8x8toGFX
                ; save env
                pusha
                ld                  b5, [lbs3_bank_rom]
                sr                  b5, [.oldrom]
                ; set variable
                sr                  b0, [.romdata]
                sr                  b1, [.rommask]
                sr                  b2, [.romimgidx]
                sr                  b3, [.gfximgidx]
                sr                  b4, [.gfximgbank]
                ; prepare computed parameters
                ;   compute rom bank address
                eor                 w0, w0
                ld                  b0, [.romimgidx]
                shl                 w0
                shl                 w0
                shl                 w0
                shl                 w0
                shl                 w0
                add                 w0, rom_scr_addr
                sr                  w0, [.romimgaddr] ; rom addr
                ;   compute GFX surface coordinates
                eor                 w0, w0
                ld                  b1, [.gfximgbank]
                shl                 b1
                shl                 b1
                shl                 b1
                shl                 b1
                shl                 b1
                shl                 b1
                
                eor                 w1, w1
                ld                  b2, [.gfximgidx]
                mov                 b3, b2
                and                 b2, $1F ; 0 to 31 Horiz image
                shl                 b2
                shl                 b2
                shl                 b2
                and                 b3, $E0 ; 0 to 3 Veti image
                shr                 b3
                shr                 b3
                add                 w0, w1
                sr                  w0, [.surfcoords] ; surface coords
                ;   prepare the mbs3_gfx_blitrans
                mbs3_gfx_setPB1     1
                mbs3_gfx_setPW2     w0
                mbs3_gfx_setPW4     $0808 ; 8x8 image
                mbs3_gfx_blitrans
                cmp                 b0, ebs3_gfx_code_ok ; is it ok ?
                jnz                 .endtrans ; not ok then quit
                ld                  w3, [.romimgaddr]
.transferloop 
                ld                  b0, [lbs3_gfx_status]
                tst                 b0, ebs3_gfx_status_waitdata
                jz                  .endtrans ; end of transfer
                ; get data and mask data from rom
                ld                  b0, [.romdata]
                sr                  b0, [lbs3_bank_rom]
                ld                  b2, [w3]
                ld                  b0, [.rommask]
                sr                  b0, [lbs3_bank_rom]
                ld                  b3, [w3]
                ; b2 = data, b3 = mask for 2 pixels 
                ; (1 byte = 2 pixels. MSq:first, LSq: second)
                ;  b4 = 1st pixel
                mov                 b4, b2
                shr                 b4
                shr                 b4
                shr                 b4
                shr                 b4
                ; first pixel : b5 4bits mask, b4 4bits data
                tst                 b5, $F0 ; 1st pixel mask
                jz                  .plot1st
                mov                 b4, 232 ; black key color
                ; transfer first pixel
.plot1st        mbs3_gfx_setPB3     b4
                call                lbs3_gfx_busywait
                ; b2 = 2nd pixel
                and                 b2, $0F
                tst                 b5, $0F ; 2nd pixel mask
                jz                  .plot2nd
                mov                 b2, 232 ; black key color
                ; transfer second pixel
.plot2nd        mbs3_gfx_setPB3     b2
                call                lbs3_gfx_busywait                
                inc                 w3
                j                   .transferloop
                ; restore env
.endtrans       ld                  b5, [.oldrom]
                sr                  b5, [lbs3_bank_rom]
                popa
                ret
.romdata        db                  0
.rommask        db                  0
.romimgidx      db                  0
.gfximgidx      db                  0
.gfximgbank     db                  0                
.oldrom         db                  0
                align               2
.surfcoords     dw                  0 
.romimgaddr     dw                  0              
; end of sk.asm                