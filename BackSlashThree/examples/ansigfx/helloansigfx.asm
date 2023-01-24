; Hello from ANSI GFX
            include         "bs3core.inc"
            include         "bs3outansi.inc"
            include         "bs3gfxansi.inc"

            mbs3_bootat     start

start       org             ebs3_start
            ansi_ED         2
            call            initgfx
.loop1      in              b0
            jz              .loop1
            mov             b0, 10 ; clear screen in green
            call            lbs3gfxclear
            call            lbs3gfxshow   
.loop2      in              b0
            jz              .loop2
            ansi_SGR_reset
            ansi_ED         2
            ansi_CUP        1, 1
            mbs3_printlnstr "bye."
            hlt

initgfx     mbs3gfxcode
            ret
