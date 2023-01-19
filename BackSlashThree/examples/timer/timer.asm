; Timer , display a counter incrementing with one second period
; demonstrate timer and input interrupt
; stop program when 'space' character input
        org     $0000
        dw      start
sysclk  org     $0006
sysinp  org     $0008
clkl32  org     $0108
clkh32  org     $010A
clkmode org     $010C

start   org     $0400
        cli                 ; disable interrupt
        ; setup clock handler
        lean_w0 clock
        sr      w0, [sysclk]
        mov     w0, $4240     ; 1000 000 micro second low 16 bits
        sr      w0, [clkl32]
        eor     b0, b0
        sr      b0, [clkmode] ; timer in time mode (not cpu tick mode)
        mov     w0, $000F     ; 1000 000 micro second high 16 bits
        sr      w0, [clkh32]
        ; setup input handler
        lean_w0 input
        sr      w0, [sysinp]
        c       welcome
        sti                 ; enable interrupt
.idle:  wait                ; wait for an interrupt
        ld      b0, [isover]
        cmp     b0, 1
        jnz     .idle       ; loop as long as it is not over
        hlt                 ; end of program

; Input handler : if ' ' (space character) then raise flag 'isover'
input:
        push_b0
.retry        
        in      b0          ; take input character
        jz      .retry      ; retry if necessary
        cmp     b0, ' '     ; is it a space character
        jnz      .over      ; if yes then stop
        mov     b0, 1
        sr      b0, [isover]
.over   pop_b0
        iret                ; if no then continue program

; Timer handler : count and display
clock:
        pusha
        ; increment counter
        lean_w1 counter     ; W1 = addr of counter
        ld      w0, [w1]    ; W0 = [W1]
        inc     w0          ; W0 = W0 + 1
        sr      w0, [w1]    ; [W1] = W0
        ; generate decimal ascii string
        lean_w2 decstr      ; w2 = addr of digit buffer
.digit:        
        div     w0, 10, w1  ; w1 = w0 mod 10, w0 = w0 / 10
        add     b2, '0'     ; convert binary 0-9 to ASCII '0'-'9'
        sr      b2, [w2]    ; store character to buffer
        inc     w2          ; increase buffer size
        cmp     w0, 0       ; is it end of number
        jnz     .digit      ; if no then conitnue to decode
.dispdigit:        
        dec     w2          ; browse back the digit buffer
        ld      b0, [w2]    ; get the character
        cmp     b0, 0       ; is it the last character
        jz      .endigit    ; if yes then finish display
.retry:        
        out     b0          ; print out the character
        jz      .retry      ; retry until output is ready
        j       .dispdigit  ; print out next character
.endigit:
        out     10          ; print out \n
        jz      .endigit
        popa
        iret                ; end of interrupt routine
        
; Print out a welcome message
welcome:
        lean_w0 msg
.loop:        
        ld      b2, [w0]
        cmp     b2, 0
        jz      .endmsg
.retry:
        out     b2
        jz      .retry
        inc     w0
        j       .loop
.endmsg:        
        ret

; data area
counter dw      0
        db      0
decstr  db      "00000", 0
isover  db      0
msg     db      "Type space character to stop",10,0
