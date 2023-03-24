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
rom_map_layer0_addr    equ     $E000
rom_map_layer1_offset  equ     $00F0
rom_map_layer2_offset  equ     $01E0

; map navigation
; x  -> y, z, s ; means the map x express way to go to map y, z and s
; 0  -> 1, 5
; 1  -> 0, 2
; 2  -> 1, 3, 5
; 3  -> 2, 4, 5, 7
; 4  -> 3, 6, 8
; 5  -> 0, 2, 3
; 6  -> 4
; 7  -> 3, 10
; 8  -> 4, 9
; 9  -> 8
; 10 -> 7, 11
; 11 -> 10, 12
; 12 -> 11, 13
; 13 -> 12
; ?? -> 14 (14 is an empty map)
; 14 -> ??
; map layout 
;  3 x (20 x 12) blocks
; 1st (20x12) blocks is tile reference
; 1st       2nd     3rd are   map metadata
; ?          0       ?  : Air ... can move into
; ?          1       ?  : player ground/wall 
; ?          2       ?  : Ledder, stairs, platform(can go down)
; when go to m2 from m1 then x,y corrdinates at m2
;  is where there is a "go to " m1
;  but if coords is at left border then move 1 block to the right
;      if coords is at right border then move 1 block to the left 
;  7         3       m  : door to go map 'm' (?m(x,y))
;  0         3       m  : border to go map 'm' (?m(x,y))

;  ?         4       0  : item, health ???
;  ?         4       1  : item, silver key
;  ?         4       2  : item, green key
;  ?         4       3  : item, red key
;  ?         4       4  : item, blue key
;  ?         4       5  : item, purple key
;  ?         4       6  : item, gold key
;  ?         4       7  : item, health plus (max health +1)
;  ?         4       8  : item, boots (double jump)
;  ?         4       9  : item, diamond (metamorphose bat)
;  ?         5       0  : enemy, bat
;  ?         5       1  : enemy, skeleton
;  ?         5       2  : enemy, red knight
;  ?         6       x  : health -= x (if health <=0 , then dead)
;  ?         7       0  : metamorphose hero to bat (if item diamond)
;  ?         7       1  : metamorphose bat to hero
;  ?         7      15  : game win, if all enemies are dead on map
;  28        8       ?  : healing + savepoint
;  ?         9       0  : emitter, fireball right
;  ?         9       1  : emitter, fireball left
;  ?         9       2  : emitter, fireskull right
;  ?         9       3  : emitter, fireskull left
;  ?         10      1  : lock, silver 
;  ?         10      2  : lock, green
;  ?         10      3  : lock, red
;  ?         10      4  : lock, blue
;  ?         10      5  : lock, purple
;  ?         10      6  : lock, gold

sk_object_item              equ     4
sk_object_enemy             equ     5
sk_object_enemy_bat         equ     0
sk_object_enemy_skeleton    equ     1
sk_object_enemy_redknight   equ     2
sk_object_emitter           equ     9
sk_object_emitter_fireballr equ     0
sk_object_emitter_fireballl equ     1
sk_object_emitter_fireskulr equ     2
sk_object_emitter_fireskull equ     3

sk_object_tomap             equ     3


sk_sprid_hero       equ     $78
sk_sprid_heroswordl equ     $79
sk_sprid_heroswordr equ     $7A
sk_sprid_item       equ     $60
sk_sprid_item_last  equ     $6A 
sk_sprid_bot_blk    equ     $0
sk_sprid_bot_heart  equ     $20
sk_sprid_bot_item   equ     $30
sk_sprid_enemie     equ     $40
sk_sprid_furnfire   equ     $77
sk_sprid_blip       equ     $76  

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
rom_sk_fonts_data   equ     26
rom_sk_fonts_mask   equ     27
rom_sk_fonts_width  equ     28

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
    embed   "sk_fonts_data.bs3",    rom_sk_fonts_data,  0
    embed   "sk_fonts_mask.bs3",    rom_sk_fonts_mask,  0
    embed   "sk_fonts_width.bs3",   rom_sk_fonts_width, 0


sk_tile_keycolor        equ     232
;tiles for sprites : defined by a word (16 bits)
; MSB : bank number
; LSB : index number
sk_spr_black            equ    $0000 ; Black opaque square
sk_spr_sword            equ    $0100 ; Sword
sk_spr_heart_red        equ    $0101 ; red heart
sk_spr_heart_empty      equ    $0102 ; empty heart
sk_spr_key_gray         equ    $0103 ; gray key
sk_spr_key_green        equ    $0104 ; green key
sk_spr_key_red          equ    $0105 ; red key
sk_spr_key_blue         equ    $0106 ; blue key
sk_spr_key_purple       equ    $0107 ; purple key
sk_spr_key_yellow       equ    $0108 ; yellow key
sk_spr_hero_right1      equ    $0109 ; hero face to right open legs
sk_spr_hero_right2      equ    $010A ; hero face to right close legs
sk_spr_hero_left1       equ    $010B ; hero face to left open legs
sk_spr_hero_left2       equ    $010C ; hero face to left close legs
sk_spr_heart_yellow     equ    $010D ; yellow heart
sk_spr_bottle_gray      equ    $010E ; gray bottle
sk_spr_horse_right1     equ    $010F ; horse face to right close legs
sk_spr_horse_right2     equ    $0110 ; horse face to right open legs
sk_spr_hero_right_ride  equ    $0111 ; hero face to right riding 
sk_spr_bat_sleep        equ    $0112 ; bat sleep
sk_spr_bat_fly1         equ    $0113 ; bat fly wings up
sk_spr_bat_fly2         equ    $0114 ; bat fly wings down
sk_spr_skelguard_right1 equ    $0115 ; sword skeleton right open legs
sk_spr_skelguard_right2 equ    $0116 ; sword skeleton right close legs
sk_spr_skelguard_left1  equ    $0117 ; sword skeleton left open legs
sk_spr_skelguard_left2  equ    $0118 ; sword skeleton left close legs
sk_spr_guard_right1     equ    $0119 ; sword guard right open legs
sk_spr_guard_right2     equ    $011A ; sword guard right close legs
sk_spr_guard_left1      equ    $011B ; sword guard left open legs
sk_spr_guard_left2      equ    $011C ; sword guard left close legs
sk_spr_picks_ground     equ    $011D ; surface small picks
sk_spr_skeleton_right1  equ    $011E ; Skeleton face to right open legs
sk_spr_skeleton_right2  equ    $011F ; Skeleton face to left close legs
sk_spr_skeleton_left1   equ    $0120 ; Skeleton face to left open legs
sk_spr_skeleton_left2   equ    $0121 ; Skeleton face to left close legs
sk_spr_fireskull_right  equ    $0122 ; fire skull face to right
sk_spr_fireskull_left   equ    $0123 ; fire skull face to left
sk_spr_herosword_right  equ    $0124 ; Hero sword attack to right
sk_spr_herosword_left   equ    $0125 ; Hero sword attack to left
sk_spr_fireball_right   equ    $0126 ; fireball to right
sk_spr_fireball_left    equ    $0127 ; fireball to left
sk_spr_whiteball        equ    $0128 ; white ball
sk_spr_circle_small     equ    $0129 ; small gray circle
sk_spr_circle_medium    equ    $012A ; medium gray circle
sk_spr_circle_large     equ    $012B ; large gray circle
sk_spr_fire_left        equ    $012C ; furnace fire danse left
sk_spr_fire_right       equ    $012D ; furnace fire danse right
sk_spr_boots            equ    $012E ; boots
sk_spr_book             equ    $012F ; spell book


