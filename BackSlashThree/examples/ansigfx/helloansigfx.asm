#!./bs3asm -R
; Hello from ANSI GFX
            include         "bs3core.inc"    ; Utilities
            include         "bs3ansi.inc"    ; Basic ANSI print
            include         "bs3ansigfx.inc" ; ANSI gfx lib            

            mbs3_bootat     start

start       ansi_ED         2               ; clear screen
            ansi_HCU                        ; hide cursor
            call            lbs3gfxinit     ; init ANSI gfx

; diagonal pixel line  
            mov             b0, 17          ; 
            call            lbs3gfxclear    ; clear screen in blue
            call            lbs3gfxshow     ; show screen result

;    50 pixels diagonal            
            mov             b0, 50
.pixelloop  push            b0              ; save b0 for loop dec
            cmp             b0, $FF
            jz              .endpixelloop
            push            b0              ; push parameter x=b0
            dup                             ; push parameter y=b0
            pushi           14              ; push parameter color=14
            call            lbs3gfxpixel    ; call set pixel routine
            drop3                           ; drop parameters
            call            lbs3gfxshow     ; show screen result
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
            ansi_SGR_reset                  ; reset ANSI terminal
            ansi_ED         2               ; clear screen
            ansi_CUP        1, 1            ; set cursor to top left
            mbs3_printlnstr "bye."
            ansi_SCU                        ; show cursor
            mbs3_putc       10

            hlt
hellomsg    db "Hello world    BackSlashThree",0
image       db 5,5 ; 5 pixels width, 5 pixels height
            db $0F, $0F, $0F, $0F, $0F
            db $0F, $00, $00, $00, $0F
            db $0F, $00, $03, $00, $0F
            db $0F, $00, $00, $00, $0F
            db $0F, $0F, $0F, $0F, $0F

