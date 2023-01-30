; based on https://en.wikipedia.org/wiki/ANSI_escape_code
; macro to help to place ANSI code to output

;#######################
;# ANSI Escape sequences
;#######################

ansi_esc    macro
            mbs3_putc   27
            mbs3_putc   {1}
            endm

;    ## Single Shift Two
ansi_SS2    macro    
            ansi_esc    'N'
            endm

;    ##  Single Shift Three
ansi_SS3    macro
            ansi_esc    'O'
            endm

;    ## Device Control String
ansi_DCS    macro
            ansi_esc    'P'
            endm

;    ## Control Sequence Introducer
ansi_CSI    macro
            ansi_esc    '['
            endm

;    ##  String Terminator
ansi_ST     macro
            ansi_esc    '\'
            endm

;    ## Operating System Command
ansi_OSC    macro
            ansi_esc    ']'
            endm

;    ## Start of String
ansi_SOS    macro
            ansi_esc    'X'
            end

;    ## Privacy Message
ansi_PM     macro
            ansi_esc    '^'
            endm

;    ##  Application Program Command
ansi_APC    macro
            ansi_esc    '_'
            endm

;    ## Reset to Initial State
ansi_RIS    macro
            ansi_esc    'c'
            endm

;####################
;# ANSI CSI sequences
;####################

_outByteStr macro
            push        w0
            mov         b0, {1}
            cmp         b0, 100
            jc          .hundreds
            cmp         b0, 10
            jc          .tens 
            j           .units 
.hundreds
            div         b0, 100, b1
            add         b0, '0'
            mbs3_putc   b0
            mov         b0, b1
.tens
            div         b0, 10, b1
            add         b0, '0'
            mbs3_putc   b0
            mov         b0, b1
.units             
            add         b0, '0'
            mbs3_putc   b0
            pop         w0
            endm

;    ## Cursor Up
ansi_CUU    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'A'
            endm

;    ## Cursor Down        
ansi_CUD    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'B'
            endm

;    ## Cursor Forward
ansi_CUF    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'C'
            endm

;    ## Cursor Back
ansi_CUB    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'D'
            endm

;    ## Cursor Next Line
ansi_CNL    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'E'
            endm

;    ## Cursor Previous Line
ansi_CPL    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'F'
            endm

;    ## Cursor Horizontal Absolute
ansi_CHA    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'G'
            endm

;    ## Cursor Position {1}row, {2}column
ansi_CUP    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   ';'
            _outByteStr {2}
            mbs3_putc   'H'
            endm

;    ## Erase Display
;    ## 0 : from cursor up to end of screen
;    ## 1 : from cursor up to begin of screen
;    ## 2 : clear all screen : keep back buffer
;    ## 3 : clear all screen , erase back buffer
ansi_ED     macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'J'
            endm

;    ## Erase in Line, to cursor position change
;    ## 0 : clear from cursor to end of line
;    ## 1 : clear from cursor to begin of line
;    ## 2 : clear all line
ansi_EL     macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'K'
            endm

;    ##  Scroll Up
ansi_SU     macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'S'
            endm

;    ## Scroll Down
ansi_SD     macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'T'
            endm

;    ## same as ansi_CUP : Cursor Position {1} row {2} column
ansi_HVP    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   ';'
            _outByteStr {2}
            mbs3_putc   'f'
            endm

;    ## Select Graphic Endition {1}n
ansi_SGR    macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   'm'
            endm

;    ## Select Graphic Rendition with 2 parameters {1}n, {2}p1, {3}p2
ansi_SGR3   macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   ';'            
            _outByteStr {2}
            mbs3_putc   ';'            
            _outByteStr {3}
            mbs3_putc   'm'
            endm

;    ## Select Graphic Rendition with 4 parameters 
;        {1} n, {2}p1, {3}p2, {4}p3, {5},p4
ansi_SGR5   macro
            ansi_CSI
            _outByteStr {1}
            mbs3_putc   ';'            
            _outByteStr {2}
            mbs3_putc   ';'            
            _outByteStr {3}
            mbs3_putc   ';'            
            _outByteStr {4}
            mbs3_putc   ';'            
            _outByteStr {5}
            mbs3_putc   'm'
            endm

;    ## AUX Port On
ansi_AUXportOn  macro
                ansi_CSI
                mbs3_putc   '5'
                mbs3_putc   'i'
                endm