; Entry point
            mbs3_bootat start

handler_timer
            push_w0
            ld      w0, [timer_count]
            inc     w0
            sr      w0, [timer_count]
            pop_w0
            iret 

handler_dummy
            iret 

timer_count dw 0

; w0 number of 1/32 second
timer_setperiod
            pusha
            pushf
            cli
            eor             w1, w1
            sr              w1, [timer_count]
            mov             w2, $7A12 ; 1/32 second
            mul             w2, w0, w3
            mbs3_starttimer w3, w2
            popf
            popa
            ret                     

timer_resetcount
            pushf
            push_w0
            cli
            eor             w0, w0
            sr              w0, [timer_count]
            pop_w0
            popf
            ret

timer_wait_tick
            push_w0
            push_w1
            pushf
            cli
            ld              w0, [timer_count]
            popf
.retry      ld              w1, [timer_count]
            cmp             w1, w0
            jz              .retry
            pop_w1
            pop_w0
            ret

; wait w0 times tick
timer_wait_ntick
            push_w0
.loop       cmp             w0, 0
            jz              .exit
            call            timer_wait_tick
            dec             w0
            jump            .loop
.exit            
            pop_w0
            ret
start:
            ; prepare handler
            cli
            mbs3_sethandler         int_timer,       handler_timer
            mbs3_sethandler         int_byteinput,   handler_dummy
            mbs3_sethandler         int_rtc72421,    handler_dummy
            mbs3_settimermode       ebs3_timer_mode_time
            mov                 w0, $0001
            call                timer_setperiod ; 1/32 sec. tick period
            sti
            ; init var
            mov                 b0, $ff
            sr                  b0, [sk_curr_map]
            ; init gfx
            mbs3_gfx_reset
            ; load tiles and sprite into gfx device
            call            transromTileSpriteToGFX
            mov             b0, rom_sk_scr_hv
            call            showromscreenshot
            mbs3_gfx_refresh
            mov             w0, $0040
            call            timer_wait_ntick
           ; mbs3_wait_input
            mov             b0, rom_sk_scr_logo
            call            showromscreenshot
            mbs3_gfx_refresh
            mov             w0, $0040
            call            timer_wait_ntick
            ; mbs3_wait_input
.menu       call            sk_menu
            cmp             b0, 0
            jz              .menu_new_game
            cmp             b0, 1
            jz              .menu_continue
            cmp             b0, 2
            jz              .menu_exit
            jump            .menu ; should never occurs
.menu_new_game
            jump            .new_game
.menu_continue
            jump            .continue
.menu_exit
            jump            .byebye

; ******* NEW GAME
.new_game
            ; render only surface
            mbs3_gfx_setPB1 1        ; only layer 0 rendered
            mbs3_gfx_setPB2 0        ; no sprite rendered
            mbs3_gfx_renderconf
            
            ; reset tile maps
            mbs3_gfx_setPB1     0 ; choose tilemap 0
            mbs3_gfx_tmreset      
            ; reset sk data
            call            sk_init_data
            ; show intro
            mov             b0, rom_sk_scr_intro1
            call            showromscreenshot
            mbs3_gfx_refresh
            mov             w0, $0040
            call            timer_wait_ntick            
            mov             b0, rom_sk_scr_intro2
            call            showromscreenshot
            mbs3_gfx_refresh
            mov             w0, $0040
            call            timer_wait_ntick

            ; render sprite and layers
            mbs3_gfx_setPB1 7        ; all layers rendered
            mbs3_gfx_setPB2 7        ; all sprite Z rendered
            mbs3_gfx_renderconf

            ; set default first hero location and looks
            ;  location is set to hero map0 location 
            mbs3_gfx_setPB1 sk_sprid_hero       ; sprite id hero
            mbs3_gfx_setPW2 $1018               ; coords x=24,y=16
            mbs3_gfx_setPB3 1                   ; tile surface
            mbs3_gfx_setPW4 sk_spr_hero_right1  ; set index/bank
            mbs3_gfx_setPB5 sk_tile_keycolor    ; keycolor
            mbs3_gfx_setPB6 $A1                 ; enabled|z=1|keycolor
            mbs3_gfx_setPW6 $0000               ; reset metadata 1
            mbs3_gfx_setPW7 $0000               ; reset metadata 2
            mbs3_gfx_sprconf                    ; sprite config
            ; prepare sprites for bottom status
            call            sk_preparebottom
            call            sk_updatebottom
            ; shows all maps
            mov             b1, 0
.cont            
            ; clear viewport
            mbs3_gfx_setPB1     16
            mbs3_gfx_vpclear


            ; show map "b1"
.cont2      mov             b0, b1 ; map "b1"
            call            sk_tilemap ; sk_map_blit
.refresh            
            call            sk_sprite_anim
            mbs3_gfx_refresh
            call            timer_wait_tick ;wait
.getinput            
            in              b0
            jz              .noaction
            and             b0, $7F ; remove 7th bit
            cmp             b0, 10
            jz              .cont4
            cmp             b0, ' ' ; attack
            jz              .action            
            cmp             b0, 'A' ; 
            jl              .getinput
            cmp             b0, 'D' ; down
            ja              .getinput
            j               .action
.noaction            
            eor             b0, b0 ; no hero action
