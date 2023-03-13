;   wait for gfx is ready to accept 
;   no register modified, except flag registers
mbs3_gfx_busywait macro
                push    b0
                call    lbs3_gfx_busywait
                pop     b0
                  endm

;  execute a BS3 GFX command
;  after execution register B0 contains the status code of the command
;   expect 1 parameter : an immediate with the value of the command
;   possible command values are defined by equ names ebs3_gfx_cmd_*
mbs3_gfx_command        macro
                call    lbs3_gfx_busywait
                mov     b0, {1}
                sr      b0, [lbs3_gfx_cmd]
                call    lbs3_gfx_busywait
                ld      b0, [lbs3_gfx_status]
                and     b0, ebs3_gfx_status_code
                endm

;  set BS3 GFX Byte register
;  expect 2 parameters
;     1) an immdiate or label to registry addr
;     2) immediate byte or register Bx
mbs3_gfx_setreg8 macro
                push    b0
                mov     b0, {2}
                sr      b0, [{1}] 
                pop     b0
                endm

; set BS3 GFX PB1 register
;  expect 1 parameter : immediate byte or register Bx
mbs3_gfx_setPB1 macro
                mbs3_gfx_setreg8 lbs3_gfx_reg1, {1}
                endm

; set BS3 GFX PB2 register
;  expect 1 parameter : immediate byte or register Bx
mbs3_gfx_setPB2 macro
                mbs3_gfx_setreg8 lbs3_gfx_reg2, {1}
                endm

; set BS3 GFX PB3 register
;  expect 1 parameter : immediate byte or register Bx
mbs3_gfx_setPB3 macro
                mbs3_gfx_setreg8 lbs3_gfx_reg3, {1}
                endm

; set BS3 GFX PB4 register
;  expect 1 parameter : immediate byte or register Bx
mbs3_gfx_setPB4 macro
                mbs3_gfx_setreg8 lbs3_gfx_reg4, {1}
                endm

; set BS3 GFX PB5 register
;  expect 1 parameter : immediate byte or register Bx
mbs3_gfx_setPB5 macro
                mbs3_gfx_setreg8 lbs3_gfx_reg5, {1}
                endm

; set BS3 GFX PB6 register
;  expect 1 parameter : immediate byte or register Bx
mbs3_gfx_setPB6 macro
                mbs3_gfx_setreg8 lbs3_gfx_reg6, {1}
                endm

; set BS3 GFX PB7 register
;  expect 1 parameter : immediate byte or register Bx
mbs3_gfx_setPB7 macro
                mbs3_gfx_setreg8 lbs3_gfx_reg7, {1}
                endm


;  set BS3 GFX Word register
;  expect 2 parameters
;     1) an immdiate or label to registry addr
;     2) immediate byte or register Wx
mbs3_gfx_setreg16 macro
                push    w0
                mov     w0, {2}
                sr      w0, [{1}] 
                pop     w0
                endm

; set BS3 GFX PW1 register
;  expect 1 parameter : immediate word or register Wx
mbs3_gfx_setPW1 macro
                mbs3_gfx_setreg16 lbs3_gfx_reg1, {1}
                endm

; set BS3 GFX PW2 register
;  expect 1 parameter : immediate word or register Wx
mbs3_gfx_setPW2 macro
                mbs3_gfx_setreg16 lbs3_gfx_reg2, {1}
                endm

; set BS3 GFX PW3 register
;  expect 1 parameter : immediate word or register Wx
mbs3_gfx_setPW3 macro
                mbs3_gfx_setreg16 lbs3_gfx_reg3, {1}
                endm

; set BS3 GFX PW4 register
;  expect 1 parameter : immediate word or register Wx
mbs3_gfx_setPW4 macro
                mbs3_gfx_setreg16 lbs3_gfx_reg4, {1}
                endm

; set BS3 GFX PW5 register
;  expect 1 parameter : immediate word or register Wx
mbs3_gfx_setPW5 macro
                mbs3_gfx_setreg16 lbs3_gfx_reg5, {1}
                endm

; set BS3 GFX PW6 register
;  expect 1 parameter : immediate word or register Wx
mbs3_gfx_setPW6 macro
                mbs3_gfx_setreg16 lbs3_gfx_reg6, {1}
                endm

; set BS3 GFX PW7 register
;  expect 1 parameter : immediate word or register Wx
mbs3_gfx_setPW7 macro
                mbs3_gfx_setreg16 lbs3_gfx_reg7, {1}
                endm

;  reset BS3 GFX device
;  at return B0 has the status code
mbs3_gfx_reset  macro
                mbs3_gfx_command  ebs3_gfx_cmd_reset      
                endm

