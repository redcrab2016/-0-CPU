; Graphic mode with ANSI 
; using an half upper block character (unicode $2580)
; to define two verical square pixels, 
; using foreground color for top pixel
; using background color for bottom pixel
; e.g a 160 x 50 character define a pixel buffer of 160 x 100 pixels

ebs3screen      equ     $8000
ebs3shadow      equ     $C000

ebs3gfxwidth    equ     160     ; GFX screen width
ebs3gfxheight   equ     100     ; GFX screen height

; below EQU , are manualy set, based on width and height
ebs3gfxwidth2   equ     320     ; two times GFX screen width
ebs3gfxhchar    equ     50      ; half of GFX screen height
ebs3gfxlength   equ     16000   ; GFX memory size(bytes)Width x Height

    jump .endofansigfx  ; include jump over

lbs3gfxshow     
; scridx = 0
; lastfc = 255
; lastbc = 255
; curow = 0
; cucol = 0
; row = 0
; col = 0
;
; for row = 1 to ebs3gfxhchar
;   for col = 1 to ebs3gfxwidth
;       if word ebs3screen[scridx] != word ebs3shadow[scridx] then
;           word ebs3shadow[scridx] = word ebs3screen[scridx]
;           if col != cucol and row != curow then
;               ansi cursorat row, col
;           else 
;               if col != cucol then
;                   ansi curhorizontal col
;               else
;                   if row != curow then
;                       if row - curow = 1 then
;                           ansi curdown
;                       else
;                           ansi curdown (row - curow)
;                   endif
;               endif
;           endif
;           if lastfc != byte ebs3screen[scridx] then
;               ansi forecolor byte ebs3screen[scridx]
;               lastfc = byte ebs3screen[scridx]
;           endif
;           if lastbc != byte ebs3screen[scridx + 1] then
;               ansi backcolor byte ebs3screen[scridx + 1]
;               lastbc = byte ebs3screen[scridx + 1]
;           endif
;           print upper half block
;           cucol = col +1
;           curow = row      
;       endif
;       scridx += 2
;   next col
; next row 
; here below the code with the above meta code in comment      
            eor             w0, w0
            sr              w0, [scridx]
            sr              b0, [curow]
            sr              b0, [cucol]
            dec             b0
            sr              b0, [lastfc]
            sr              b0, [lastbc]
; for row = 1 to ebs3gfxhchar            
  startfwbloop    row, 1, ebs3gfxhchar

;   for col = 1 to ebs3gfxwidth
    startfwbloop    col, 1, ebs3gfxwidth

;       if word ebs3screen[scridx] != word ebs3shadow[scridx] then
        ld              w2, [scridx]
        mov             w0, ebs3screen
        ld              w0, [w0 + w2]
        mov             w1, ebs3shadow
        ld              w1, [w1 + w2]
        cmp             w0, w1
        jnz             .l0
        jump            .continue
.l0

;           word ebs3shadow[scridx] = word ebs3screen[scridx]
            mov             w1, ebs3shadow
            sr              w0, [w1 + w2]

;           if col != cucol and row != curow then
            ld              b0, [col]
            ld              b1, [cucol]
            cmp             b0, b1
            jz              .l1
            ld              b0, [row]
            ld              b1, [curow]
            cmp             b0, b1
            jz              .l1

;               ansi cursorat row, col
                mbs3_putc       27
                mbs3_putc       '['
                mov             w0, lbs3gfxstr256
                ld              b2, [row]
                eor             b3, b3
                mul             w1, 4, w3
                add             w1, w0
                mbs3_print_     w1
                mbs3_putc       ';'
                mov             w0, lbs3gfxstr256
                ld              b2, [col]
                eor             b3, b3
                mul             w1, 4, w3
                add             w1, w0
                mbs3_print      w1
                mbs3_putc       'H'
                j               .l2

;           else
.l1

;               if col != cucol then
                ld              b0, [col]
                ld              b1, [cucol]
                cmp             b0, b1
                jz              .l3
;                   ansi curhorizontal col
                    mbs3_putc       27
                    mbs3_putc       '['
                    mov             w0, lbs3gfxstr256
                    ld              b2, [col]
                    eor             b3, b3
                    mul             w1, 4, w3
                    add             w1, w0
                    mbs3_print_     w1
                    mbs3_putc       'G'
                    J               .l4

;               else
.l3

;                   if row != curow then
                    ld              b0, [row]
                    ld              b1, [curow]
                    cmp             b0, b1
                    jz              .l5

;                       ansi curdown (CSI prefix)
                        mbs3_putc       27
                        mbs3_putc       '['
            
;                       if row - curow > 1 then
                        sub             b0, b1
                        cmp             b0, 1
                        jz             .l6

;                           ansi curdown quanta (row - curow) 
                            eor             b1, b1
                            mul             w0, 4, w3
                            mov             w2, lbs3gfxstr256
                            add             w0, w2
                            mbs3_print_     w0

;                       endif
.l6
            
;                       ansi curdown (suffix)
                        mbs3_putc       'B'

;                   endif
.l5

;               endif
.l4 

;           endif
.l2

;           if lastfc != byte ebs3screen[scridx] then
            mov             w0, ebs3screen
            ld              w1, [scridx]
            ld              b4, [w0 + w1]
            ld              b5, [lastfc]
            cmp             b4, b5
            jz              .l7

;               ansi forecolor byte ebs3screen[scridx]
                push            b4
                mbs3_putc       27
                mbs3_putc       '['
                mbs3_putc       '3'
                mbs3_putc       '8'
                mbs3_putc       ';'
                mbs3_putc       '5'
                mbs3_putc       ';'
                mov             w3, lbs3gfxstr256
                push            b4
                pop             w1
                mul             w1, 4, w2
                add             w1, w3
                mbs3_print_     w1
                mbs3_putc       'm'
            