.action
            call            sk_hero_action            
            jump            .refresh
            ;bs3_wait_input

.cont4
            inc             b1
            cmp             b1, 15
            jnz             .cont2 
            jump            .menu

; ******* CONTINUE
.continue
            ; hide tile map 0
            mbs3_gfx_setPB1 0
            mbs3_gfx_setPB2 0
            mbs3_gfx_tmconfig
            ; below is code to show the surface 1 (tile and spr banks)
            mbs3_gfx_vpgetconf
            mbs3_gfx_setPB1 1
            mbs3_gfx_vpconfig
            mbs3_gfx_refresh
            mbs3_wait_input
            mbs3_gfx_setPW2     $6400
            mbs3_gfx_vpconfig
            mbs3_gfx_refresh
            mbs3_wait_input
            mbs3_gfx_setPW2     $6460
            mbs3_gfx_vpconfig
            mbs3_gfx_refresh
            mbs3_wait_input


            mbs3_gfx_vpgetconf
            mbs3_gfx_setPB1 1
.blop
            mov             w1, 0
.again1     mbs3_gfx_setPW2     w1
            mbs3_gfx_vpconfig
            mbs3_gfx_refresh
            inc             b2
            cmp             b2, $60
            jnz             .again1

            mov             w1, $0060
.again2     mbs3_gfx_setPW2     w1
            mbs3_gfx_vpconfig
            mbs3_gfx_refresh
            dec             b2
            cmp             b2, $ff
            jnz             .again2

            in              b0
            jnz              .cont3
            jump            .blop

.cont3      
            mbs3_gfx_vpgetconf
            mbs3_gfx_setPB1 0
            mbs3_gfx_setPW2 0
            mbs3_gfx_vpconfig
            jump            .menu



; ******* EXIT
.byebye            
            mbs3_gfx_end
            hlt


sk_init_data
            pusha
            mov     b0, $FF
            sr      b0, [sk_prev_map]
            sr      b0, [sk_curr_map]
            mov     b0, 3
            sr      b0, [sk_max_life]
            sr      b0, [sk_curr_life]
            mov     w0, sk_have_item
            mov     w1, $0110
            eor     b7, b7
.loop            
            cmp     w1, 0
            jz      .quitroutine
            sr      b7, [w0]
            inc     w0
            dec     w1
            j       .loop
.quitroutine
            popa
            ret
;
; Hero info:
; pulse: vertical value (8 bits: countdown)
;        horizontal value (8 bits countdown)
; status :
;       7 6 5 4 3 2 1 0
;       | | | | | | | +- 0: Pulse horiz. right
;       | | | | | | |    1: Pulse horiz. left
;       | | | | | | | 
;       | | | | | | +--- 0: Pulse verti. down
;       | | | | | |      1: pulse verti. up
;       | | | | | | 
;       | | | | | +----- 0: Mood Passive 
;       | | | | |        1: Mood Attack
;       | | | | |
;       | | | | +------- 0: Human Normal
;       | | | |          1: Human invincible (blink)
;       | | | |
;       | | | +--------- 0: Double jump disabled
;       | | |            1: Double jump enabled
;       | | |
;       | | +----------- 0: Double jump not used
;       | |              1: Double jump used
;       | | 
;       | +------------- 0: Morph Bat disabled
;       |                1: Morph Bat enabled
;       |
;       +--------------- 0: Morph Hero
;                        1: Morph Bat
;
;  counter: 8 bits countdown (for invincibility )
; 
;  tile on and around Hero sprite
;            (x,y-1)       (x+7,y-1)
;             u1                  u2
;            +---------------------+
; (x-1,y)  l1|(x,y) (spr coord)    |r1 (x+8,y)
;            |                     |
;            |                     |
;            |        m(x+3,y+3)   |
;            |                     |
;            |                     |
;            |                     |
; (x-1,y+7)l2|                     |r2 (x+8,y+7)
;            +---------------------+
;             d1                  d2 
;             (x,y+8)             (x+7, y+8)
; Alternative coordinates
; X = x-1, Y = y-1
; u1 = (X+1, Y+0)
; u2 = (X+8, Y+0)
; l1 = (X+0, Y+1)
; l2 = (X+0, Y+8)
; d1 = (X+1, Y+9)
; d2 = (X+8, Y+9)
; r1 = (X+9, Y+1)
; r2 = (X+9, Y+8)
; m = (X+4, Y+4)
; function canmove(x,y, tiledir, action)
;   X = tile coord de x
;   Y = tile coord de y
;   if X >=20 return false
;   if Y >=12 return false 
;   T = tile type at (X,Y)
;   P = tile parameter at (X, Y)
;   if T = 0 return true 
;   if T = 1 return false
;   if T = 2
;     if tiledir != 'DOWN' return true
;     if (y  & $07 != 0) return true
;     if (action = 'go down') return true
;     return false
;   if T = 10 (locked block)
;      if P is owned (key item) return true
;      return false
;
;  // POSSIBLE MOVE
;
; u,d,l,r are boolean to indicate if move is possible
; u = canmove(X+1,Y+0, 'UP' , action)
; if u 
;   u = canmove(X+8, Y+0, 'UP', action)
; d = canmove(X+1, Y+9, 'DOWN', action)
; if d 
;    d= canmove(X+8, Y+9, 'DOWN', action)
; l = canmove(X+0, Y+1, 'LEFT', action)
; if l
;    l = canmove(X+0, Y+8, 'LEFT', action)
; r = canmove(X+9, Y+1, 'RIGHT', action)
; if r
;    r = canmove(X+9, Y+8)
;
; if !d & vertical pulse = 0
;    double jump not used
; 
; // ACTION to pulse 
;
; if action = 'go up'
;    if hero morph human
;      if !d & u & vertical pulse = 0
;                vertical pulse is up and = 6
;      else
;         if d & u and dbl jump enabled and double jump is not use
;                vertical pulse is up and = 6
;                spawn blip sprite at hero location 
;    else // hero is bat
;      if u 
;           vertical pulse is up and = 2
;
; if action = 'go down'
;     if d & vertical pulse = 0
;              vertical pulse is down and = 2 (or P ?)
; 
; if action = 'go left' & l
;     if hero morph human
;         horiz pulse is left and = 2
;     else // hero morph bat
;         horiz pulse is left and = 20
;
; if action = 'go right' & r 
;     if hero morph human 
;         horiz pulse is right and = 2
;     else // hero morph bat
;         horiz pulse is left and = 20
;
; // GRAVITY pulse
; if d & vertical pulse = 0 
;      vertical pulse is down and = 1
;
; // PHYSICAL pulse cancellation
; if pulse vertical is up and !u
;    pulse vertical = 0
; if pulse vertical is down and !d 
;    pulse verticial = 0
; if pulse horiz is left and !l 
;    pulse horiz = 0
; if pulse horiz is right and !r 
;    pulse horiz = 0
;
;  // APPEARANCE
; Hero sprite Z = 1
; if  hero invicible & countdown > 0
;   countdown--
;   if tick & 04 != 0 
;      Hero Sprite Z = 3
;   if countdown == 0 
;     hero is normal (not invicible)
; 
; if hero morph human 
;   if horiz pulse is left
;      hero tile face to left open leg
;      if horiz pulse > 0 and tick & 02 != 0
;         hero tile face to left close leg
;   else
;      hero tile face to right open leg
;      if horiz pulse > 0 and tick & 02 != 0
;         hero tile face to right close leg
; else (morph bat)
;      hero tile bat up wings
;      if tick & 02 != 0
;         hero tile bat down wings
;
; // LOCATION update by pulse
; if pulse verical > 0
;    pulse vertical--
;    if pulse vertical is up
;       hero y -=2
;    else
;       hero y +=2
;
; if pulse horiz > 0
;    pulse horiz--
;    if pulse horiz is left
;        hero x -=2
;    else
;        hero x +=2
;
; // tile interact (Hurt, change map, morph to map/human, furnace)
;
; // Sprite interact
;
;
; b0 =action  
;      'A' up , 'B' down, 'C' right, 'D' left
;      ' ' attack , $00 no action    
sk_hs_pulseh_mask   equ     $01
sk_hs_pulseh_right  equ     $00
sk_hs_pulseh_left   equ     $01
sk_hs_pulsev_mask   equ     $02
sk_hs_pulsev_down   equ     $00
sk_hs_pulsev_up     equ     $02
sk_hs_mood_mask     equ     $04
sk_hs_mood_passive  equ     $00
sk_hs_mood_attack   equ     $04
sk_hs_human_mask    equ     $08
sk_hs_human_normal  equ     $00
sk_hs_human_invicible equ   $08
sk_hs_2jump_mask    equ     $10
sk_hs_2jump_disable equ     $00
sk_hs_2jump_enable  equ     $10
sk_hs_u2jump_mask   equ     $20
sk_hs_u2jump_nused  equ     $00
sk_hs_u2jump_used   equ     $20
sk_hs_morphbat_mask equ     $40
sk_hs_morphbat_disable equ  $00
sk_hs_morphbat_enable  equ  $40
sk_hs_morph_mask    equ     $80
sk_hs_morph_hero    equ     $00
sk_hs_morph_bat     equ     $80

