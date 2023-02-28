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
ebs3_start              equ     $0400
ebs3_tinystart          equ     $0300
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
ebs3_bank_rom           equ     $0106
ebs3_bank_ram           equ     $0107
ebs3_timer_low16        equ     $0108
ebs3_timer_high16       equ     $010A
ebs3_timer_mode         equ     $010C
ebs3_timer_mode_time    equ     $00
ebs3_timer_mode_tick    equ     $01
ebs3_irq_queue          equ     $010E
ebs3_gfx_cmd            equ     $0120
ebs3_gfx_cmd_reset      equ     $00
ebs3_gfx_cmd_end        equ     $FF
ebs3_gfx_cmd_refresh    equ     $01
ebs3_gfx_cmd_vpconfig   equ     $02
ebs3_gfx_cmd_vpclear    equ     $03
ebs3_gfx_cmd_getpixel   equ     $04
ebs3_gfx_cmd_setpixel   equ     $05
ebs3_gfx_cmd_hline      equ     $06
ebs3_gfx_cmd_vline      equ     $07
ebs3_gfx_cmd_box        equ     $08
ebs3_gfx_cmd_boxfull    equ     $09
ebs3_gfx_cmd_blitope    equ     $0A
ebs3_gfx_cmd_blitkcolor equ     $0B
ebs3_gfx_cmd_blitrans   equ     $0C
ebs3_gfx_status         equ     $0120
ebs3_gfx_status_busy    equ     $80
ebs3_gfx_status_waitdata equ    $40
ebs3_gfx_status_enable  equ     $20
ebs3_gfx_status_code    equ     $1F
ebs3_gfx_code_ok        equ     $00
ebs3_gfx_code_disabled  equ     $01
ebs3_gfx_code_badsize   equ     $02
ebs3_gfx_code_badsurface equ    $03
ebs3_gfx_code_badope    equ     $04
ebs3_gfx_blit_ope_copy  equ     $00
ebs3_gfx_blit_ope_or    equ     $01
ebs3_gfx_blit_ope_eor   equ     $02
ebs3_gfx_blit_ope_and   equ     $03
ebs3_gfx_blit_ope_add   equ     $04
ebs3_gfx_blit_ope_sub   equ     $05
ebs3_gfx_blit_ope_mul   equ     $06

ebs3_gfx_reg1           equ     $0121
ebs3_gfx_reg2           equ     $0122
ebs3_gfx_reg3           equ     $0123
ebs3_gfx_reg4           equ     $0124
ebs3_gfx_reg5           equ     $0125
ebs3_gfx_reg6           equ     $0126
ebs3_gfx_reg7           equ     $0127

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
lbs3int_rtc72421
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
lbs3_bank_rom           org     ebs3_bank_rom
lbs3_bank_ram           org     ebs3_bank_ram
lbs3_timer_low16        org     ebs3_timer_low16
lbs3_timer_high16       org     ebs3_timer_high16
lbs3_timer_mode         org     ebs3_timer_mode
lbs3_irq_queue          org     ebs3_irq_queue

;  GFX I/O
lbs3_gfx_cmd            org     ebs3_gfx_cmd
lbs3_gfx_status
lbs3_gfx_reg1           org     ebs3_gfx_reg1
lbs3_gfx_reg2           org     ebs3_gfx_reg2
lbs3_gfx_reg3           org     ebs3_gfx_reg3
lbs3_gfx_reg4           org     ebs3_gfx_reg4
lbs3_gfx_reg5           org     ebs3_gfx_reg5
lbs3_gfx_reg6           org     ebs3_gfx_reg6
lbs3_gfx_reg7           org     ebs3_gfx_reg7
lbs3_gfx_autorefresh


;   Misc
lbs3_default_stack      org     ebs3_default_stack
                        org             ebs3_start
; == BS3 macros

