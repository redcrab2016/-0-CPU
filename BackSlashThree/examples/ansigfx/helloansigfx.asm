; Hello from ANSI GFX
            include         "bs3core.inc"
            include         "bs3ansi.inc"
            include         "bs3ansigfx.inc" ; ANSI gfx lib            

            mbs3_bootat     start

start       ; 
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
            mov             b0, 25 ; xmin
            push            b0
            mov             b0, 25 ; ymin
            push            b0
            mov             b0, 75 ; xmax
            push            b0
            mov             b0, 75 ; ymax
            push            b0
            mov             b0, 10 ; color
            push            b0
            call            lbs3gfxbox
            drop
            drop
            drop
            drop
            drop

            mov             b0, 26 ; xmin
            push            b0
            mov             b0, 26 ; ymin
            push            b0
            mov             b0, 74 ; xmax
            push            b0
            mov             b0, 74 ; ymax
            push            b0
            mov             b0, 12 ; color blue
            push            b0
            call            lbs3gfxfullbox
            drop
            drop
            drop
            drop
            drop

            leaf_w0         hellomsg
            push_w0
            mov             b0, 30 ; x coordinate
            push_b0
            mov             b0, 46 ; y coordinate
            push_b0
            mov             b0, 8 ; color (grey)
            push_b0
            call            lbs3gfxdrawstr
            drop
            drop
            drop
            drop

            leaf_w0         hellomsg
            push_w0
            mov             b0, 29 ; x coordinate
            push_b0
            mov             b0, 45 ; y coordinate
            push_b0
            mov             b0, 15 ; color (white)
            push_b0
            call            lbs3gfxdrawstr
            drop
            drop
            drop
            drop

            call            lbs3gfxshow     ; show screen result

.loop2      in              b0
            jz              .loop2
            ansi_SGR_reset                  ; reset ANSI terminal
            ansi_ED         2               ; clear screen
            ansi_CUP        1, 1            ; set cursor to top left
            mbs3_printlnstr "bye."
            ansi_SCU                        ; show cursor

            hlt
hellomsg    db "Hello world    BackSlashThree",0