;               lastfc = byte ebs3screen[scridx]
                pop             b4
                sr              b4, [lastfc]

;           endif
.l7 

;           if lastbc != byte ebs3screen[scridx + 1] then
            mov             w0, ebs3screen
            ld              w1, [scridx]
            ld              b4, [w0 + w1 + 1]
            ld              b5, [lastbc]
            cmp             b4, b5
            jz              .l8

;               ansi backcolor byte ebs3screen[scridx + 1]
                push            b4
                mbs3_putc       27
                mbs3_putc       '['
                mbs3_putc       '4'
                mbs3_putc       '8'
                mbs3_putc       ';'
                mbs3_putc       '5'
                mbs3_putc       ';'
                mov             w3, lbs3gfxstr256
                push            b4
                pop             w1
                mul             w1, 4, w2
                add             w1, w3
                mbs3_print_     w1
                mbs3_putc       'm'

;               lastbc = byte ebs3screen[scridx + 1]
                pop             b4
                sr              b4, [lastbc]

;           endif
.l8

;           print upper half block
            mbs3_putc       $E2
            mbs3_putc       $96
            mbs3_putc       $80

;           cucol = col +1
            ld              b0, [col]
            inc             b0
            sr              b0, [cucol]

;           curow = row
            ld              b0, [row]
            sr              b0, [curow]


;       endif
.continue

;       scridx += 2      
        ld              w0, [scridx]
        add             w0, 2
        sr              w0, [scridx]

;   next col
    endfwbloop      col

; next row             
    endfwbloop      row
    mbs3_putc   10
    ret

; data area of above subroutine
scridx      dw      0
lastfc      db      0
lastbc      db      0
curow       db      0
cucol       db      0
row         db      0
col         db      0


;   clear screen with a value
;   expect 1 parameter:
;     1) register b0 byte value or register
;   NB: register w0,w1 and w2 are modified
lbs3gfxclear    ; b0 is  screen value
            mov     b4, b0
            mov     b5, b4
            mov     w0, ebs3screen
            mov     w1, ebs3gfxlength
.loop       cmp     w1,0
            jz      .endloop
            sr      w2, [w0]
            add     w0, 2
            sub     w1, 2
            j       .loop
.endloop                        
                ret


; look down here
;   |
;   |
;   |
;   |
;   |
;   |
;   |
;   v
; line E(100Pi) ;)

; set w0 = address of pixel at coordinates (x,y)
; w1 and w2 registers are modified
mbs3gfx_xyaddr  macro ; {1} x stack offset, {2} y stack offset
            ; addr = screen+(x<<1)+parity(y)+[((y>>1)*width)<<1]
            ; NB: parity(y) is the value of y bit 0.
            ld      w0, [sp + {1}]  ; take x
            shl     w0                          ; x'=2x
            ld      w1, [sp + {2}]  ; take y
            shr     w1                          ; y'=E(y/2)
            adc     w0, 0                       ; x"+= parity(y)
            mul     w1, ebs3gfxwidth2, w2       ; y"=y'*width*2
            add     w0, w1                      ; idx=x"+y"
            add     w0, ebs3screen              ; addr=screen+idx
            endm

;   Set a pixel color at location x,y (zero based coordinate)
;   (0,0) is top left coordinate
;   expect 3 parameters in stack 
;    1) SP + 6  : x coordinate (first  push)
;    2) SP + 4  : y coordinate (second push)
;    3) SP + 2  : color        (third  push)
;       color value is between 0 to 255 : ANSI 256 colors palette
ebs3gfx_pixel_x equ     6
ebs3gfx_pixel_y equ     4
ebs3gfx_pixel_c equ     2

lbs3gfxpixel
            mbs3gfx_xyaddr ebs3gfx_pixel_x, ebs3gfx_pixel_y
            ld      b2, [sp + ebs3gfx_pixel_c]
            sr      b2, [w0]
            ret

;   Horizontal pixel line
;   draw a horizontal line with xmin, xmax, y and color
;   expect 4 parameters in stack
;     1) SP + 8 : xmin: xmin <= xmax (first  push)
;     2) SP + 6 : xmax: xmax >= xmin (second push)
;     3) SP + 4 : y                  (third  push)
;     4) SP + 2 : color              (fourth push)
;       color value is between 0 to 255 : ANSI 256 colors palette
ebs3gfx_hline_xmin  equ     8
ebs3gfx_hline_xmax  equ     6
ebs3gfx_hline_y     equ     4
ebs3gfx_hline_c     equ     2

lbs3gfxhline
            mbs3gfx_xyaddr  ebs3gfx_hline_xmin, ebs3gfx_hline_y
            ; now w0 is address of first pixel
            ld              b7, [sp + ebs3gfx_hline_c]   ; b7 = color
            ld              b6, [sp + ebs3gfx_hline_xmax]
            ld              b5, [sp + ebs3gfx_hline_xmin]
            sub             b6, b5
            inc             b6                           ; b6 = length
.pixeloop   cmp             b6, 0
            jz              .endline
            sr              b7, [w0]
            dec             b6
            add             w0, 2
            j               .pixeloop
.endline            
            ret

;   vertical pixel line
;   draw a vertical line with ymin, ymax, x and color
;   expect 4 parameters in stack
;     1) SP + 8 : ymin: ymin <= ymax (first  push)
;     2) SP + 6 : ymax: ymax >= ymin (second push)
;     3) SP + 4 : x                  (third  push)
;     4) SP + 2 : color              (fourth push)
;       color value is between 0 to 255 : ANSI 256 colors palette
ebs3gfx_vline_ymin  equ     8
ebs3gfx_vline_ymax  equ     6
ebs3gfx_vline_x     equ     4
ebs3gfx_vline_c     equ     2