;   Set BS3 boot address,
;   to be use only once per program before first code
;    or after all code ('code' represent your program)
;   expect 1 parameter:
;     1) global label/decimal/hexadecimal 
;        representing the boot address
;   example: mbs3_bootat myprogram
mbs3_bootat     macro
_beforeboot_    equ     $
        
                org     ebs3int0
                dw      {1}
                org     _beforeboot_
                endm

;   BS3 CPU macro for push_X
;   expect 1 parameter:
;     1) byte register B0-7 or word register W0-3
push    macro
        push_{1}
        endm

;   BS3 CPU macro for pop_X
;   expect 1 parameter:
;      1) byte register B0-7 or word register W0-3
pop     macro
        pop_{1}
        endm

;   BS3 CPU macro for multi drop
drop2   macro
        drop
        drop
        endm

drop3   macro
        drop
        drop
        drop
        endm

drop4   macro
        drop
        drop
        drop
        drop
        endm

drop5   macro
        drop
        drop
        drop
        drop
        endm

;   BS3 CPU push an immediate value or label address
pushi   macro
        j       .over
.word   dw      0
.over
        sr      w0, [.word]
        mov     w0, {1}
        push    w0
        ld      w0, [.word]
        endm

pushi2  macro ; pushi {1}, pushi {2}
        pushi   {1}
        pushi   {2}
        endm

pushi3  macro ; pushi {1}, pushi {2} & {3}
        pushi   {1}
        pushi   {2}
        pushi   {3}
        endm

pushi4  macro ; pushi {1}, pushi {2} ..{4}
        pushi   {1}
        pushi   {2}
        pushi   {3}
        pushi   {4}
        endm

pushi5  macro ; pushi {1}, pushi {2} ..{5}
        pushi   {1}
        pushi   {2}
        pushi   {3}
        pushi   {4}
        pushi   {5}
        endm


;   BS3 CPU macro for lean_X, 
;   expect 2 parameters:
;     1) register W0-3
;     2) a global label (not local label : starting with a '.' )
lean    macro
        lean_{1} {2}
        endm

;   BS3 CPU macro for leaf_X
;   expect 2 parameters:
;     1) register W0-3
;     2) a global label (not local label : starting with a '.' )
leaf    macro
        leaf_{1} {2}
        endm

; unsigned byte variable forward(increase) loop 
;     1) global label of the byte variable
;     2) immediate byte value
;     3) immediate byte last value
startfwbloop macro ; {1} varname, {2} firstvalue, {3} lastvalue
                mov     b0, {2}
_loopstart_{1}                
                sr      b0, [{1}]
                cmp     b0, {3}
                jbe     .l1
                jump    _loopend_{1}
.l1                
                endm 

endfwbloop   macro ; {1} varname
                ld      b0, [{1}]
                inc     b0
                jump    _loopstart_{1}
_loopend_{1}
                endm

; unsigned word variable forward(increase) loop 
;     1) global label of the word variable
;     2) immediate word value
;     3) immediate word last value
startfwwloop macro ; {1} varname, {2} firstvalue, {3} lastvalue
                mov     w0, {2}
_loopstart_{1}                
                sr      w0, [{1}]
                cmp     w0, {3}
                jbe     .l1
                jump    _loopend_{1}
.l1                
                endm 

endfwwloop   macro ; {1} varname
                ld      w0, [{1}]
                inc     w0
                jump    _loopstart_{1}
_loopend_{1}
                endm


;   Set an handler to an interrupt vector entry
;   expect 2 parameters:
;     1) interrupt reference 
;     2) global label/address/w0-3 representing the handler address
;   if interrupt ref is xxx then lbs3xxx label name is used
;   example: mbs3_sethandler int_timer myTimerHandler
mbs3_sethandler macro
                push    w0
                mov     w0, {2}
                sr      w0, [lbs3{1}]
                pop     w0
                endm

