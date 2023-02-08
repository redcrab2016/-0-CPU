; display RTC clok updated every seconds
; demonstrate timer and input interrupt
; stop program when 'space' character input
; this code do the same thing as timer.asm, but use bs3core.inc
        include                 "bs3core.inc"
        include                 "rtc72421.inc"
        mbs3_bootat             start

start   org                     ebs3_start
        cli
        mbs3_sethandler         int_rtc72421, clock
        ; set the RTC fixed period (register CE), signal mode,
        ;   signal periodicity, enable rtc fixed period signal
        eor     b0, b0
        ; set t1t0 value to 1 second interval
        or      b0, RTC72421_VAL_FP_1SEC
        ; set STND signal (repeat IRQ)
        or      b0, RTC72421_VAL_STND
        ; unset mask (enable fixed period signal)
        or      b0, RTC72421_VAL_MASK_CLEAR 
        ; set CE register (fixed period control register)
        sr      b0, [RTC72421_REG_CE]
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
        jnz     .over       ; if yes then stop
        mov     b0, 1
        sr      b0, [isover]
.over   pop     b0
        iret                ; end of interrupt routine

; macro that output a rtc BCD register
; expect one parameter : the rtc register address
rtc_out macro
        ld      b0, [{1}]
        add     b0, $30
.retry  out     b0
        jz      .retry            
        endm


; Timer handler : count and display
clock:
        pusha
        mbs3_putc   '2'
        mbs3_putc   '0'
        rtc_out     RTC72421_REG_Y10
        rtc_out     RTC72421_REG_Y1
        mbs3_putc   '-'
        rtc_out     RTC72421_REG_MO10
        rtc_out     RTC72421_REG_MO1
        mbs3_putc   '-'
        rtc_out     RTC72421_REG_D10
        rtc_out     RTC72421_REG_D1
        mbs3_putc   ' '
        rtc_out     RTC72421_REG_H10
        rtc_out     RTC72421_REG_H1
        mbs3_putc   ':'
        rtc_out     RTC72421_REG_MI10
        rtc_out     RTC72421_REG_MI1
        mbs3_putc   ':'
        rtc_out     RTC72421_REG_S10
        rtc_out     RTC72421_REG_S1
        mbs3_putc   10
        popa
        iret                ; end of interrupt routine

; data area

isover  db      0