lbs3gfxvline
            mbs3gfx_xyaddr  ebs3gfx_vline_x, ebs3gfx_vline_ymin
            ; now w0 is address of first pixel
            ld              b7, [sp + ebs3gfx_vline_c]   ; b7 = color
            ld              b6, [sp + ebs3gfx_vline_ymax]
            ld              w2, [sp + ebs3gfx_vline_ymin] ; b4=ymin
            mov             w1, ebs3gfxwidth2
            dec             w1                          ; w1=bigoffset
            sub             b6, b4
            inc             b6                           ; b6 = length
            not             w2
            and             w2, 1
            jnz             .pixeloop
            mov             w2, w1                       ; w2 = addoff

.pixeloop   cmp             b6, 0
            jz              .endline
            sr              b7, [w0]
            dec             b6
            add             w0, w2
            cmp             b4, b2
            jz              .off1
            mov             w2, w1
            j               .pixeloop
.off1       mov             w2, 1
            j               .pixeloop            
.endline            
            ret

;   Draw a rectangle with 4 coordinates xmin,ymin xmax,ymax
;   with a byte color (between 0 and 255 : ansi 256 color palette)
;   expect 5 parameters in stack
;     1) SP + 10 : xmin: xmin <= xmax  (first  push)
;     2) SP + 8  : ymin: ymin <= ymax  (second push)
;     3) SP + 6  : xmax: xmax >= xmin  (third  push)
;     4) SP + 4  : ymax: ymax >= ymin  (fourth push)
;     5) SP + 2  : color               (fifth  push)
;       color value is between 0 to 255 : ANSI 256 colors palette
;       no optimazation or check done : each corner are ploted twice
ebs3gfx_box_xmin  equ     10
ebs3gfx_box_xmax  equ     6
ebs3gfx_box_ymin  equ     8
ebs3gfx_box_ymax  equ     4
ebs3gfx_box_c     equ     2

lbs3gfxbox
            mov     w3, sp
            sr      w3, [lbs3gfxbox_data1]
            ; top horizontal line
            ld      b0, [w3 + ebs3gfx_box_xmin]
            push    b0
            ld      b0, [w3 + ebs3gfx_box_xmax]
            push    b0
            ld      b0, [w3 + ebs3gfx_box_ymin]
            push    b0
            ld      b0, [w3 + ebs3gfx_box_c]
            push    b0
            call    lbs3gfxhline
            ; bottom horizontal line
            ld      w3, [lbs3gfxbox_data1]
            ld      b0, [w3 + ebs3gfx_box_ymax]
            sr      b0, [sp + 2]
            call    lbs3gfxhline
            ; left vertical line
            ld      w3, [lbs3gfxbox_data1]
            ld      b0, [w3 + ebs3gfx_box_ymin]
            sr      b0, [sp + 6]
            ld      b0, [w3 + ebs3gfx_box_ymax]
            sr      b0, [sp + 4]
            ld      b0, [w3 + ebs3gfx_box_xmin]
            sr      b0, [sp + 2]
            call    lbs3gfxvline
            ; right vertical line
            ld      w3, [lbs3gfxbox_data1]
            ld      b0, [w3 + ebs3gfx_box_xmax]
            sr      b0, [sp + 2]
            call    lbs3gfxvline
            drop
            drop
            drop
            drop
            ret
lbs3gfxbox_data1 ; used to save SP
            dw      0

;   Draw a rectangle with 4 coordinates xmin,ymin xmax,ymax
;   with a byte color (between 0 and 255 : ansi 256 color palette)
;   expect 5 parameters in stack
;     1) SP + 10 : xmin: xmin <= xmax  (first  push)
;     2) SP + 8  : ymin: ymin <= ymax  (second push)
;     3) SP + 6  : xmax: xmax >= xmin  (third  push)
;     4) SP + 4  : ymax: ymax >= ymin  (fourth push)
;     5) SP + 2  : color               (fifth  push)
;       color value is between 0 to 255 : ANSI 256 colors palette
;       no optimazation or check done : each corner are ploted twice
lbs3gfxfullbox
            mov     w3, sp
            sr      w3, [lbs3gfxbox_data1]
            ; top horizontal line
            ld      b0, [w3 + ebs3gfx_box_xmin]
            push    b0
            ld      b0, [w3 + ebs3gfx_box_xmax]
            push    b0
            ld      b0, [w3 + ebs3gfx_box_ymin]
            push    b0
            ld      b0, [w3 + ebs3gfx_box_c]
            push    b0
.boxfill
            call    lbs3gfxhline
            ; next horizontal line
            ld      w3, [lbs3gfxbox_data1]
            ld      b0, [sp + 2]
            ld      b1, [w3 + ebs3gfx_box_ymax]
            cmp     b0, b1
            jz      .endboxfill
            inc     b0    
            sr      b0, [sp + 2]
            j       .boxfill

.endboxfill
            drop
            drop
            drop
            drop
            ret

;   Draw a font glyph
;   w0 address on screen (x, y & $FE ) even Y coordinate
;   w1 font glyph address 
;   b4 fore color
;   no W register changed
lbs3gfxglyph
            push_w0
            push_b6
            and     w0, $FFFE
.PT         ld      b6, [w1]

.PT8        shl     b6
            jnc     .PT7
            sr      b4, [w0]
.PT7        shl     b6            
            jnc     .PT6
            sr      b4, [w0 + 1]
.PT6        shl     b6            
            jnc     .PT5
            sr      b4, [w0 + 2]
.PT5        shl     b6            
            jnc     .PT4
            sr      b4, [w0 + 3]