sk_hero_action
            pusha
            sr      b0, [.action]; store action 
            ; get sprite hero info
            mbs3_gfx_setPB1 sk_sprid_hero
            mbs3_gfx_sprgetconf
            ld      w0, [lbs3_gfx_reg2]
            sr      w0, [.hero_coords]
            ld      w0, [lbs3_gfx_reg6]
            sr      w0, [.hero_meta1]
            ld      w0, [lbs3_gfx_reg7]
            sr      w0, [.hero_meta2]
            ld      w0, [lbs3_gfx_reg4]
            sr      w0, [.hero_bankindex]
            ld      b0, [lbs3_gfx_reg6]
            sr      b0, [.hero_config]
            ; get surrounding tile info
            ;  center
            ld      w1, [.hero_coords]
            sub     w1, $0101
            add     w1, $0404
            call    sk_map_getmeta
            sr      w2, [.htm_center]
            ;  left
            ld      w1, [.hero_coords]
            sub     w1, $0101
            add     w1, $0400
            call    sk_map_getmeta
            sr      w2, [.htm_left]
            ;  right
            ld      w1, [.hero_coords]
            sub     w1, $0101
            add     w1, $0409
            call    sk_map_getmeta
            sr      w2, [.htm_right]
            ;  top
            ld      w1, [.hero_coords]
            sub     w1, $0101
            add     w1, $0004
            call    sk_map_getmeta
            sr      w2, [.htm_top]
            ;  bottom
            ld      w1, [.hero_coords]
            sub     w1, $0101
            add     w1, $0904
            call    sk_map_getmeta
            sr      w2, [.htm_bottom]
            ; compute hero update
            cmp     b4, 0
            jnz     .cont1
            ld      w0, [.hero_coords]
            add     w0, $0200
            sr      w0, [.hero_coords]

.cont1

            ; set hero sprite data
            ld      w0, [.hero_coords]
            sr      w0, [lbs3_gfx_reg2]
            ld      w0, [.hero_bankindex]
            sr      w0, [lbs3_gfx_reg4]
            ld      b0, [.hero_config]
            sr      b0, [lbs3_gfx_reg6]
            ld      w0, [.hero_meta1]
            sr      w0, [lbs3_gfx_reg6]
            ld      w0, [.hero_meta2]
            sr      w0, [lbs3_gfx_reg7]
            mbs3_gfx_sprconf
            popa
            ret
.action             db      0

.hero_config        db      0
.hero_coords        dw      0
.hero_bankindex     dw      0
.hero_meta1     
.hero_status        db      0
.hero_counter       db      0
.hero_meta2     
.hero_pulsehoriz    db      0
.hero_pulseverti    db      0

.htm_center
.htm_center_l1      db      0
.htm_center_l2      db      0
.htm_top
.htm_top_l1         db      0
.htm_top_l2         db      0
.htm_bottom
.htm_bottom_l1      db      0
.htm_bottom_l2      db      0
.htm_left
.htm_left_l1        db      0
.htm_left_l2        db      0
.htm_right
.htm_right_l1       db      0
.htm_right_l2       db      0


; w1 = pixel coordinate
; return w2 : metainfo1 of tile at coordinate
sk_map_getmeta
            pusha
            mbs3_gfx_pushparameters
            and     w1,$F8F8
            shr     w1
            shr     w1
            shr     w1
            mbs3_gfx_setPB1 0 ; tilemap 0
            mbs3_gfx_setPW2 w1 ; tile coordinates
            mbs3_gfx_tmcellgetconf
            ld      w1, [lbs3_gfx_reg6]
            sr      w1, [.metainfo1]
            mbs3_gfx_popparameters
            popa
            ld      w2, [.metainfo1]
            ret