;  end BS3 GFX device
;  at return B0 has the status code
mbs3_gfx_end    macro
                mbs3_gfx_command  ebs3_gfx_cmd_end      
                endm

; Refresh BS3 GFX Viewport
; at return B0 has the status code
mbs3_gfx_refresh macro
                mbs3_gfx_command  ebs3_gfx_cmd_refresh
                endm

; Clear BS3 GFX Viewport
; at return B0 has the status code
mbs3_gfx_vpclear macro
                mbs3_gfx_command  ebs3_gfx_cmd_vpclear
                endm

; Config BS3 GFX Viewport
; at return B0 has the status code
mbs3_gfx_vpconfig macro
                mbs3_gfx_command  ebs3_gfx_cmd_vpconfig
                endm

; Get config BS3 GFX Viewport
; at return B0 has the status code
mbs3_gfx_vpgetconf macro
                mbs3_gfx_command  ebs3_gfx_cmd_vpgetconf
                endm

; Get pixel on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_getpixel macro
                mbs3_gfx_command  ebs3_gfx_cmd_getpixel
                endm

; Set pixel on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_setpixel macro
                mbs3_gfx_command  ebs3_gfx_cmd_setpixel
                endm

; Horizontal line on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_hline macro
                mbs3_gfx_command  ebs3_gfx_cmd_hline
                endm

; Vertical line on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_vline macro
                mbs3_gfx_command  ebs3_gfx_cmd_vline
                endm

; Box on BS3 GFX Surface 
; at return B0 has the status code
mbs3_gfx_box macro
                mbs3_gfx_command  ebs3_gfx_cmd_box
                endm

; Full box on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_boxfull macro
                mbs3_gfx_command  ebs3_gfx_cmd_boxfull
                endm

; Blit with operator on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_blitope macro
                mbs3_gfx_command  ebs3_gfx_cmd_blitope
                endm

; Blit with key color on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_blitkcolor macro
                mbs3_gfx_command  ebs3_gfx_cmd_blitkcolor
                endm

; Blit with key color on BS3 GFX Surface
; at return B0 has the status code
mbs3_gfx_blitrans macro
                mbs3_gfx_command  ebs3_gfx_cmd_blitrans
                endm

; Configure a tile bank (set size)
; at return B0 has the status code
mbs3_gfx_tbconf macro
                mbs3_gfx_command  ebs3_gfx_cmd_tbconfig
                endm

; Get configuration of a tile bank (get size)
; at return B0 has the status code
mbs3_gfx_tbgetconf macro
                mbs3_gfx_command  ebs3_gfx_cmd_tbgetconf
                endm

; Tile Map reset (all cells and tile map iteself not visible)
; at return B0 has the status code
mbs3_gfx_tmreset  macro
                mbs3_gfx_command  ebs3_gfx_cmd_tmreset
                endm

; Tile Map config (set visible/invisible the tile map)
; at return B0 has the status code
mbs3_gfx_tmconfig macro
                mbs3_gfx_command  ebs3_gfx_cmd_tmconfig
                endm

; Tile Map cell config
; at return B0 has the status code
mbs3_gfx_tmcellconf macro
                mbs3_gfx_command  ebs3_gfx_cmd_tmcellconf
                endm

; Tile Map cell get config
; at return B0 has the status code
mbs3_gfx_tmcellgetconf macro
                mbs3_gfx_command  ebs3_gfx_cmd_tmcellgetconf
                endm

; Sprite reset (disable all sprites)
; at return B0 has the status code
mbs3_gfx_sprreset macro
                mbs3_gfx_command  ebs3_gfx_cmd_sprreset
                endm

; Sprite config
; at return B0 has the status code
mbs3_gfx_sprconf macro
                mbs3_gfx_command  ebs3_gfx_cmd_sprconfig
                endm

; Sprite get config
; at return B0 has the status code
mbs3_gfx_sprgetconf macro
                mbs3_gfx_command  ebs3_gfx_cmd_sprgetconf
                endm

; Sprite get collision count 
; at return B0 has the status code
mbs3_gfx_sprcollcount  macro
                mbs3_gfx_command  ebs3_gfx_cmd_sprcollcnt
                endm

; Sprite get collision
; at return B0 has the status code
mbs3_gfx_sprgetcoll macro
                mbs3_gfx_command  ebs3_gfx_cmd_sprgetcoll
                endm


; Enable auto refresh of the BS3 GFX device
mbs3_gfx_autorefresh_on macro
                push    b0
                mbs3_gfx_busywait
                mov     b0, 1
                sr      b0, [lbs3_gfx_autorefresh]
                pop     b0