.PT4        shl     b6            
            jnc     .PT3
            sr      b4, [w0 + 4]
.PT3        shl     b6            
            jnc     .PT2
            sr      b4, [w0 + 5]
.PT2        shl     b6            
            jnc     .PT1
            sr      b4, [w0 + 6]
.PT1        shl     b6            
            jnc     .PM
            sr      b4, [w0 + 7]

.PM         ld      b6, [w1 + 1]
            add     w0, ebs3gfxwidth2

.PM8        shl     b6
            jnc     .PM7
            sr      b4, [w0]
.PM7        shl     b6            
            jnc     .PM6
            sr      b4, [w0 + 1]
.PM6        shl     b6            
            jnc     .PM5
            sr      b4, [w0 + 2]
.PM5        shl     b6            
            jnc     .PM4
            sr      b4, [w0 + 3]
.PM4        shl     b6            
            jnc     .PM3
            sr      b4, [w0 + 4]
.PM3        shl     b6            
            jnc     .PM2
            sr      b4, [w0 + 5]
.PM2        shl     b6            
            jnc     .PM1
            sr      b4, [w0 + 6]
.PM1        shl     b6            
            jnc     .PB
            sr      b4, [w0 + 7]

.PB         ld      b6, [w1 + 2]
            add     w0, ebs3gfxwidth2

.PB8        shl     b6
            jnc     .PB7
            sr      b4, [w0]
.PB7        shl     b6            
            jnc     .PB6
            sr      b4, [w0 + 1]
.PB6        shl     b6            
            jnc     .PB5
            sr      b4, [w0 + 2]
.PB5        shl     b6            
            jnc     .PB4
            sr      b4, [w0 + 3]
.PB4        shl     b6            
            jnc     .PB3
            sr      b4, [w0 + 4]
.PB3        shl     b6            
            jnc     .PB2
            sr      b4, [w0 + 5]
.PB2        shl     b6            
            jnc     .PB1
            sr      b4, [w0 + 6]
.PB1        shl     b6            
            jnc     .endplot
            sr      b4, [w0 + 7]
.endplot
            pop_b6
            pop_w0
            ret

;   Draw a character
;   w0 address on screen
;   b2 character to print
;   b4 color
;   after call:
;       registers b2-b3, w1 are changed
lbs3gfxchar
            eor     b3, b3          ; w1 = b2 = character
            and     b2, $7F         ; takes ascii 0..127
            tst     b2, $E0
            jz      .endchar        ; do not print ascii 0..31
            ; compute w1 to be address to the right font glyph
            sub     b2, $20         ; first glyph for ASCII ' '/32
            push_w2
            mul     w1, 3, w2       ; 3 bytes per glyph
            pop_w2
            tst     w0, 1           ; odd screen => odd Y and font
            jz      .fonteven
            add     w1, lbs3gfxtinyfont_odd
            j       .printout
.fonteven   add     w1, lbs3gfxtinyfont_even
.printout   call lbs3gfxglyph
.endchar
            ret

;   Draw a character string
;   expect 4 parameters in stack
;     1) SP + 8 : String address    (first  push)
;     2) SP + 6 : x coordinate      (second push)
;     3) SP + 4 : y coordinate      (third  push)
;     4) SP + 2 : c color           (fourth push)
ebs3gfx_drawstr_addr    equ     8
ebs3gfx_drawstr_x       equ     6
ebs3gfx_drawstr_y       equ     4
ebs3gfx_drawstr_c       equ     2

lbs3gfxdrawstr
            ; compute w0 = address on screen
            mbs3gfx_xyaddr  ebs3gfx_drawstr_x, ebs3gfx_drawstr_y

            ; b4 = color
            ld      b4, [SP + ebs3gfx_drawstr_c]
            ; w3 = current character address
            ld      w3, [SP + ebs3gfx_drawstr_addr]
            ; b2 = current character
.loopstr    ld      b2, [w3]
            cmp     b2, 0
            jz      .endloopstr
            call    lbs3gfxchar
            add     w0, 8
            inc     w3
            j       .loopstr
.endloopstr
            ret

;   Initialize the screen and shadow
;   by doing a double screen clear
;   one with color 232 (black in grey scale palette)
;   one with color 0  (black in classic pallete)
;   No parameter expected
lbs3gfxinit
            ; compute tiny font at odd Y coordinate
            c               lbs3gfxfontodd 
            ; clear to 232 (black in grey scale)
            mov             b0, 232
            call            lbs3gfxclear
            call            lbs3gfxshow
            ; clear to 0 (black in classic 16 palette)
            eor             b0, b0
            call            lbs3gfxclear
            call            lbs3gfxshow
            ret

; compute tiny font at odd Y coordinate
lbs3gfxfontodd 
            leaf_w0         lbs3gfxtinyfont_even
            leaf_w1         lbs3gfxtinyfont_odd

            mov             b4, 96 ; 96 characters to generate
.loopchar   cmp             b4, 0            
            jz              .endloopchar

            ; first 4x2 bit font 
            ld              b7, [w0]
            shr             b7
            and             b7, $55
            sr              b7, [w1]

            ; second 4x2 bits font 
            ld              w3, [w0] ; b6=[w0], b7=[W0+1]
            shr             b7
            and             b7, $55
            shl             b6
            and             b6, $AA 
            or              b7, b6
            sr              b7, [w1 + 1]

            ; third 4x2 bits font
            ld              w3, [w0 + 1] ; b6=[w0+1], b7=[W0+2]
            shr             b7
            and             b7, $55
            shl             b6
            and             b6, $AA 
            or              b7, b6
            sr              b7, [w1 + 2]

            ; increment addr to next font glyph
            add             w0, 3
            add             w1, 3

            dec             b4
            j               .loopchar