.metainfo1  dw      0

sk_preparebottom
            pusha
            ; black Sprites
            mov     b2, sk_sprid_bot_blk
            eor     b3, b3
            mov     w2, $5C00
.nextblack            
            mbs3_gfx_setPB1 b2        
            mbs3_gfx_setPW2 w2                  ; coords
            mbs3_gfx_setPB3 1                   ; tile surface
            mbs3_gfx_setPW4 sk_spr_black        ; set index/bank
            mbs3_gfx_setPB5 sk_tile_keycolor    ; keycolor
            mbs3_gfx_setPB6 $A1                 ; enabled|z=1|keycolor
            mbs3_gfx_sprconf                    ; sprite config
            add     w1, $0101
            add     w2, $0008
            cmp     b3, 20
            jnz     .nextblack

            ; heart sprites
            mov     b2, sk_sprid_bot_heart
            eor     b3, b3
            mov     w2, $5D01
.nextheart
            mbs3_gfx_setPB1 b2        
            mbs3_gfx_setPW2 w2                  ; coords
            mbs3_gfx_setPB3 1                   ; tile surface
            mbs3_gfx_setPW4 sk_spr_heart_empty  ; set index/bank
            mbs3_gfx_setPB5 sk_tile_keycolor    ; keycolor
            mbs3_gfx_setPB6 $A1                 ; enabled|z=1|keycolor
            mbs3_gfx_sprconf                    ; sprite config
            add     w1, $0101
            add     w2, $0008
            cmp     b3, 6
            jnz     .nextheart

            ; item sprites
            mov     b2, sk_sprid_bot_item
            eor     b3, b3
            mov     w2, $5D99
.nextitem
            mbs3_gfx_setPB1 b2        
            mbs3_gfx_setPW2 w2                  ; coords
            mbs3_gfx_setPB3 1                   ; tile surface
            mbs3_gfx_setPW4 sk_spr_black        ; set index/bank
            mbs3_gfx_setPB5 sk_tile_keycolor    ; keycolor
            mbs3_gfx_setPB6 $A1                 ; enabled|z=1|keycolor
            mbs3_gfx_sprconf                    ; sprite config
            add     w1, $0101
            sub     w2, $0008
            cmp     b3, 14
            jnz     .nextitem                        
            popa
            ret

sk_updatebottom
            pusha
            mov     b2, sk_sprid_bot_heart
            ld      b4, [sk_curr_life]
            ld      b5, [sk_max_life]
            mov     b6, 6
.currlife            
            cmp     b4, 0
            jz      .maxlife
            mbs3_gfx_setPB1 b2
            mbs3_gfx_sprgetconf
            mbs3_gfx_setPW4 sk_spr_heart_red
            mbs3_gfx_sprconf
            inc     b2
            dec     b4
            dec     b5
            dec     b6
            j       .currlife

.maxlife
            cmp     b5,0
            jz      .blanklife
            mbs3_gfx_setPB1 b2
            mbs3_gfx_sprgetconf
            mbs3_gfx_setPW4 sk_spr_heart_empty
            mbs3_gfx_sprconf
            inc     b2
            dec     b5
            dec     b6
            j       .maxlife

.blanklife 
            cmp     b6, 0
            jz      .endheart
            mbs3_gfx_setPB1 b2
            mbs3_gfx_sprgetconf
            mbs3_gfx_setPW4 sk_spr_black
            mbs3_gfx_sprconf
            inc     b2
            dec     b6
            j       .blanklife
.endheart   
; show item in bottom bar
            mov     b2, sk_sprid_bot_item
            eor     b3, b3
            mov     w2, $5D9F ; start location at right
            ld      b6, [sk_curr_map]
            eor     b7, b7
            shl     w3
            shl     w3
            shl     w3
            shl     w3
            add     w3, sk_have_item ; got item in map addr
.item            
            mbs3_gfx_setPB1 b2
            mbs3_gfx_sprgetconf
            ld      b0, [w3]
            cmp     b0, 0
            jz      .hideitem
            cmp     b3, 0
            jz      .nextitem
            cmp     b3, 7
            jz      .nextitem
            cmp     b3, 6
            jbe     .keysize
            cmp     b3, 8
            jz      .bootsize
            cmp     b3, 9
            jz      .diamondsize
            ; default size
            sub     w2, $0008
            j       .tilebankindex
.keysize    
            sub     w2, $0006
            j       .tilebankindex
.bootsize
            sub     w2, $0007
            j       .tilebankindex
.diamondsize            
            sub     w2, $0007
.tilebankindex            
            mov     b0, b3
            eor     b1, b1
            shl     w0
            add     w0, sk_item_bankindex
            ld      w0, [w0]
            mbs3_gfx_setPW2 w2
            mbs3_gfx_setPW4 w0
            mbs3_gfx_sprconf
            j       .nextitem
.hideitem
            mbs3_gfx_setPW2 $5DA0
            mbs3_gfx_sprconf
.nextitem
            inc     b2
            inc     w3
            inc     b3
            cmp     b3,10
            jz      .endBottom            
            jump    .item   
            
.endBottom            
            popa
            ret


sk_sprite_anim
            pusha
        ; ****  item animate up/down
            mov     b2, sk_sprid_item ; first item sprite id
            ld      w0, [timer_count]
            tst     w0, $0008
            jz      .item_up
            mov     w3, $0101 ; item_down
            j       .item_move
.item_up
            mov     w3, $0001
.item_move
            mbs3_gfx_setPB1 b2
            mbs3_gfx_sprgetconf
            ld      b0, [lbs3_gfx_reg6]
            tst     b0, $80
            jz      .item_next ; sprite disabled, go next item
            ld      w0, [lbs3_gfx_reg2]
            and     w0, $FEFE
            or      w0, w3
            sr      w0, [lbs3_gfx_reg2]
            mbs3_gfx_sprconf
.item_next
            inc     b2
            cmp     b2, sk_sprid_item_last ; last item sprite+1
            jnz     .item_move

            ; TODO , furnace fire, enemy etc...
            popa
            ret

