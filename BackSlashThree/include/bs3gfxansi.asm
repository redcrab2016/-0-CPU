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
; line E(100Pi) ;)
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

; set w0 = address of pixel
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

;   Draw a rectangle with 4 coordinate xmin,ymin xmax,ymax
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

;   Initialize the screen and shadow
;   by doing a double screen clear
;   one with color 232 (black in grey scale palette)
;   one with color 0  (black in classic pallete)
;   No parameter expected
lbs3gfxinit
            mov             b0, 232
            call            lbs3gfxclear
            call            lbs3gfxshow
            eor             b0, b0
            call            lbs3gfxclear
            call            lbs3gfxshow
            ret

; include jump over label
.endofansigfx  

