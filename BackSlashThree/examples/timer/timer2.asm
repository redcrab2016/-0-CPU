; Timer2 , display a counter incrementing with one second period
; demonstrate timer and input interrupt
; stop program when 'space' character input
; this code do the same thing as timer.asm, but use bs3core.inc
        include                 "bs3core.inc"
        mbs3_bootat             start

start   org                     ebs3_start
        cli
        mbs3_sethandler         int_timer, clock
        mbs3_settimermode       ebs3_timer_mode_time
        mbs3_starttimer         $000F, $4240
        mbs3_sethandler         int_byteinput, input
        mbs3_printlnstr         "Type space character to stop"
        sti
        ; idle loop        
.idle:  wait
        ld      b0, [isover]
        cmp     b0, 1
        jnz     .idle       ; loop on idle as long as it is not over
        hlt                 ; end of program

; Input handler : if ' ' (space character) then raise flag 'isover'
input:
        push    b0
.retry        
        in      b0          ; take input character
        jz      .retry      ; retry if necessary
        cmp     b0, ' '     ; is it a space character
        jnz      .over      ; if yes then stop
        mov     b0, 1
        sr      b0, [isover]
.over   pop     b0
        iret                ; end of interrupt routine

; Timer handler : count and display
clock:
        pusha
        ; increment counter
        lean_w1 counter     ; W1 = addr of counter
        ld      w0, [w1]    ; W0 = [W1]
        inc     w0          ; W0 = W0 + 1
        sr      w0, [w1]    ; [W1] = W0
        ; generate decimal ascii string
        lean    w2, decstr      ; w2 = addr of digit buffer
.digit:        
        div     w0, 10, w1  ; w1 = w0 mod 10, w0 = w0 / 10
        add     b2, '0'     ; convert binary 0-9 to ASCII '0'-'9'
        sr      b2, [w2]    ; store character to buffer
        inc     w2          ; increase buffer size
        cmp     w0, 0       ; is it end of number
        jnz     .digit      ; if no then continue to decode
.dispdigit:        
        dec     w2          ; browse back the digit buffer
        ld      b0, [w2]    ; get the character
        cmp     b0, 0       ; is it the last character
        jz      .endigit    ; if yes then finish display
        mbs3_putc   b0
        j       clock.dispdigit  ; print out next character
.endigit:
        mbs3_putc   10
        popa
        iret                ; end of interrupt routine

; data area
counter dw      0
        db      0
decstr  db      "00000", 0
isover  db      0