; return b0: 0: new game, 1: Continue, 2 exit
sk_menu
            ; save env
            pusha
            ; fill surface with menu screenshot
            mov             b0, rom_sk_scr_menu
            call            showromscreenshot
            mbs3_gfx_rendergetconf
            ld              b0, [lbs3_gfx_reg1]
            sr              b0, [.render_layer]
            ld              b0, [lbs3_gfx_reg2]
            sr              b0, [.render_spitez]
            ld              b0, [lbs3_gfx_reg3]
            sr              b0, [.render_defcolor]
            ; render only layer 0 
            mov             b0, 1
            sr              b0, [lbs3_gfx_reg1]
            mov             b0, 0
            sr              b0, [lbs3_gfx_reg2]
            mbs3_gfx_renderconf
            ; 
            mov             w0, sk_menu_new_game
            mov             w1, $3C3E
            mov             b4, 10 ; green
            call            sk_print
            mov             w0, sk_menu_continue
            mov             w1, $463F
            mov             b4, 15 ; white
            call            sk_print
            mov             w0, sk_menu_exit
            mov             w1, $5048
            mov             b4, 12 ; red
            call            sk_print
.showSelector            
            ; side clear left/right hero "selector"
            mbs3_gfx_setPB1 0 ; surface 0
            mbs3_gfx_setPW2 $3A32
            mbs3_gfx_setPB3 0 ; full black box
            mbs3_gfx_setPW4 $1C08    ; size (WxH,16x30)
            mbs3_gfx_boxfull
            mbs3_gfx_setPW2 $3A65
            mbs3_gfx_boxfull
            ; draw hero selector
            mov             w0, $3A32 ; hero top/left coords
            ld              b7, [.menu_result]
            cmp             b7, 1
            jnz             .other1
            add             b1, 10
.other1     cmp             b7, 2
            jnz             .other2
            add             b1, 20
.other2                                         
            mbs3_gfx_setPB1 $81
            mbs3_gfx_setPW2 sk_spr_hero_right1
            mbs3_gfx_setPB3 sk_tile_keycolor
            mbs3_gfx_setPW3 0
            mbs3_gfx_setPB5 0
            mbs3_gfx_setPW6 w0
            push_w0
            mbs3_gfx_blitkcolor ; left hero
            pop_w0
            mbs3_gfx_setPW2 sk_spr_hero_left1
            mov             b0, $65
            mbs3_gfx_setPW6 w0
            mbs3_gfx_blitkcolor ; right hero
            mbs3_gfx_refresh
            ld              b1, [.menu_result]
.getInput
            mbs3_get_input
            cmp             b0, 10
            jz              .quitMenu
            cmp             b0, 'A'
            jz              .moveup
            cmp             b0, 'B'
            jz              .movedown
            j               .getInput
.moveup     
            dec             b1
            cmp             b1, $FF
            jnz             .move
            mov             b1, 2
            j               .move
.movedown
            inc             b1
            cmp             b1, 3
            jnz             .move
            eor             b1, b1
.move
            sr              b1, [.menu_result]
            jump            .showSelector
.quitMenu            
            ; restore env
            ld              b0, [.render_layer]
            sr              b0, [lbs3_gfx_reg1]
            ld              b0, [.render_spitez]
            sr              b0, [lbs3_gfx_reg2]
            ld              b0, [.render_defcolor]
            sr              b0, [lbs3_gfx_reg3]
            mbs3_gfx_renderconf
            popa
            ld              b0, [.menu_result]
            ret
.render_layer       db  0
.render_spitez      db  0
.render_defcolor    db  0
.menu_result        db  0 ; 0:new game, 1:continue, 2:Exit

sk_menu_new_game    db "NEW GAME",0
sk_menu_continue    db "CONTINUE",0
sk_menu_exit        db "EXIT",0


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

; tile map load from sk map
; parameter
;    b0 : sk map number
sk_tilemap
                ; save env
                pusha
                ld                  b1, [sk_curr_map]
                sr                  b1, [sk_prev_map]
                sr                  b0, [sk_curr_map]
                ld                  b1, [lbs3_bank_rom]
                sr                  b1, [.oldrom]
                ; show correct bottom bar
                call                sk_updatebottom
                ; hide map items 
                call                sk_hideMapItems
                ; hide enemy items
                call                sk_hideMapEnemies
                ; select rom map
                add                 b0, rom_sk_map0
                sr                  b0, [lbs3_bank_rom]
                ; select gfx image bank
                mov                 b1, 0 ; tile gfx bank
                ; first tile index address in rom
                mov                 w2, rom_map_layer0_addr
                ; first tilemap coordinates
                mov                 w1, $0000
                ; reset tile maps
                mbs3_gfx_setPB1     0 ; choose tilemap 0
                mbs3_gfx_tmreset      ; reset chosen tilemap
                ; make visible tile map 0
                mbs3_gfx_setPB2     1 ; visible
                mbs3_gfx_tmconfig     ; set chosen tilemap config
                ; common register for all tiles in map
                mbs3_gfx_setPB3     1   ; surface where are tiles
                mbs3_gfx_setPB5     sk_tile_keycolor ; keycolor
                mbs3_gfx_setPB6     $81 ; visible | use keycolor
                ; get tile index
.blit           ld                  b0, [w2] ; get image index
                mov                 b1, 0    ; tile bank 0
                mbs3_gfx_setPW4     w0       ; set tile bank and index
                mbs3_gfx_setPW2     w1       ; tile map coordinates
                mov                 w0, rom_map_layer1_offset
                ld                  b6, [w2 + w0]
                mov                 w0, rom_map_layer2_offset
                ld                  b7, [w2 + w0]
                call                sk_map_meta
                mbs3_gfx_setPW6     w3       ; set tile map meta data
                mbs3_gfx_tmcellconf          ; set tile into tilemap
                inc                 w2 ; next rom map tile address
                ; next tilemap coordinates
                inc                 b2
                cmp                 b2, 20
                jnz                 .blit
                eor                 b2, b2
                inc                 b3
                cmp                 b3, 12
                jnz                 .blit
                ; restore env
                ld                  b1, [.oldrom]
                sr                  b1, [lbs3_bank_rom]
                popa
                ret
.oldrom         db                  0

; meta processing
; input : 
;   w3 = meta : b6 map layer 1, b7 map layer 2
;   w1 = tile coordinates YYXX (YY: 0..19, XX: 0..11)

sk_map_meta
                sr      w3, [.savedmeta]
                sr      w1, [.savedcoord]
                pusha
                
                cmp     b6, sk_object_item
                jz      .item
                cmp     b6, sk_object_enemy
                jz      .enemy
                cmp     b6, sk_object_emitter
                jz      .emitter
                cmp     b6, sk_object_tomap
                jz      .tomap
                j       .quitroutine
