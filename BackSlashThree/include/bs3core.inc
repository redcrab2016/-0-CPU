; ==== BS3 common
; == BS3 common values
ebs3int0                equ     $0000
ebs3int1                equ     $0002
ebs3int2                equ     $0004
ebs3int3                equ     $0006
ebs3int4                equ     $0008
ebs3int5                equ     $000A
ebs3int6                equ     $000C
ebs3int7                equ     $000E
ebs3int8                equ     $0010
ebs3int9                equ     $0012
ebs3int10               equ     $0014
ebs3int11               equ     $0016
ebs3int12               equ     $0018
ebs3int13               equ     $001A
ebs3int14               equ     $001C
ebs3int15               equ     $001E
ebs3_default_stack      equ     $0400
ebs3_input              equ     $0100
ebs3_input_status       equ     $0101
ebs3_input_status_ready equ     $00
ebs3_input_status_wait  equ     $01
ebs3_input_status_none  equ     $FF
ebs3_output             equ     $0102
ebs3_output_status      equ     $0103
ebs3_output_status_ready equ    $00
ebs3_output_status_wait  equ    $01
ebs3_output_status_none  equ    $FF
ebs3_output_aux         equ     $0104
ebs3_output_aux_status  equ     $0105
ebs3_timer_low16        equ     $0108
ebs3_timer_high16       equ     $010A
ebs3_timer_mode         equ     $010C
ebs3_timer_mode_time    equ     $00
ebs3_timer_mode_tick    equ     $01

; == BS3 common addresses
;   interruption handler addresses
lbs3int0                org     ebs3int0
lbs3int_boot
lbs3int1                org     ebs3int1
lbs3int2                org     ebs3int2
lbs3int_hypevent
lbs3int3                org     ebs3int3
lbs3int_timer
lbs3int4                org     ebs3int4
lbs3int_byteinput
lbs3int5                org     ebs3int5
lbs3int_blockinput
lbs3int6                org     ebs3int6
lbs3int_divzero
lbs3int7                org     ebs3int7
lbs3int_badinstr
lbs3int8                org     ebs3int8
lbs3int9                org     ebs3int9
lbs3int10               org     ebs3int10
lbs3int11               org     ebs3int11
lbs3int12               org     ebs3int12
lbs3int13               org     ebs3int13
lbs3int14               org     ebs3int14
lbs3int15               org     ebs3int15

;   Hypervisor data block receiver exchange memory
lbs3_blk_status         org     $0020
lbs3_blk_input_address  org     $0022
lbs3_blk_input_length   org     $0024

;   Hypervisor Event data
lbs3_event_id           org     $0030
lbs3_event_data         org     $0032

;   Core I/O
lbs3_input              org     ebs3_input
lbs3_input_status       org     ebs3_input_status
lbs3_output             org     ebs3_output
lbs3_output_status      org     ebs3_output_status
lbs3_output_aux         org     ebs3_output_aux
lbs3_output_aux_status  org     ebs3_output_aux_status
lbs3_timer_low16        org     ebs3_timer_low16
lbs3_timer_high16       org     ebs3_timer_high16
lbs3_timer_mode         org     ebs3_timer_mode

;   Misc
lbs3_default_stack      org     ebs3_default_stack

; == BS3 macros

;   Set BS3 boot address,
;   to be use only once per program before first code
;    or after all code ('code' represent your program)
;   expect 1 parameter:
;     1) global label/decimal/hexadecimal 
;        representing the boot address
;   example: mbs3_bootat myprogram
mbs3_bootat             macro
                        org     ebs3int0
                        dw      {1}
                        endm

;   Set an handler to an interrupt vector entry
;   expect 2 parameters:
;     1) interrupt reference 
;     2) global label/address/w0-3 representing the handler address
;   if interrupt ref is xxx then lbs3xxx label name is used
;   example: mbs3_sethandler int_timer myTimerHandler
mbs3_sethandler  macro
                        push_w0
                        mov     w0, {2}
                        sr      w0, [lbs3{1}]
                        pop_w0
                        endm

;   Save current handler address of an interrupt vetor entry
;   expect 2 parameters:
;     1) interrupt reference
;     2) label/address/W1-3 representing the address location that
;        will contain the handler address
;   if interrupt reference is xxx then lbs3xxx label name is used
;   example: mbs3_savehandler int_timer oldHandlerAddr
mbs3_savehandler macro
                        push_w0
                        ld      w0, [lbs3{1}]
                        sr      w0, [{2}]
                        pop_w0
                        endm

