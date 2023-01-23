; Hello Ansi example
; same as helloworld.asm but use bs3core.inc and bs3outansi.inc
        include "bs3core.inc"           ; use bs3 core
        include "bs3outansi.inc"        ; use bs3 print out ANSI 
        mbs3_bootat     start           ; 'start' is where CPU boot

start   org             ebs3_start      ; boot at normal code addr
        ansi_ED         2               ; Clear screen
        ansi_CUP        3, 10           ; cursor row 3 column 10
        ansi_SGR_fg_yellow              ; foreground color Yellow
        ansi_SGR_bg_blue                ; backgound color Blue
        mbs3_printstr "Hello "          ; print'Hello ' yellow on blue
        ansi_SGR_fg_red                 ; foreground color Yellow
        ansi_SGR_sblink                 ; blink
        mbs3_printstr "ANSI"            ; print blink 'ANSI'
        ansi_SGR_reset                  ; reset to normal
        ansi_CUP        8, 1            ; cursor row 8 column 1
        hlt                             ; end of Hello World, stop