; Disable auto refresh of the BS3 GFX device
mbs3_gfx_autorefresh_off macro
                push    b0
                mbs3_gfx_busywait
                mov     b0, 0
                sr      b0, [lbs3_gfx_autorefresh]
                pop     b0
                        endm                 


; push all gfx parameters to stack
mbs3_gfx_pushparameters macro
                sr    w0, [.savedw0]
                ; push word paramters
                ld    w0, [lbs3_gfx_reg1]
                push_w0
                ld    w0, [lbs3_gfx_reg2]
                push_w0
                ld    w0, [lbs3_gfx_reg3]
                push_w0
                ld    w0, [lbs3_gfx_reg4]
                push_w0
                ld    w0, [lbs3_gfx_reg5]
                push_w0
                ld    w0, [lbs3_gfx_reg6]
                push_w0
                ld    w0, [lbs3_gfx_reg7]
                push_w0
                ; push byte parameters
                ld    b0, [lbs3_gfx_reg1]
                push_b0
                ld    b0, [lbs3_gfx_reg2]
                push_b0
                ld    b0, [lbs3_gfx_reg3]
                push_b0
                ld    b0, [lbs3_gfx_reg4]
                push_b0
                ld    b0, [lbs3_gfx_reg5]
                push_b0
                ld    b0, [lbs3_gfx_reg6]
                push_b0
                ld    b0, [lbs3_gfx_reg7]
                push_b0
                ld    w0, [.savedw0]
                j     .endmacro
.savedw0        dw    0
.endmacro                
                endm

; pop all gfx parameters from stack
mbs3_gfx_popparameters macro
                sr    w0, [.savedw0]
                ; pop byte parameters
                pop_b0  
                sr    b0, [lbs3_gfx_reg7]
                pop_b0  
                sr    b0, [lbs3_gfx_reg6]
                pop_b0  
                sr    b0, [lbs3_gfx_reg5]
                pop_b0  
                sr    b0, [lbs3_gfx_reg4]
                pop_b0  
                sr    b0, [lbs3_gfx_reg3]
                pop_b0  
                sr    b0, [lbs3_gfx_reg2]
                pop_b0  
                sr    b0, [lbs3_gfx_reg1]

                ; pop word paramters
                pop_w0
                sr    w0, [lbs3_gfx_reg7]
                pop_w0
                sr    w0, [lbs3_gfx_reg6]
                pop_w0
                sr    w0, [lbs3_gfx_reg5]
                pop_w0
                sr    w0, [lbs3_gfx_reg4]
                pop_w0
                sr    w0, [lbs3_gfx_reg3]
                pop_w0
                sr    w0, [lbs3_gfx_reg2]
                pop_w0
                sr    w0, [lbs3_gfx_reg1]

                ld    w0, [.savedw0]
                j     .endmacro
.savedw0        dw    0
.endmacro                
                endm



lbs3_gfx_busywait
.retry          ld      b0, [lbs3_gfx_status]
                tst     b0, ebs3_gfx_status_busy
                jnz     .retry
                ret

;   Bitmap transfert 
;   serial transfer of a bitmap to BS3 GFX, to a surface at coords
;   expect 4 parameters in stack
;     1) SP + 8 : surface number                     (first  push)
;     2) SP + 6 : bitmap size, MSB:height, LSB:width (second push)
;     3) SP + 4 : bitmap source addr                 (third  push)
;     4) SP + 2 : target coordinates, MSB:y, LSB:x   (fourth push)
; all registers Bx,Wy modified

ebs3gfx_btrans_surface  equ     8
ebs3gfx_btrans_size     equ     6
ebs3gfx_btrans_addrsrc  equ     4
ebs3gfx_btrans_coord    equ     2

lbs3_gfx_blitrans
                ld                  b0, [sp + ebs3gfx_btrans_surface]
                ld                  w1, [sp + ebs3gfx_btrans_coord]
                ld                  w2, [sp + ebs3gfx_btrans_size]
                mbs3_gfx_setPB1     b0  ; set surface
                mbs3_gfx_setPW2     w1  ; set coordinates
                mbs3_gfx_setPW4     w2  ; set size
                mbs3_gfx_blitrans
                cmp                 b0, ebs3_gfx_code_ok ; is it ok ?
                jnz                 .endblitrans ; not ok then quit
                ld                  w3, [sp + ebs3gfx_btrans_addrsrc]
.transferloop                
                ld                  b0, [lbs3_gfx_status]
                tst                 b0, ebs3_gfx_status_waitdata
                jz                  .endblitrans ; end of transfer
                ld                  b0, [w3]
                mbs3_gfx_setPB3     b0
                c                   lbs3_gfx_busywait
                inc                 w3
                j                   .transferloop
.endblitrans
                ret