;   Restore handler address of an interrupt vector entry
;   expect 2 parameters:
;     1) interrupt reference
;     2) label/address/W0-3 representing the address location that
;        contain the handler address to set
;   if interrupt reference is xxx then lbs3xxx label name is used
;   example: mbs3_restorehandler int_timer oldHandlerAddr
mbs3_restorehandler macro
                        push_w0
                        ld      w0, [{2}]
                        sr      w0, [lbs3{1}]
                        pop_w0
                        endm

;   Set timer mode
;   expect 1 parameter:
;     1) ebs3_timer_mode_tick or ebs3_timer_mode_time
;   example: mbs3_settimermode ebs3_timer_mode_time
mbs3_settimermode     macro
                        push_b0
                        mov     b0, {1}
                        sr      b0, [lbs3_timer_mode]
                        pop_b0
                        endm

;   Start timer, interrupt 3 invoked at regular interval
;   expect 2 parameters:  32 bits (2x16bits) value in microseconds
;     1) 16 bits value representing MSW 32 bits (decimal/hexa/w0-3)
;     2) 16 bits value representing LSW 32 bits (decimal/hexa/w0-3)
;   example: mbs3_starttimer $000F $4240 ; 1 second interval
;            mbs3_starttimer w0 w1 ; w0-w1 as a 32 bits value
mbs3_starttimer        macro
                        pushf
                        cli
                        push_w0
                        mov     w0, {2}
                        sr      w0, [lbs3_timer_low16]
                        pop_w0
                        push_w0
                        mov     w0, {1}
                        sr      w0, [lbs3_timer_high16]
                        pop_w0
                        popf
                        endm

;   Stop timer, no more interrupt 3 invocation
;   no parameter expected
mbs3_stoptimer         macro
                        mbs3_start_timer 0 0
                        endm

;   Print a character 
;   expect 1 parameter: (byte compatible immediate or Bx register)
;      1) decimal/hexadecimal/character/B0-7 as character to print
;   example: mbs3_putc 'A'
mbs3_putc               macro
.retry                  out     {1}
                        jz      .retry
                        endm

;   Print a string character located at an address
;   expect 1 parameter:
;   1) decimal/hexadecimal, global label or Wx register
;      representing the address of the character string
;      the string must be terminated by a NULL byte (0)
;   NB: register w0 ad b2 are modified
;   example: mbs3_print_ welcomeMessage
mbs3_print_             macro
                        mov         w0, {1}
.loop                   ld          b2, [w0]
                        cmp         b2, 0
                        jz          .end
                        mbs3_putc   b2
                        inc         w0
                        j           .loop
.end
                        endm

;   Print a string character located at an address
;   expect 1 parameter:
;   1) decimal/hexadecimal, global label or Wx register
;      representing the address of the character string
;      the string must be terminated by a NULL byte (0)
;   example: mbs3_print welcomeMessage
mbs3_print              macro
                        push_w0
                        push_b2
                        mbs3_print_ {1}
                        pop_b2
                        pop_w0
                        endm

;   Print a string character located at an address
;   expect 1 parameter:
;   1) decimal/hexadecimal, global label or Wx register
;      representing the address of the character string
;      the string must be terminated by a NULL byte (0)
;  NB: print out also a line feed '\n' afterward
;   example: mbs3_println welcomeMessage
mbs3_println            macro
                        mbs3_print  {1}
                        mbs3_putc   10
                        endm

;   Print a static string passed as parameter
;   usefull if the message is use only once in the program
;   expect 1 parameter
;   1) static string 
;   example: mbs3_print_static "Hello world."
mbs3_printstr           macro
                        j           .code
.msg db {1}, 0
.code                   push_w0
                        push_b2
                        lean_w0     .msg
                        mbs3_print_ w0
                        pop_b2
                        pop_w0
                        endm 

;   Print a static string passed as parameter
;   usefull if you print a message only once in your code
;   expect 1 parameter
;   1) static string 
;  NB: print out also a line feed '\n' afterward
;   example: mbs3_print_static "Hello world."
mbs3_printlnstr      macro
 mbs3_printstr {1}
                        mbs3_putc           10
                        endm
                        