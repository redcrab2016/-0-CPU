; Hello from ANSI GFX
            include         "bs3core.inc"
            include         "bs3outansi.inc"
            

            mbs3_bootat     start

start       org             ebs3_start
            ansi_ED         2               ; clear screen
            ansi_HCU                        ; hide cursor
            call            lbs3gfxinit     ; init ANSI gfx
.loop1      in              b0
            jz              .loop1
            mov             b0, 1           ; 
            call            lbs3gfxclear    ; clear screen in red
            call            lbs3gfxshow     ; show screen result
.loop3      in              b0
            jz              .loop3
            mov             b0, 50

.pixelloop  push            b0
            cmp             b0, $FF
            jz              .endpixelloop
            push            b0              ; push parameter x=b0
            dup                             ; push parameter y=b0
            mov             b0, 14          
            push            b0              ; push parameter color=14
            call            lbs3gfxpixel    ; call set pixel routine
            drop                            ; drop color parameter
            drop                            ; drop y parameter
            drop                            ; drop x parameter
            call            lbs3gfxshow     ; show screen result
            pop             b0
            dec             b0              ; decrement b0 for coords.
            j               .pixelloop
.endpixelloop            
            call            lbs3gfxshow     ; show screen result

.loop2      in              b0
            jz              .loop2
            ansi_SGR_reset                  ; reset ANSI terminal
            ansi_ED         2               ; clear screen
            ansi_CUP        1, 1            ; set cursor to top left
            mbs3_printlnstr "bye."
            ansi_SCU                        ; show cursor

            hlt

            include         "bs3gfxansi.asm" ; ANSI gfx lib
