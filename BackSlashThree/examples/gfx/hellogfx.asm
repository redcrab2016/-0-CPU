; Hello from BS3 GFX device
            include "bs3core.inc"

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
            pushi5          25, 25, 75, 75, 10 ; xmin,ymin,xmax,ymax,c
            call            lbs3gfxbox
            drop5

; Blue full box
            pushi5          26, 26, 74, 74, 12 ; xmin,ymin,xmax,ymax,c
            call            lbs3gfxfullbox
            drop5

; Print message shadow
            pushi4          hellomsg, 30, 46, 8 ; msg,x,y,color
            call            lbs3gfxdrawstr
            drop4

; Print message over shadow
            pushi4          hellomsg, 29, 45, 15 ; msg,x,y,color
            call            lbs3gfxdrawstr
            drop4

; blit a bitmap with 'set' operator
            ; Set : prepare parameter for image blit
            pushi4          110, 25, image, ebs3gfx_blit_op_set
            call            lbs3gfxblit
            drop4

; blit a bitmap with 'eor' exclusive or operator
            pushi4          110, 32, image, ebs3gfx_blit_op_eor
            call            lbs3gfxblit
            drop4

            call            lbs3gfxshow     ; show screen result

; wait for input (e.g keyboard)
.loop2      in              b0
            jz              .loop2
            mbs3_gfx_end                ; Leave graphical 
            mbs3_printlnstr "bye."

            hlt
hellomsg    db "Hello world    BackSlashThree",0
image       db 5,5 ; 5 pixels width, 5 pixels height
            db $0F, $0F, $0F, $0F, $0F
            db $0F, $00, $00, $00, $0F
            db $0F, $00, $03, $00, $0F
            db $0F, $00, $00, $00, $0F
            db $0F, $0F, $0F, $0F, $0F