.endloopchar
            ret


    ; from 0 to 255 in string, each string is 4 bytes long
    ; from 'n' binary , string addr = lbs3gfxstr256 + n * 4
lbs3gfxstr256
    db  "0",0,0,0, "1",0,0,0, "2",0,0,0, "3",0,0,0, "4",0,0,0
    db  "5",0,0,0, "6",0,0,0, "7",0,0,0, "8",0,0,0, "9",0,0,0
    db  "10",0,0,  "11",0,0,  "12",0,0,  "13",0,0,  "14",0,0
    db  "15",0,0,  "16",0,0,  "17",0,0,  "18",0,0,  "19",0,0
    db  "20",0,0,  "21",0,0,  "22",0,0,  "23",0,0,  "24",0,0
    db  "25",0,0,  "26",0,0,  "27",0,0,  "28",0,0,  "29",0,0
    db  "30",0,0,  "31",0,0,  "32",0,0,  "33",0,0,  "34",0,0
    db  "35",0,0,  "36",0,0,  "37",0,0,  "38",0,0,  "39",0,0
    db  "40",0,0,  "41",0,0,  "42",0,0,  "43",0,0,  "44",0,0
    db  "45",0,0,  "46",0,0,  "47",0,0,  "48",0,0,  "49",0,0
    db  "50",0,0,  "51",0,0,  "52",0,0,  "53",0,0,  "54",0,0
    db  "55",0,0,  "56",0,0,  "57",0,0,  "58",0,0,  "59",0,0
    db  "60",0,0,  "61",0,0,  "62",0,0,  "63",0,0,  "64",0,0
    db  "65",0,0,  "66",0,0,  "67",0,0,  "68",0,0,  "69",0,0
    db  "70",0,0,  "71",0,0,  "72",0,0,  "73",0,0,  "74",0,0
    db  "75",0,0,  "76",0,0,  "77",0,0,  "78",0,0,  "79",0,0
    db  "80",0,0,  "81",0,0,  "82",0,0,  "83",0,0,  "84",0,0
    db  "85",0,0,  "86",0,0,  "87",0,0,  "88",0,0,  "89",0,0
    db  "90",0,0,  "91",0,0,  "92",0,0,  "93",0,0,  "94",0,0
    db  "95",0,0,  "96",0,0,  "97",0,0,  "98",0,0,  "99",0,0
    db  "100", 0,  "101", 0,  "102", 0,  "103", 0,  "104", 0
    db  "105", 0,  "106", 0,  "107", 0,  "108", 0,  "109", 0
    db  "110", 0,  "111", 0,  "112", 0,  "113", 0,  "114", 0
    db  "115", 0,  "116", 0,  "117", 0,  "118", 0,  "119", 0
    db  "120", 0,  "121", 0,  "122", 0,  "123", 0,  "124", 0
    db  "125", 0,  "126", 0,  "127", 0,  "128", 0,  "129", 0
    db  "130", 0,  "131", 0,  "132", 0,  "133", 0,  "134", 0
    db  "135", 0,  "136", 0,  "137", 0,  "138", 0,  "139", 0
    db  "140", 0,  "141", 0,  "142", 0,  "143", 0,  "144", 0
    db  "145", 0,  "146", 0,  "147", 0,  "148", 0,  "149", 0
    db  "150", 0,  "151", 0,  "152", 0,  "153", 0,  "154", 0
    db  "155", 0,  "156", 0,  "157", 0,  "158", 0,  "159", 0
    db  "160", 0,  "161", 0,  "162", 0,  "163", 0,  "164", 0
    db  "165", 0,  "166", 0,  "167", 0,  "168", 0,  "169", 0
    db  "170", 0,  "171", 0,  "172", 0,  "173", 0,  "174", 0
    db  "175", 0,  "176", 0,  "177", 0,  "178", 0,  "179", 0
    db  "180", 0,  "181", 0,  "182", 0,  "183", 0,  "184", 0
    db  "185", 0,  "186", 0,  "187", 0,  "188", 0,  "189", 0
    db  "190", 0,  "191", 0,  "192", 0,  "193", 0,  "194", 0
    db  "195", 0,  "196", 0,  "197", 0,  "198", 0,  "199", 0
    db  "200", 0,  "201", 0,  "202", 0,  "203", 0,  "204", 0
    db  "205", 0,  "206", 0,  "207", 0,  "208", 0,  "209", 0
    db  "210", 0,  "211", 0,  "212", 0,  "213", 0,  "214", 0
    db  "215", 0,  "216", 0,  "217", 0,  "218", 0,  "219", 0
    db  "220", 0,  "221", 0,  "222", 0,  "223", 0,  "224", 0
    db  "225", 0,  "226", 0,  "227", 0,  "228", 0,  "229", 0
    db  "230", 0,  "231", 0,  "232", 0,  "233", 0,  "234", 0
    db  "235", 0,  "236", 0,  "237", 0,  "238", 0,  "239", 0
    db  "240", 0,  "241", 0,  "242", 0,  "243", 0,  "244", 0
    db  "245", 0,  "246", 0,  "247", 0,  "248", 0,  "249", 0
    db  "250", 0,  "251", 0,  "252", 0,  "253", 0,  "254", 0
    db  "255", 0

lbs3gfxtinyfont ; 5x3 tiny font (6x4 occupied space)
; inspired from https://fontsgeek.com/small-5x3-font

