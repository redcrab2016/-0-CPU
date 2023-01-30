; Hello Ansi example
; An 'hello world' program to test ANSI feature in bs3outansi.inc
        include "bs3core.inc"           ; use bs3 core
        include "bs3ansi.inc"        ; use bs3 print out ANSI 
        mbs3_bootat     start           ; 'start' is where CPU boot

start   org             ebs3_start      ; boot at normal code addr
        ansi_ED         2               ; Clear screen
        ansi_CUP        3, 10           ; cursor row 3 column 10
        ansi_SGR_fg_yellow              ; foreground color Yellow
        ansi_SGR_bg_blue                ; backgound color Blue
        mbs3_printstr "Hello "          ; print'Hello ' yellow on blue
        ansi_SGR_fg_red                 ; foreground color Red
        ansi_SGR_sblink                 ; blink
        mbs3_printstr "ANSI"            ; print blink red 'ANSI'
        ansi_SGR_blinkoff               ; stop blinking
        ansi_CUP        5, 1            ; cursor row 8 column 1
        ansi_SGR_fg_white               ; foreground color White
        ansi_SGR_bg_green               ; backgound color green        
        mbs3_println    msg
        mbs3_println    msg
        mbs3_println    msg
        ansi_SGR_fg_magenta               ; foreground color magenta
        ansi_SGR_bg_magenta               ; backgound color magenta 
        mbs3_println    msg
        ansi_SGR_reset                  ; reset to normal
        ansi_CUP        12, 1           ; cursor row 10 column 1
        hlt                             ; end of Hello World, stop

msg     db              $E2, $96, $80   ; half upper block (UTF8 of $2580)
        db              $E2, $96, $80   ; another half upper block
        db              $E2, $96, $80   ; a third one
        db              ' ',$E2, $96, $80, 0 ; a space and a fourth one
