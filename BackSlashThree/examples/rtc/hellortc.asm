; display RTC clok updated every seconds
; demonstrate RTC timer info and interrupt
; demonstrate the stadard input interrupt
; stop program when 'space' character input
; this code do the same thing as timer.asm, but use bs3core.inc
        include                 "bs3core.inc"
        include                 "rtc72421.inc"
        mbs3_bootat             start

start   org                     ebs3_start
        cli
        mbs3_sethandler         int_rtc72421, RTC_clock
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

; RTC handler: display 'day_of_week year-month-day hour:minute:second'
RTC_clock:
        pusha
        ; print day of week
        ;   get day of week from RTC
        eor         w3, w3
        ld          b6, [RTC72421_REG_W]
        ;   multiply by two to have week array index
        shl         b6
        ;   get day of week string address
        leaf        w2, weekarray
        ld          w2,[w2 + w3]
        ;   print day of week string
        mbs3_print  w2
        ; print ' ' separator
        mbs3_putc   ' '
        ; print year with century
        mbs3_putc   '2'
        mbs3_putc   '0'
        rtc_out     RTC72421_REG_Y10
        rtc_out     RTC72421_REG_Y1
        ; print '-' separator
        mbs3_putc   '-'
        ; print month
        rtc_out     RTC72421_REG_MO10
        rtc_out     RTC72421_REG_MO1
        ; print '-' separator
        mbs3_putc   '-'
        ; print day of month
        rtc_out     RTC72421_REG_D10
        rtc_out     RTC72421_REG_D1
        ; print ' ' separator to time
        mbs3_putc   ' '
        ; print Hour 
        rtc_out     RTC72421_REG_H10
        rtc_out     RTC72421_REG_H1
        ; print ':' separator
        mbs3_putc   ':'
        ; print Minutes of the hour
        rtc_out     RTC72421_REG_MI10
        rtc_out     RTC72421_REG_MI1
        ; print ':' separator
        mbs3_putc   ':'
        ; print Second of the minute
        rtc_out     RTC72421_REG_S10
        rtc_out     RTC72421_REG_S1
        ; print new line
        mbs3_putc   10
        popa
        iret                ; end of interrupt routine

; data area

isover          db      0
weekarray       dw      sunday, monday,tuesday,wednesday
                dw      thursday,friday,saturday
sunday          db      "Sunday",0
monday          db      "Monday",0
tuesday         db      "Tuesday",0
wednesday       db      "Wednesday",0
thursday        db      "Thursday",0
friday          db      "Friday",0
saturday        db      "Saturday",0
