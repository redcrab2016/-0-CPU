; Hello from BS3 GFX device
            include "bs3core.inc"
            include "bs3gfx.inc"

            mbs3_bootat     start

start       mbs3_gfx_reset                  ; Reset GFX device

; diagonal pixel line 
            mbs3_gfx_setPB1 17  ; view port clear with color PB1=blue 
            mbs3_gfx_vpclear    ; clear view port
            mbs3_gfx_refresh    ; show screen result

;    50 pixels diagonal            
            mbs3_gfx_setPB1 0   ; surface 0
            mbs3_gfx_setPB3 14  ; color 14 in IBM PC palette (yellow)
            mov             b0, 50
.pixelloop  push            b0              ; save b0 for loop dec
            cmp             b0, $FF
            jz              .endpixelloop
            mov             b1, b0
            mbs3_gfx_setPW2 w0  ; coordinate b1 = y b0 = x
            mbs3_gfx_setpixel   ; draw pixel on surface
            mbs3_gfx_refresh    ; show screen result
            pop             b0
            dec             b0              ; decrement b0 for coords.
            j               .pixelloop
.endpixelloop

; Green box
            mov             b2, 25  ; x = 25 w1 LSB
            mov             b3, 25  ; y = 25 w1 MSB
            mov             b4, 51  ; w = 51 w2 LSB
            mov             b5, 51  ; h = 51 w2 MSB
            mbs3_gfx_setPB1 0       ; surface 0
            mbs3_gfx_setPW2 w1      ; top left coordinates (25, 25)
            mbs3_gfx_setPB3 10      ; color 10 (green)
            mbs3_gfx_setPW4 w2      ; box size  (51, 51)
            mbs3_gfx_box            ; draw box

; Blue full box
            add             w1, $0101 ; x = 26, y = 26
            sub             w2, $0202 ; w = 49, h = 49
            mbs3_gfx_setPW2 w1      ; top left coordinates (26, 26)
            mbs3_gfx_setPB3 12      ; color 12 
            mbs3_gfx_setPW4 w2      ; box size  (49, 49)
            mbs3_gfx_boxfull        ; draw box

; Print message shadow
;            pushi4          hellomsg, 30, 46, 8 ; msg,x,y,color
;            call            lbs3gfxdrawstr
;            drop4

; Print message over shadow
;            pushi4          hellomsg, 29, 45, 15 ; msg,x,y,color
;            call            lbs3gfxdrawstr
;            drop4

; transfer a bitmap to BS3 GFX device
            pushi           1              ; target on second surface
            ld              w0, [image]
            push            w0             ; bitmap size (16bits HHWW)
            pushi           imgdata        ; bitmap data address
            pushi           $0000          ; top|left surface coords
            call            lbs3_gfx_blitrans
            drop4

; blit a bitmap with 'set/copy' operator
            ; Set : prepare parameter for image blit
            mbs3_gfx_setPB1 1              ; source surface (second)
            mbs3_gfx_setPW2 $0000          ; source coords (top/left)
            mbs3_gfx_setPB3 ebs3_gfx_blit_ope_copy ; copy bitmap
            mbs3_gfx_setPW4 $0505          ; size 5 height,5 width
            mbs3_gfx_setPB5 0              ; target surface (first)
            mov             b0, 110        ; x = 110
            mov             b1, 25         ; y = 25, w0=coordinates
            mbs3_gfx_setPW6 w0
            mbs3_gfx_blitope

; blit a bitmap with 'eor' exclusive or operator
            mbs3_gfx_setPB3 ebs3_gfx_blit_ope_eor ;exclusiv or bitmap
            mov             b0, 110        ; x = 110
            mov             b1, 32         ; y = 32, w0=coordinates
            mbs3_gfx_setPW6 w0
            mbs3_gfx_blitope

            mbs3_gfx_refresh                ; show screen result

; wait for input (e.g keyboard)
.loop2      in              b0
            jz              .loop2
            mbs3_gfx_end                ; Leave graphical 
            mbs3_printlnstr "bye."

            hlt
;hellomsg    db "Hello world    BackSlashThree",0
image       db 5,5 ; 5 pixels width, 5 pixels height
imgdata     db $0F, $0F, $0F, $0F, $0F
            db $0F, $00, $00, $00, $0F
            db $0F, $00, $03, $00, $0F
            db $0F, $00, $00, $00, $0F
            db $0F, $0F, $0F, $0F, $0F

