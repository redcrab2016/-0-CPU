; Program test to execute
org 0
    mov R15, ptr start

; Plot pixel
; R1 : pixel coordinates yyxx in hexadecimal,  x=00 to ff, y=00 to ff
; R2 : color masked with #0x000f : value from 0 to 15
sub_plotpix:
    ; save modified registers
    mov stack, R1
    mov stack, R2
    mov stack, R3
    mov R3, R1
    and R3, #0x0003
    mov R0, R1
    shr R0, 2
    mov R1, R0
    add R1, #0xC000 ; R1 is now memory location of 4 pixels (modulus 4 address)
    mov R0, R3
    mov R3, [R1]    ; R3 get the 4 pixels value
    no fl sub R0,3              ; compare R0 with 3, R0 is unchanged
    eq add R15, loffset pix3    ; change the 4th quad of the four pixels value (most significant quad, right most pixel )
    no fl sub R0,2              ; compare R0 with 2, R0 is unchanged
    eq add R15, loffset pix2    ; change the 3rd quad of the four pixels value
    no fl sub R0,1              ; compare R0 with 1, R0 is unchanged
    eq add R15, loffset pix1    ; change the 2nd quad of the four pixels value
    ; change the 1st quad of the four pixels value (less significant, left most pixel)
    and R3, #0xfff0
    or  R3, R2
    add R15, loffset writepix
pix1:
    and R3, #0xff0f
    mov R0, R2
    shl R0, 4
    or  R0, R3
    mov R3, R0
    add R15, loffset writepix
pix2:    
    and R3, #0xf0ff
    mov R0, R2
    shl R0, 8
    or  R0, R3
    mov R3, R0
    add R15, loffset writepix
pix3:    
    and R3, #0x0fff
    mov R0, R2
    shl R0, 12
    or  R0, R3
    mov R3, R0
writepix:
    mov [R1], R3
    ; restore modified registers
    mov R3, stack
    mov R2, stack
    mov R1, stack
    ; return to the caller
    mov R15, stack ; end of sub routine

; Entry point of the program
start:    
; Change video mode from 80 x 40  (default) , to (160 x 100)
    mov R1, 17
    mov [#0xBF00], R1  ; Video register selection (17 : resolution mode)
    mov R1, 20
    mov [#0xBF01], R1  ; Video register value ( 20 => 160 x 100 )

; draw a yello square : 50 pixels height, 112 piwels width at top left coordinates (24,25)
;  the method is to draw 50 times a 112 pixels horizontal line
    mov R1, #0xC000 ;  set R1 to top left corner coordinates of the canva
    add R1, #0x0646 ;  set R1 to coordinates (24, 25) = 25 * 64 + 6*4
    mov R4, #0xAAA1 ;  R4 : 4 pixels color, 0x0000 = 0 = all in black, 0x1111 all in blue (CGA text color palette)
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
; unitary red pixels (8 diagonal pixels)
    mov R1, #0x3250 ; x=0x50=80 , y=0x32=50
    mov R2, 15      ; 15=white  (CGA text color palette)
    mov stack:R15, ptr sub_plotpix
    add R1, #0x0101
    mov stack:R15, ptr sub_plotpix
    add R1, #0x0101
    mov stack:R15, ptr sub_plotpix
    add R1, #0x0101
    mov stack:R15, ptr sub_plotpix
    add R1, #0x0101
    mov stack:R15, ptr sub_plotpix
    add R1, #0x0101
    mov stack:R15, ptr sub_plotpix
    add R1, #0x0101
    mov stack:R15, ptr sub_plotpix
    add R1, #0x0101
    mov stack:R15, ptr sub_plotpix



; End of program
    mov R14:13, 1      ; halt the CPU, set the 13th flag bit (H flag = 1)