lbs3gfxtinyfont_even
; comment on font description comment
; each glyph is defined by a 
; matrix of 4x6 bits (4 pixels width, 6 pixels height)
; each bit 1 is represented by a '#'
; each bit 0 is represented by a '.'
;  bit reading and encoding as follow
    ;    <4p>
    ;  ^ 8642
    ;  | 7531  8765 4321 $00 (hexa result of 8 bits)
    ;6p| 8642
    ;  | 7531  8765 4321 $00 (hexa result of 8 bits)
    ;  | 8642
    ;  v 7531  8765 4321 $00 (hexa result of 8 bits)

    ; even Y, character 32 to 127
    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  ....  0000 0000 $00
    db  $00, $00, $00    ; 32  : $20 : ' ' Space 

    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .#..
    ;  ....  0010 0000 $20
    ;  .#..
    ;  ....  0000 0000 $00
    db  $30, $20, $00    ; 33  : $21 : ! Exclamation mark

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  ....  0000 0000 $00
    db  $CC, $00, $00    ; 34  : $22 : " Double quote

    ;  #.#.
    ;  ###.  1101 1100 $DC
    ;  #.#.
    ;  ###.  1101 1100 $DC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $DC, $DC, $88    ; 35  : $23 : # Number

    ;  ###.
    ;  ##..  1111 1000 $F8
    ;  ###.
    ;  .##.  1011 1100 $BC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $F8, $BC, $A8    ; 36  : $24 : $ Dollar Sign

    ;  #...
    ;  ..#.  1000 0100 $84
    ;  .#..
    ;  #...  0110 0000 $60
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $84, $60, $08    ; 37  : $25 : % Percent

    ;  .#..
    ;  ###.  0111 0100 $74
    ;  #...
    ;  ###.  1101 0100 $D4
    ;  .#..
    ;  ....  0010 0000 $20
    db  $74, $D4, $20    ; 38  : $26 : & Ampersand

    ;  .#..
    ;  .#..  0011 0000 $30
    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  ....  0000 0000 $00
    db  $30, $00, $00    ; 39  : $27 : ' Single quote

    ;  ..#.
    ;  .#..  0001 1000 $18
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $18, $00, $00    ; 40  : $28 : ( Left parenthesis

    ;  #...
    ;  .#..  1001 0000 $90
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  #...
    ;  ....  1000 0000 $80
    db  $90, $30, $80    ; 41  : $29 : ) Right parenthesis

    ;  ....
    ;  .#..  0001 0000 $10
    ;  #.#.
    ;  .#..  1001 1000 $98
    ;  ....
    ;  ....  0000 0000 $00
    db  $10, $98, $00    ; 42  : $2A : * Asterisk
   
    ;  ....
    ;  .#..  0001 0000 $10
    ;  ###.
    ;  .#..  1011 1000 $B8
    ;  ....
    ;  ....  0000 0000 $00
    db  $10, $B8, $00    ; 43  : $2B : + Plus

    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  .#..  0001 0000 $10
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $00, $10, $A0    ; 44  : $2C : , Comma

    ;  ....
    ;  ....  0000 0000 $00
    ;  ###.
    ;  ....  1010 1000 $A8
    ;  ....
    ;  ....  0000 0000 $00
    db  $00, $A8, $00    ; 45  : $2D : - Minus

    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  ....  0000 0000 $00
    ;  .#..
    ;  ....  0010 0000 $20
    db  $00, $00, $20    ; 46  : $2E : . Period

    ;  ....
    ;  ..#.  0000 0100 $04
    ;  .#..
    ;  #...  0110 0000 $60
    ;  ....
    ;  ....  0000 0000 $00
    db  $04, $60, $00    ; 47  : $2F : / Slash

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $EC, $CC, $A8    ; 48  : $30 : '0'

    ;  ##..
    ;  .#..  1011 0000 $B0
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $B0, $30, $AB    ; 49  : $31 : '1'
    
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ###.
    ;  #...  1110 1000 $E8
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $AC, $E8, $A8    ; 50  : $32 : '2'

    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $AC, $AC, $A8    ; 51  : $33 : '3'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $CC, $AC, $08    ; 52  : $34 : '4'

    ;  ###.
    ;  #...  1110 1000 $E8
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $E8, $AC, $A8    ; 53  : $35 : '5'

    ;  ###.
    ;  #...  1110 1000 $E8
    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $E8, $EC, $A8    ; 54  : $36 : '6'

    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ..#.
    ;  ..#.  0000 1100 $0C
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $00, $00, $00    ; 55  : $37 : '7'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $EC, $EC, $A8    ; 56  : $38 : '8'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $EC, $AC, $00    ; 57  : $39 : '9'

    ;  ....
    ;  .#..  0001 0000 $10
    ;  ....
    ;  .#..  0001 0000 $10
    ;  ....
    ;  ....  0000 0000 $00
    db  $10, $10, $00    ; 58  : $3A : : Colon

    ;  ....
    ;  .#..  0001 0000 $10
    ;  ....
    ;  .#..  0001 0000 $10
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $10, $10, $A0    ; 59  : $3B : ; Semicolon

    ;  ..#.
    ;  .#..  0001 1000 $18
    ;  #...
    ;  .#..  1001 0000 $90
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $18, $90, $08    ; 60  : $3C : < Less than

    ;  ....
    ;  ###.  0101 0100 $54
    ;  ....
    ;  ###.  0101 0100 $54
    ;  ....
    ;  ....  0000 0000 $00
    db  $54, $54, $00    ; 61  : $3D : = Equality sign
    
    ;  #...
    ;  .#..  1001 0000 $90
    ;  ..#.
    ;  .#..  0001 1000 $18
    ;  #...
    ;  ....  1000 0000 $80
    db  $90, $18, $80    ; 62  : $3E : > Greater than

    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  .##.
    ;  ....  0010 1000 $28
    ;  .#..
    ;  ....  0010 0000 $20
    db  $AC, $28, $20    ; 63  : $3F : ? Question mark

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  #...  1100 1000 $C8
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $EC, $C8, $A8    ; 64  : $40 : @ At sign

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $EC, $EC, $88    ; 65  : $41 : 'A'
    
    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ##..
    ;  #.#.  1110 0100 $E4
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $EC, $E4, $A8    ; 66  : $42 : 'B'

    ;  ###.
    ;  #...  1110 1000 $E8
    ;  #...
    ;  #...  1100 0000 $C0
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $E8, $C0, $A8    ; 67  : $43 : 'C'

    ;  ##..
    ;  #.#.  1110 0100 $EC
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $EC, $CC, $A0    ; 68  : $44 : 'D'

    ;  ###.
    ;  #...  1110 1000 $E8
    ;  ###.
    ;  #...  1110 1000 $E8
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $E8, $E8, $A8    ; 69  : $45 : 'E'

    ;  ###.
    ;  #...  1110 1000 $E8
    ;  ###.
    ;  #...  1110 1000 $E8
    ;  #...
    ;  ....  1000 0000 $80
    db  $E8, $E8, $80    ; 70  : $46 : 'F'

    ;  ###.
    ;  #...  1110 1000 $E8
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $E8, $CC, $A8    ; 71  : $47 : 'G'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $CC, $EC, $88    ; 72  : $48 : 'H'

    ;  ###.
    ;  .#..  1011 1000 $B8
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $B8, $30, $A8    ; 73  : $49 : 'I'

    ;  ..#.
    ;  ..#.  0000 1100 $0C
    ;  ..#.
    ;  #.#.  0100 1100 $4C
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $0C, $4C, $A8    ; 74  : $4A : 'J'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ##..
    ;  #.#.  1110 0100 $E4
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $CC, $E4, $88    ; 75  : $4B : 'K'

    ;  #...
    ;  #...  1100 0000 $C0
    ;  #...
    ;  #...  1100 0000 $C0
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $C0, $C0, $A8    ; 76  : $4C : 'L'

    ;  #.#.
    ;  ###.  1101 1100 $DC
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $DC, $CC, $88    ; 77  : $4D : 'M'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $EC, $CC, $88    ; 78  : $4E : 'N'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $EC, $CC, $A8    ; 79  : $4F : 'O'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ###.
    ;  #...  1110 1000 $E8
    ;  #...
    ;  ....  1000 0000 $80
    db  $EC, $E8, $80    ; 80  : $50 : 'P'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  ###.  1101 1100 $DC
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $EC, $DC, $08    ; 81  : $51 : 'Q'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ##..
    ;  #.#.  1110 0100 $E4
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $EC, $E4, $88    ; 82  : $52 : 'R'

    ;  ###.
    ;  #...  1110 1000 $E8
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $E8, $AC, $A8    ; 83  : $53 : 'S'

    ;  ###.
    ;  .#..  1011 1000 $B8
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .#..
    ;  ....  0010 0000 $20
    db  $B8, $30, $20    ; 84  : $54 : 'T'
    
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $CC, $CC, $A8    ; 85  : $55 : 'U'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  .#..
    ;  ....  0010 0000 $20
    db  $CC, $CC, $20    ; 86  : $56 : 'V'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  #.#.
    ;  ###.  1101 1100 $DC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $CC, $DC, $88    ; 87  : $57 : 'W'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  .#..
    ;  #.#.  0110 0100 $64
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $CC, $64, $88    ; 88  : $58 : 'X'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  .#..  1011 1000 $B8
    ;  .#..
    ;  ....  0010 0000 $20
    db  $CC, $B8, $20    ; 89  : $59 : 'Y'

    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  .#..
    ;  #...  0110 0000 $60
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $AC, $60, $A8    ; 90  : $5A : 'Z'

    ;  .##.
    ;  .#..  0011 1000 $38
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .##.
    ;  ....  0010 1000 $28
    db  $38, $30, $28    ; 91  : $5B : [ Left square bracket

    ;  ....
    ;  #...  0100 0000 $40
    ;  .#..
    ;  ..#.  0010 0100 $24
    ;  ....
    ;  ....  0000 0000 $00
    db  $00, $00, $00    ; 92  : $5C : \ Backslash ;)

    ;  ##..
    ;  .#..  1011 0000 $B0
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $B0, $30, $A0    ; 93  : $5D : ] Right square bracket

    ;  .#..
    ;  #.#.  0110 0100 $64
    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  ....  0000 0000 $00
    db  $64, $00, $00    ; 94  : $5E : ^ Caret / Circumflex

    ;  ....
    ;  ....  0000 0000 $00
    ;  ....
    ;  ....  0000 0000 $00
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $00, $00, $A8    ; 95  : $5F : _ Underscore

    ;  .#..
    ;  ..#.  0010 0100 $24
    ;  ....
    ;  ....  1100 1100 $00
    ;  ....
    ;  ....  0010 1000 $00
    db  $24, $00, $00    ; 96  : $60 : ` Back quote

    ;  ....
    ;  .##.  0001 0100 $14
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  .##.
    ;  ....  0010 1000 $28
    db  $14, $CC, $28    ; 97  : $61 : 'a'

    ;  #...
    ;  ##..  1101 0000 $D0
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $D0, $CC, $A0    ; 98  : $62 : 'b'

    ;  ....
    ;  ###.  0101 0100 $54
    ;  #...
    ;  #...  1100 0000 $C0
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $54, $C0, $A8    ; 99  : $63 : 'c'
    
    ;  ..#.
    ;  .##.  0001 1100 $1C
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  .##.
    ;  ....  0010 1000 $28
    db  $1C, $CC, $28    ; 100 : $64 : 'd'

    ;  ....
    ;  .#..  0001 0000 $10
    ;  #.#.
    ;  ##..  1101 1000 $D8
    ;  .##.
    ;  ....  0010 1000 $28
    db  $10, $D8, $28    ; 101 : $65 : 'e'
    
    ;  .##.
    ;  .#..  0011 1000 $38
    ;  ###.
    ;  .#..  1011 1000 $B8
    ;  .#..
    ;  ....  0010 0000 $20
    db  $38, $B8, $20    ; 102 : $66 : 'f'

    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $EC, $AC, $A0    ; 103 : $67 : 'g'
    
    ;  #...
    ;  ##..  1101 0000 $D0
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $D0, $CC, $88    ; 104 : $68 : 'h'

    ;  .#..
    ;  ....  0010 0000 $20
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .#..
    ;  ....  0010 0000 $20
    db  $20, $30, $20    ; 105 : $69 : 'i'

    ;  .#..
    ;  ....  0010 0000 $20
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $20, $30, $A0    ; 106 : $6A : 'j'

    ;  #...
    ;  #.#.  1100 0100 $C4
    ;  ##..
    ;  #.#.  1110 0100 $E4
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $C4, $E4, $88    ; 107 : $6B : 'k'

    ;  ##..
    ;  .#..  1011 0000 $B0
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .#..
    ;  ....  0010 0000 $20
    db  $B0, $30, $20    ; 108 : $6C : 'l'

    ;  ....
    ;  #.#.  0100 0100 $44
    ;  ###.
    ;  #.#.  1110 1100 $EC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $44, $EC, $88    ; 109 : $6D : 'm'

    ;  ....
    ;  ###.  0101 0100 $54
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $54, $CC, $88    ; 110 : $6E : 'n'

    ;  ....
    ;  ###.  0101 0100 $54
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $54, $CC, $A8    ; 111 : $6F : 'o'
    
    ;  ##..
    ;  # #.  1110 0100 $E4
    ;  #.#.
    ;  ##..  1101 1000 $D8
    ;  #...
    ;  ....  1000 0000 $80
    db  $E4, $D8, $80    ; 112 : $70 : 'p'

    ;  .##.
    ;  # #.  0110 1100 $6C
    ;  #.#.
    ;  .##.  1001 1100 $9C
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $6C, $9C, $08    ; 113 : $71 : 'q'

    ;  ....
    ;  ###.  0101 0100 $54
    ;  #.#.
    ;  #...  1100 1000 $C8
    ;  #...
    ;  ....  1000 0000 $80
    db  $54, $C8, $80    ; 114 : $72 : 'r'

    ;  ....
    ;  .##.  0001 0100 $14
    ;  #..
    ;  ..#.  1000 0100 $84
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $14, $84, $A0    ; 115 : $73 : 's'

    ;  .#..
    ;  ###.  0111 0100 $74
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .##.
    ;  ....  0010 1000 $28
    db  $74, $30, $28    ; 116 : $74 : 't'

    ;  ....
    ;  #.#.  0100 0100 $44
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $44, $CC, $A8    ; 117 : $75 : 'u'

    ;  ....
    ;  #.#.  0100 0100 $44
    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  .#..
    ;  ....  0010 0000 $20
    db  $44, $CC, $20    ; 118 : $76 : 'v'

    ;  ....
    ;  #.#.  0100 0100 $44
    ;  #.#.
    ;  ###.  1101 1100 $DC
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $44, $DC, $88    ; 119 : $77 : 'w'

    ;  ....
    ;  #.#.  0100 0100 $44
    ;  .#..
    ;  #.#.  0110 0100 $64
    ;  #.#.
    ;  ....  1000 1000 $88
    db  $44, $64, $88    ; 120 : $78 : 'x'

    ;  #.#.
    ;  #.#.  1100 1100 $CC
    ;  ###.
    ;  ..#.  1010 1100 $AC
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $CC, $AC, $A0    ; 121 : $79 : 'y'

    ;  ....
    ;  ###.  0101 0100 $54
    ;  ..#.
    ;  #...  0100 1000 $48
    ;  ###.
    ;  ....  1010 1000 $A8
    db  $54, $48, $A8    ; 122 : $7A : 'z'

    ;  .##.
    ;  .#..  0011 1000 $38
    ;  ##..
    ;  .#..  1011 0000 $B0
    ;  .##.
    ;  ....  0010 1000 $28
    db  $38, $B0, $28    ; 123 : $7B : { Left curly bracket

    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .#..
    ;  .#..  0011 0000 $30
    ;  .#..
    ;  ....  0010 0000 $20
    db  $30, $30, $20    ; 124 : $7C : | Vertical bar

    ;  ##..
    ;  .#..  1011 0000 $B0
    ;  .##.
    ;  .#..  0011 1000 $38
    ;  ##..
    ;  ....  1010 0000 $A0
    db  $B0, $38, $A0    ; 125 : $7D : } Right curly bracket

    ;  ....
    ;  .#.#  0001 0001 $11
    ;  #.#.
    ;  ....  1000 1000 $88
    ;  ....
    ;  ....  0000 0000 $00
    db  $11, $88, $00    ; 126 : $7E : ~ Tilde

    ;  ..#.
    ;  .#..  0001 1000 $18
    ;  ####
    ;  .#..  1011 1010 $BA
    ;  ..#.
    ;  ....  0000 1000 $08
    db  $18, $BA, $08    ; 127 : $7F : ' ' Delete (here as back arrow)

lbs3gfxtinyfont_odd
; characters 32 to 127 (96 characters) with 3 bytes per character
;  96 x 3 = 288 bytes to reserved
;  this area will contains the computed font when Y location is odd
    space   288 

; include jump over label
.endofansigfx  