;    ## AUX Port Off
ansi_AUXportOff macro
                ansi_CSI
                mbs3_putc   '4'
                mbs3_putc   'i'
                endm

;    ## Device Status Report
ansi_DSR        macro
                ansi_CSI
                mbs3_putc   '6'
                mbs3_putc   'n'
                endm

;    ## Save Cursor Position
ansi_SCP    macro
            ansi_CSI
            mbs3_putc   's'
            endm
    
;    ## Restore Cursor Position
ansi_RCP    macro
            ansi_CSI
            mbs3_putc   'u'
            endm

;    ## show cursor
ansi_SCU    macro
            ansi_CSI
            mbs3_putc   '?'
            mbs3_putc   '2'
            mbs3_putc   '5'
            mbs3_putc   'h'
            endm

;    ## hide cursor
ansi_HCU    macro
            ansi_CSI
            mbs3_putc   '?'
            mbs3_putc   '2'
            mbs3_putc   '5'
            mbs3_putc   'l'
            endm

;    ## enable alternate screen buffer
ansi_EASB   macro
            ansi_CSI
            mbs3_putc   '?'
            mbs3_putc   '1'
            mbs3_putc   '0'
            mbs3_putc   '4'
            mbs3_putc   '9'
            mbs3_putc   'h'
            endm

;    ## disable alternate screen buffer
ansi_DASB   macro
            ansi_CSI
            mbs3_putc   '?'
            mbs3_putc   '1'
            mbs3_putc   '0'
            mbs3_putc   '4'
            mbs3_putc   '9'
            mbs3_putc   'l'
            endm

;    ## Turn on bracketed paste mode 
ansi_BPMon  macro
            ansi_CSI
            mbs3_putc   '?'
            mbs3_putc   '2'
            mbs3_putc   '0'
            mbs3_putc   '0'
            mbs3_putc   '4'
            mbs3_putc   'h'
            endm

;    ## Turn off bracketed paste mode 
ansi_BPMoff macro
            ansi_CSI
            mbs3_putc   '?'
            mbs3_putc   '2'
            mbs3_putc   '0'
            mbs3_putc   '0'
            mbs3_putc   '4'
            mbs3_putc   'l'
            endm

;#####################################
;# ANSI SGR (Select Graphic Rendition)
;#####################################

;    ## Reset / Normal
ansi_SGR_reset  macro
                ansi_SGR    0
                endm

;    ## Bold or increased intensity
ansi_SGR_bold   macro
                ansi_SGR    1
                endm

;    ## Faint (decreased intensity)
ansi_SGR_faint  macro
                ansi_SGR    2
                endm

;    ## Italic
ansi_SGR_italic macro
                ansi_SGR    3
                endm

;    ## Underline
ansi_SGR_underline  macro
                    ansi_SGR    4
                    endm

;    ## Slow Blink
ansi_SGR_sblink     macro
                    ansi_SGR    5
                    endm
    
;    ## Rapid Blink
ansi_SGR_rblink macro
                ansi_SGR    6    
                endm

;    ## Reverse Video
ansi_SGR_reverse    macro
                    ansi_SGR    7
                    endm

;    ## Conceal
ansi_SGR_conceal    macro
                    ansi_SGR    8    
                    endm

;    ## Crossed-out
ansi_SGR_crossout   macro
                    ansi_SGR    9    
                    endm

;    ## primary font
ansi_SGR_defaultfont    macro
                        ansi_SGR    10
                        endm

;    ## alternate font (1 to 9) , 0 same as primary
ansi_SGR_font   macro
                push        b0
                mov         b0, {1}
                add         b0, 10
                ansi_SGR    b0
                pop         b0   
                endm

;    ## Fraktur
ansi_SGR_fraktur    macro
                    ansi_SGR    20
                    endm

;    ## Bold off or Double Underline
ansi_SGR_doubleunderline    macro
                            ansi_SGR    21
                            endm

;    ## Normal color or intensity (neither bold nor faint)
ansi_SGR_normalcolorintensity   macro
                                ansi_SGR    22
                                endm

;    ## Not italic nor fraktur
ansi_SGR_italicfrakturoff       macro
                                ansi_SGR    23
                                endm

;    ## Underline off
ansi_SGR_underlineoff           macro
                                ansi_SGR    24
                                endm

;    ## blink off
ansi_SGR_blinkoff   macro
                    ansi_SGR    25
                    endm

;    ## Inverse off
ansi_SGR_inverseoff macro
                    ansi_SGR    27
                    endm

