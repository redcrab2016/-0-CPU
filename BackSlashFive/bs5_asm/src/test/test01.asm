; Program test to execute
org 0
; Change video mode from 80 x 40  (default) , to (160 x 100)
    mov R1, 17
    mov [#0xBF00], R1  ; Video register selection (17 : resolution mode)
    mov R1, 20
    mov [#0xBF01], R1  ; Video register value ( 20 => 160 x 100 )

; draw a black square : 50 pixels height, 112 piwels width at top left coordinates (24,25)
;  the method is to draw 50 times a 112 pixels horizontal line
    mov R1, #0xC000 ;  set R1 to top left corner coordinates of the canva
    add R1, #0x0646 ;  set R1 to coordinates (24, 25) = 25 * 64 + 6*4
    mov R4, 0   ;  R4 : 4 pixels color, 0x0000 = 0 = all in black
    mov R5, 50   ; 50 line of height
vertiloop:    
    mov R2, R1   ; set the start of horizontal line to R1
    mov R3, 28   ; 28 * 4 pixels = 112
horizloop:    
    mov [R2], R4
    add R2,1
    fl sub R3,1
    zc add R15, loffset horizloop
    add R1, 64 ; 64 words per line , 64 * 4 = 256 pixels per line
    fl sub R5,1
    zc add R15, loffset vertiloop

; End of program
    mov R14:13, 1      ; halt the CPU, set the 13th flag bit (H flag = 1)