;   Save current handler address of an interrupt vetor entry
;   expect 2 parameters:
;     1) interrupt reference
;     2) label/address/W1-3 representing the address location that
;        will contain the handler address
;   if interrupt reference is xxx then lbs3xxx label name is used
;   example: mbs3_savehandler int_timer oldHandlerAddr
mbs3_savehandler macro
                 push    w0
                 ld      w0, [lbs3{1}]
                 sr      w0, [{2}]
                 pop     w0
                 endm

;   Restore handler address of an interrupt vector entry
;   expect 2 parameters:
;     1) interrupt reference
;     2) label/address/W0-3 representing the address location that
;        contain the handler address to set
;   if interrupt reference is xxx then lbs3xxx label name is used
;   example: mbs3_restorehandler int_timer oldHandlerAddr
mbs3_restorehandler macro
                    push    w0
                    ld      w0, [{2}]
                    sr      w0, [lbs3{1}]
                    pop     w0
                    endm

;   Set timer mode
;   expect 1 parameter:
;     1) ebs3_timer_mode_tick or ebs3_timer_mode_time
;   example: mbs3_settimermode ebs3_timer_mode_time
mbs3_settimermode   macro
                    push    b0
                    mov     b0, {1}
                    sr      b0, [lbs3_timer_mode]
                    pop     b0
                    endm

;   Start timer, interrupt 3 invoked at regular interval
;   expect 2 parameters:  32 bits (2x16bits) value in microseconds
;     1) 16 bits value representing MSW 32 bits (decimal/hexa/w0-3)
;     2) 16 bits value representing LSW 32 bits (decimal/hexa/w0-3)
;   example: mbs3_starttimer $000F $4240 ; 1 second interval
;            mbs3_starttimer w0 w1 ; w0-w1 as a 32 bits value
mbs3_starttimer macro
                pushf
                cli
                push    w0
                mov     w0, {2}
                sr      w0, [lbs3_timer_low16]
                pop     w0
                push    w0
                mov     w0, {1}
                sr      w0, [lbs3_timer_high16]
                pop     w0
                popf
                endm

;   Stop timer, no more interrupt 3 invocation
;   no parameter expected
mbs3_stoptimer  macro
                mbs3_start_timer 0 0
                endm

;   Print a character 
;   expect 1 parameter: (byte compatible immediate or Bx register)
;      1) decimal/hexadecimal/character/B0-7 as character to print
;   example: mbs3_putc 'A'
mbs3_putc   macro
.retry      out     {1}
            jz      .retry
            endm

;   Print a string character located at an address
;   expect 1 parameter:
;   1) decimal/hexadecimal, global label or Wx register
;      representing the address of the character string
;      the string must be terminated by a NULL byte (0)
;   NB: register w0 ad b2 are modified
;   example: mbs3_print_ welcomeMessage
mbs3_print_ macro
            mov         w0, {1}
.loop       ld          b2, [w0]
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
mbs3_print  macro
            push        w0
            push        b2
            mbs3_print_ {1}
            pop         b2
            pop         w0
            endm

;   Print a string character located at an address
;   expect 1 parameter:
;   1) decimal/hexadecimal, global label or Wx register
;      representing the address of the character string
;      the string must be terminated by a NULL byte (0)
;  NB: print out also a line feed '\n' afterward
;   example: mbs3_println welcomeMessage
mbs3_println    macro
                mbs3_print  {1}
                mbs3_putc   10
                endm

;   Print a static string passed as parameter
;   usefull if the message is use only once in the program
;   expect 1 parameter
;   1) static string 
;   example: mbs3_printstr "Hello world."
mbs3_printstr   macro
                j           .code
.msg db {1}, 0
.code           push        w0
                push        b2
                lean_w0     .msg
                mbs3_print_ w0
                pop         b2
                pop         w0
                endm 

;   Print a static string passed as parameter
;   usefull if you print a message only once in your code
;   expect 1 parameter
;   1) static string 
;  NB: print out also a line feed '\n' afterward
;   example: mbs3_printlnstr "Hello world."
mbs3_printlnstr macro
 mbs3_printstr {1}
                mbs3_putc   10
                endm

