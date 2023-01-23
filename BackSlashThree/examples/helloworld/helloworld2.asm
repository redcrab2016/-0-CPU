; Hello World example
; same as helloworld.asm but use bs3core.inc
        include "bs3core.inc"           ; use bs3 core 
        mbs3_bootat     start           ; 'start' is where CPU boot

start   org             ebs3_start      ; boot at normal code addr
        mbs3_printlnstr "Hello world."  ; print out the 'Hello world'
        hlt                             ; end of Hello World, stop