.item           call    sk_showMapItem
                j       .quitroutine
.enemy          call    sk_showEnemy
                j       .quitroutine
.emitter        call    sk_showEmitter
                j       .quitroutine
.tomap          call    sk_tomaphero                
.quitroutine    popa
                ret
.savedmeta      dw      0
.savedcoord     dw      0

; hide all items on map
sk_hideMapItems
            pusha
            mov     w1, sk_sprid_item
.nextItem   mbs3_gfx_setPB1 b2
            mbs3_gfx_sprgetconf
            mbs3_gfx_setPB6 01
            mbs3_gfx_sprconf
            inc     b2
            cmp     b2, sk_sprid_item_last
            jnz     .nextItem
            popa
            ret
; hide all enemies on map
sk_hideMapEnemies
            pusha
            mov     b2, sk_sprid_enemie
            eor     b3, b3
.loopenemy            
            mbs3_gfx_setPB1 b2 
            mbs3_gfx_sprgetconf
            ld      b4, [lbs3_gfx_reg6]
            tst     b4, $80 ; is it enabled
            jz      .nextenemy ; if already disable, next enemy
            and     b4, $7F ; disable sprite
            mbs3_gfx_setPB6 b4
            mbs3_gfx_sprconf
.nextenemy            
            inc     b2
            inc     b3
            cmp     b3, $20
            jnz     .loopenemy
            popa
            ret

; hero position ? at map load
; input:
;   b7 : map destination
;   w1 : hero position? in tile coords  YYXX (YY: 0..19, XX: 0..11)
sk_tomaphero
            pusha
            ld      b0, [sk_prev_map]
            cmp     b0, b7
            jz      .setherolocation
            jump    .quitroutine
.setherolocation            
            mbs3_gfx_pushparameters
            mbs3_gfx_setPB1 sk_sprid_hero ; get hero info
            mbs3_gfx_sprgetconf
            shl     w1
            shl     w1
            shl     w1
            mbs3_gfx_setPW2 w1
            mbs3_gfx_sprconf
            mbs3_gfx_popparameters
.quitroutine            
            popa
            ret

; set GFX PB1 with a free sprite id for enemy
; set GFX PB1 with $FF is no free sprite id
sk_enemy_getfreeesprid
            pusha
            mov     b2, sk_sprid_enemie
            eor     b3, b3
.nextSprite            
            mbs3_gfx_setPB1 b2
            mbs3_gfx_sprgetconf
            ld      b6, [lbs3_gfx_reg6]
            tst     b6, $80
            jz      .found
            inc     b2
            inc     b3
            cmp     b3, $20
            jnz     .nextSprite
            mbs3_gfx_setPB1 $FF
.found            
            popa
            ret

; add Enemy 
; input:
;   b7 : Enemy 
;   w1 = Enemy in tile coordinates YYXX (YY: 0..19, XX: 0..11)
sk_showEnemy
            pusha
            mbs3_gfx_pushparameters
            ; get free sprite id 
            call    sk_enemy_getfreeesprid
            ld      b6, [lbs3_gfx_reg1]
            cmp     b6, $FF
            jnz     .spawn 
            jump    .cantspawn ; no free sprite id
.spawn            
            ; prepare the sprite info PB1 is already set
            ;  set coordinates
            and     w1, $1F1F
            shl     w1
            shl     w1
            shl     w1
            mbs3_gfx_setPW2 w1
            ;  set tile surface source
            mbs3_gfx_setPB3 1
            ; set tile bank/index
            cmp     b7, sk_object_enemy_bat
            jnz     .skeleton
            mbs3_gfx_setPW4 sk_spr_bat_sleep
            j       .cont
.skeleton
            cmp     b7, sk_object_enemy_skeleton
            jnz     .redknight 
            mbs3_gfx_setPW4 sk_spr_skelguard_right1
            j       .cont
.redknight
            cmp     b7, sk_object_enemy_redknight
            jnz     .cantspawn
            mbs3_gfx_setPW4 sk_spr_guard_right1                
.cont
            ;  set keycolor
            mbs3_gfx_setPB5 sk_tile_keycolor
            ; enabled|z=1|collidable|keycolor
            mbs3_gfx_setPB6 $A9
            ; metadata1
            mov     b6, b7
            eor     b7, b7
            mbs3_gfx_setPW6 w3 ; $00xx (xx = enemy type id)
            ; metadata2
            mbs3_gfx_setPW7 $0000
            ; set sprite config
            mbs3_gfx_sprconf
.cantspawn            
            mbs3_gfx_popparameters
            popa
            ret

; add  Emitter  if not already in gotten
; input:
;   b7 : emiter type from 0 to 3 (right, left, flame and skull)
;   w1 = item in tile coordinates YYXX (YY: 0..19, XX: 0..11)
sk_showEmitter
            ret

; add item if not already in gotten
; input:
;   b7 : item 
;   w1 = item in tile coordinates YYXX (YY: 0..19, XX: 0..11)
sk_showMapItem
            pusha
            ; prepare local variable
            sr      b7, [.itemId]
            sr      w1, [.itemTileCoords]
            mbs3_gfx_pushparameters
            ; test if the item is already gotten
            ; compute item inventory address
            mov     w0, sk_have_item ; base item inventory
            ld      b2, [sk_curr_map]
            and     w1, $000F
            shl     w1
            shl     w1
            shl     w1
            shl     w1
            ; 16 inventory status per map
            add     w0, w1 ; base item inv of current map
            push    b7
            pop     w1
            add     w0, w1 ; item inv addr in current map
            ld      b2, [w0] ; get if it is in inventory
            cmp     b2, 0
            jnz     .quitmapitem ;(in inventory then do not show up)

            ; show item [.itemId] at location map [.itemTileCoords]
            ld      b0, [.itemId]
            add     b0, sk_sprid_item   ; sprite id 
            mbs3_gfx_setPB1 b0          ; sprite b0
            ld      w0, [.itemTileCoords]
            ; convert tile to pixel Coordinates
            and     w0, $1F1F
            shl     w0
            shl     w0
            shl     w0
            mbs3_gfx_setPW2 w0          ; pixel coordinates
            mbs3_gfx_setPB3 1           ; tile surface
            mov     w0, sk_item_bankindex
            ld      b2, [.itemId]
            eor     b3, b3
            shl     w1
            ld      w0, [w0 + w1]
            mbs3_gfx_setPW4 w0                  ; set index/bank
            mbs3_gfx_setPB5 sk_tile_keycolor    ; keycolor
            mbs3_gfx_setPB6 $A9                 ; enabled|z=1|coll|kc
            mbs3_gfx_sprconf                    ; sprite config
