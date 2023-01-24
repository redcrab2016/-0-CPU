; Hello from ANSI GFX
            include         "bs3core.inc"
            include         "bs3outansi.inc"
            include         "bs3gfxansi.inc"

            mbs3_bootat     start

start       org             ebs3_start
            ansi_ED         2
            call            initgfx
.loop       in              b0
            jz              .loop
            ansi_SGR_reset
            ansi_ED         2
            ansi_CUP        1, 1
            mbs3_printlnstr "bye."
            hlt

initgfx     mbs3gfxcode
            ret