;    ## Reveal / conceal off
ansi_SGR_concealoff macro
                    ansi_SGR    28
                    endm

;    ## Not cross-out
ansi_SGR_crossoutoff    macro
                        ansi_SGR    29
                        endm

;    ## Set foreground color 0 to 7  n {1}
ansi_SGR_fgcolor    macro
                    push        b0
                    mov         b0, {1}
                    add         b0, 30
                    ansi_SGR    b0
                    pop         b0
                    endm

;    ## set foreground color , palette on 256 colors n {1}
ansi_SGR_fg256  macro
                ansi_SGR3   38, 5,  {1}
                endm

;    ## set foreground color, RGB 24 bits (3* 256 colors, RGB)
;        r {1}, g {2}, b {3}
ansi_SGR_fgRGB  macro
                ansi_SGR5   38, 2,  {1}, {2}, {3}    
                endm

;    ## set default foreground color
ansi_SGR_fgdefault  macro
                    ansi_SGR    39
                    endm

;    ## Set background color 0 to 7 n{1}
ansi_SGR_bgcolor    macro
                    push        b0
                    mov         b0, {1}
                    add         b0, 40
                    ansi_SGR    b0
                    pop         b0
                    endm

;    ## set background color , palette on 256 colors n{1}
ansi_SGR_bg256  macro
                ansi_SGR3   48, 5, {1}
                endm

;    ## set background color, RGB 24 bits (3* 256 colors, RGB)
;       r{1}, g{2}, b{3}
ansi_SGR_bgRGB  macro
                ansi_SGR5   48, 2, {1}, {2}, {3}    
                endm

;    ## set default background color
ansi_SGR_bgdefault  macro
                    ansi_SGR    49
                    endm

;    ## Framed
ansi_SGR_framed macro
                ansi_SGR    51    
                endm

;    ## Encircled   
ansi_SGR_encircled  macro
                    ansi_SGR    52
                    endm

;    ## Overlined
ansi_SGR_overlined  macro
                    ansi_SGR    53
                    endm

;    ## Not framed or encirlced
ansi_SGR_framedoff  macro
                    ansi_SGR    54
                    endm

ansi_SGR_encircledoff   macro
                        ansi_SGR    54
                        endm

;    ## Not overlined
ansi_SGR_overlinedoff   macro
                        ansi_SGR    55
                        endm

;    ## Foreground Black
ansi_SGR_fg_black   macro
                    ansi_SGR_fgcolor    0
                    endm

;    ## Foreground Red
ansi_SGR_fg_red macro
                ansi_SGR_fgcolor    1
                endm

;    ## Foreground Green
ansi_SGR_fg_green   macro
                    ansi_SGR_fgcolor    2
                    endm

;    ## Foreground Yellow
ansi_SGR_fg_yellow  macro
                    ansi_SGR_fgcolor    3
                    endm

;    ## Foreground Blue
ansi_SGR_fg_blue    macro
                    ansi_SGR_fgcolor    4
                    endm

;    ## Foreground Magenta
ansi_SGR_fg_magenta macro
                    ansi_SGR_fgcolor    5
                    endm

;    ## Foreground Cyan
ansi_SGR_fg_cyan    macro
                    ansi_SGR_fgcolor    6
                    endm

;    ## Foreground White
ansi_SGR_fg_white   macro
                    ansi_SGR_fgcolor    7
                    endm
    
;    ## Background Black
ansi_SGR_bg_black   macro
                    ansi_SGR_bgcolor    0
                    endm

;    ## Background Red
ansi_SGR_bg_red     macro
                    ansi_SGR_bgcolor    1
                    endm

;    ## Background Green
ansi_SGR_bg_green   macro
                    ansi_SGR_bgcolor    2
                    endm

;    ## Background Yellow
ansi_SGR_bg_yellow  macro
                    ansi_SGR_bgcolor    3
                    endm

;    ## Background Blue
ansi_SGR_bg_blue    macro
                    ansi_SGR_bgcolor    4
                    endm    

;    ## Background Magenta
ansi_SGR_bg_magenta macro
                    ansi_SGR_bgcolor    5
                    endm

;    ## Background Cyan
ansi_SGR_bg_cyan    macro
                    ansi_SGR_bgcolor    6
                    endm

;    ## Background White
ansi_SGR_bg_white   macro
                    ansi_SGR_bgcolor    7
                    endm