.quitmapitem    
            mbs3_gfx_popparameters                 
            popa
            ret
.itemId         db      0
.itemTileCoords dw      0



; blit a 8x8 gfx image bank
; parameters (registers)
;   b0 : gfx image index
;   b1 : gfx image bank
;   w1 : target coordinates on surface 0
blit8x8frombank
                pusha
                mbs3_gfx_setPB1     $81     ; source surface
                mbs3_gfx_setPW2     w0      ; source coordinates
                mbs3_gfx_setPB3     sk_tile_keycolor
                mbs3_gfx_setPB5     0       ; target surface
                mbs3_gfx_setPW6     w1      ; target coordinates
                mbs3_gfx_blitkcolor
                popa
                ret


; transfer rom sk tiles and sprite to gfx surface
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
                ; transfer fonts
                mov                 b0, rom_sk_fonts_data
                mov                 b1, rom_sk_fonts_mask
                mov                 b4, 2 ; font gfx image bank
                mov                 b2, 0 ; rom img idx 
                mov                 b3, 0 ; gfx img idx 
.transOneGlyph  c                   trans8x8toGFX
                add                 w1, $0101
                cmp                 b2, 128
                jnz                 .transOneGlyph                
                ret


; transfer  to GFX surface number 1
; one 8x8 image with "mask to key color" transform
; to GFX surface number 1
; parameters
;      b0 : rom bank number for data
;      b1 : rom bank number for mask
;      b2 : rom 8x8 image index (0 to 255)
;      b3 : GFX 8x8 image index in image bank
;      b4 : GFX 8x8 image bank number (0: tile, 1: sprite, ... 3)
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
                ld                  b1, [.gfximgbank]
                ld                  b0, [.gfximgidx]
                ;   prepare the mbs3_gfx_blitrans
                mbs3_gfx_setPB1     $81
                mbs3_gfx_setPW2     w0
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
                ; first pixel : b3 4bits mask, b4 4bits data
                tst                 b3, $F0 ; 1st pixel mask
                jz                  .plot1st
                cmp                 b4, 0
                jnz                 .plot1st
                mov                 b4, sk_tile_keycolor
                ; transfer first pixel
.plot1st        mbs3_gfx_setPB3     b4
                call                lbs3_gfx_busywait
                ; b2 = 2nd pixel
                and                 b2, $0F
                tst                 b3, $0F ; 2nd pixel mask
                jz                  .plot2nd
                cmp                 b2, 0
                jnz                 .plot2nd
                mov                 b2, sk_tile_keycolor
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

; print a text in color 
; w0 : address of a null terminated string to print
; w1 : coordinates of the text
;      $YYXX : YY MSB y coordinate
;              XX LSB x coordinate
; b4 : color of the text
sk_print 
                ; save env
                pusha
                mbs3_gfx_pushparameters
                ld      b5, [lbs3_bank_rom]
                sr      b5, [.oldrom]
                ; set rom to font glyph width
                mov     b5, rom_sk_fonts_width
                sr      b5, [lbs3_bank_rom]
                ; save parameters
                sr      w0, [.textaddr]
                sr      w1, [.coordinates]
                sr      b4, [.color]
                ; for each character in text string
                mov     w3, w0 ; w3 = address text
.nextchar
                ld      b5, [w3]
                cmp     b5, 0
                jz      .endprint ; finish if end of string
                ; print character at b5=[w3] with correct color
                ; b4 color, w1 coordinates, b5 character
                mbs3_gfx_setPB1     $81 ; tile surface 1
                mov     b1, 2 ; tile bank 2 (font)
                mov     b0, b5; tile index (mapped to character)
                mbs3_gfx_setPW2     w0 ; tile bank/index
                mbs3_gfx_setPB3     sk_tile_keycolor
                mov     b1, $0F ; color find 
                mov     b0, b4  ; color replace (text color)
                mbs3_gfx_setPW3     w0 ; colorize
                mbs3_gfx_setPB5     0 ; target surface
                mbs3_gfx_setPW6     w1 ; target coordinates
                mbs3_gfx_blitkcolor
                ; move coordinates with the character width
                ;   w0= address in glyph width rom
                eor     w0, w0
                mov     b0, b5
                add     w0, $E000
                ;   w0= glyph width
                ld      b0, [w0]
                eor     b1, b1
                ; move coordinates to the right by glyph width
                add     w1, w0 
            
                ; got to next character address
                inc     w3 ; next character address
                jump    .nextchar ; process next character
.endprint                
                ; restore env
                ld      b5, [.oldrom]
                sr      b5, [lbs3_bank_rom]
                mbs3_gfx_popparameters
                popa
                ret
.textaddr       dw      0
.coordinates    dw      0
.color          db      0
.oldrom         db      0

; sk data

sk_prev_map     db                  $FF
sk_curr_map     db                  $FF
sk_max_life     db                  3
sk_curr_life    db                  3
sk_tick         db                  0

                align               16
sk_have_item   ; 0 : not yet in posession  != 0 gotten item
                db                  0 ; item 0, health ?
                db                  1 ; item 1, silver key
                db                  1 ; item 2, green key
                db                  0 ; item 3, red key
                db                  0 ; item 4, blue key
                db                  0 ; item 5, purple key
                db                  0 ; item 6, gold key
                db                  0 ; item 7, health plus
                db                  1 ; item 8, boots
                db                  1 ; item 9, diamond
                align               16
                ; max 16 maps of 16 items
                ; if a item is unique in game, then if collected in A
                ; map then it must be "as collected" in other map
                space               256 ; each map has its inventory
sk_item_bankindex
                dw                  sk_spr_heart_yellow ; item 0 health
                dw                  sk_spr_key_gray; item 1 silver key
                dw                  sk_spr_key_green; item 2 green key
                dw                  sk_spr_key_red; item 3 red key
                dw                  sk_spr_key_blue; item 4 blue key
                dw                  sk_spr_key_purple; i. 5 purple key
                dw                  sk_spr_key_yellow; item 6 gold key
                dw                  sk_spr_heart_yellow ;item 7 health+
                dw                  sk_spr_boots ; item 8 boots
                dw                  sk_spr_book ; item 9 diamond
                align               16
; end of sk.asm