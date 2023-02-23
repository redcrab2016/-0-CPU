/***
 *        ___    _   _______ ____
 *       /   |  / | / / ___//  _/
 *      / /| | /  |/ /\__ \ / /  
 *     / ___ |/ /|  /___/ // /   
 *    /_/ _|_/_/_|_//____/___/   
 *       / ____/ ___// ____/     
 *      / __/  \__ \/ /          
 *     / /___ ___/ / /___        
 *    /_____//____/\____/        
 *
 * terminal ANSI macro commands 
 *
 */
#ifndef _ANSI_ESC_H
#define _ANSI_ESC_H         1
#define _out(c)             putchar((int)(c))
#define _outStr(str)        fprintf(stdout,(str))
#define _outFlush()         fflush(stdout)
#define _outByteStr(x)      fprintf(stdout,"%d",(int)(x))
#define ansi_esc(e)         _out(27);_out(e)

/*    ## Single Shift Two                                               */
#define _bs3_ansi_SS2()     ansi_esc('N')

/*    ##  Single Shift Three                                            */
#define ansi_SS3()          ansi_esc('O')

/*    ## Device Control String                                          */
#define ansi_DCS()          ansi_esc('P')

/*    ## Control Sequence Introducer                                    */
#define ansi_CSI()          ansi_esc('[')

/*    ##  String Terminator                                             */
#define ansi_ST()           ansi_esc('\\')

/*    ## Operating System Command                                       */
#define ansi_OSC()          ansi_esc(']')

/*    ## Start of String                                                */
#define ansi_SOS()          ansi_esc('X')

/*    ## Privacy Message                                                */
#define ansi_PM()           ansi_esc('^')

/*    ##  Application Program Command                                   */
#define ansi_APC()          ansi_esc('_')

/*    ## Reset to Initial State                                         */
#define ansi_RIS()          ansi_esc('c')

/*####################
  # ANSI CSI sequences
  ####################                                                  */


/*    ## Cursor Up                                                      */
#define ansi_CUU(x)         ansi_CSI();_outByteStr(x);_out('A')

/*    ## Cursor Down                                                    */
#define ansi_CUD(x)         ansi_CSI();if ((x) > 1) _outByteStr(x);_out('B')

/*    ## Cursor Forward                                                 */
#define ansi_CUF(x)         ansi_CSI();_outByteStr(x);_out('C')

/*    ## Cursor Back                                                    */
#define ansi_CUB(x)         ansi_CSI();_outByteStr(x);_out('D')

/*    ## Cursor Next Line                                               */
#define ansi_CNL(x)         ansi_CSI();_outByteStr(x);_out('E')

/*    ## Cursor Previous Line                                           */
#define ansi_CPL(x)         ansi_CSI();_outByteStr(x);_out('F')

/*    ## Cursor Horizontal Absolute                                     */
#define ansi_CHA(x)         ansi_CSI();_outByteStr(x);_out('G')

/*    ## Cursor Position {1}row, {2}column                              */
#define ansi_CUP(y,x)       ansi_CSI();_outByteStr(y);_out(';');_outByteStr(x);_out('H')

/*   ## Erase Display
     ## 0 : from cursor up to end of screen
     ## 1 : from cursor up to begin of screen
     ## 2 : clear all screen : keep back buffer
     ## 3 : clear all screen , erase back buffer                        */
#define ansi_ED(x)          ansi_CSI();_outByteStr(x);_out('J')

/*   ## Erase in Line, to cursor position change
     ## 0 : clear from cursor to end of line
     ## 1 : clear from cursor to begin of line
     ## 2 : clear all line                                              */
#define ansi_EL(x)          ansi_CSI();_outByteStr(x);_out('K')

/*   ##  Scroll Up                                                      */
#define ansi_SU(x)          ansi_CSI();_outByteStr(x);_out('S')

/*   ## Scroll Down                                                     */
#define ansi_SD(x)          ansi_CSI();_outByteStr(x);_out('T')

/*   ## same as ansi_CUP : Cursor Position {1} row {2} column           */
#define ansi_HVP(y,x)       ansi_CSI();_outByteStr(y);_out(';');_outByteStr(x);_out('f')

/*   ## Select Graphic Endition {1}                                     */
#define ansi_SGR(x)         ansi_CSI();_outByteStr(x);_out('m')

/*   ## Select Graphic Rendition with 2 parameters {1}n, {2}p1, {3}p2   */
#define ansi_SGR3(n,p1,p2)  ansi_CSI();_outByteStr(n);_out(';');_outByteStr(p1);_out(';');_outByteStr(p2);_out('m')

/*    ## Select Graphic Rendition with 4 parameters  
          {1} n, {2}p1, {3}p2, {4}p3, {5},p4                            */
#define ansi_SGR5(n,p1,p2,p3,p4) ansi_CSI();_outByteStr(n);_out(';');_outByteStr(p1);_out(';');_outByteStr(p2);_out(';');_outByteStr(p3);_out(';');_outByteStr(p4);_out('m')

/*    ## AUX Port On                                                    */
#define ansi_AUXportOn()      ansi_CSI()_out('5');_out('i')

/*    ## AUX Port Off                                                   */
#define ansi_AUXportOff()     ansi_CSI();_out('4');_out('i')

/*    ## Device Status Report                                           */
#define ansi_DSR()            ansi_CSI();_out('6');_out('n')

/*    ## Save Cursor Position                                           */
#define ansi_SCP()            ansi_CSI();_out('s')
    
/*    ## Restore Cursor Position                                        */
#define ansi_RCP()            ansi_CSI();_out('u')

/*    ## show cursor                                                    */
#define ansi_SCU()            ansi_CSI();_outStr("?25h")

/*    ## hide cursor                                                    */
#define ansi_HCU()            ansi_CSI();_outStr("?25l")

/*    ## enable alternate screen buffer                                 */
#define ansi_EASB()           ansi_CSI();_outStr("?1049h")

/*    ## disable alternate screen buffer                                */
#define ansi_DASB()           ansi_CSI();_outStr("?1049l")

/*   ## Turn on bracketed paste mode                                    */
#define ansi_BPMon()          ansi_CSI();_outStr("?2004h")

/*    ## Turn off bracketed paste mode                                  */
#define ansi_BPMoff()         ansi_CSI();_outStr("?2004l")

/*#####################################
  # ANSI SGR (Select Graphic Rendition)
  #####################################                                 */

/*    ## Reset / Normal                                                 */
#define ansi_SGR_reset()     ansi_SGR(0)

/*    ## Bold or increased intensity                                    */
#define ansi_SGR_bold()      ansi_SGR(1)

/*    ## Faint (decreased intensity)                                    */
#define ansi_SGR_faint()     ansi_SGR(2)

/*    ## Italic                                                         */
#define ansi_SGR_italic()    ansi_SGR(3)

/*    ## Underline                                                      */
#define ansi_SGR_underline() ansi_SGR(4)

/*    ## Slow Blink                                                     */
#define ansi_SGR_sblink()    ansi_SGR(5)
    
/*    ## Rapid Blink                                                    */
#define ansi_SGR_rblink()    ansi_SGR(6)    

/*    ## Reverse Video                                                  */
#define ansi_SGR_reverse()   ansi_SGR(7)

/*    ## Conceal                                                        */
#define ansi_SGR_conceal()   ansi_SGR(8)    

/*    ## Crossed-out                                                    */
#define ansi_SGR_crossout()  ansi_SGR(9)    

/*    ## primary font                                                   */
#define ansi_SGR_defaultfont ansi_SGR(10)

/*    ## alternate font (1 to 9) , 0 same as primary                    */
#define ansi_SGR_font(x)     ansi_SGR((x+10))

/*    ## Fraktur                                                        */
#define ansi_SGR_fraktur()   ansi_SGR(20)

/*    ## Bold off or Double Underline                                   */
#define ansi_SGR_doubleunderline()  ansi_SGR(21)

/*    ## Normal color or intensity (neither bold nor faint)             */
#define ansi_SGR_normalcolorintensity() ansi_SGR(22)

/*    ## Not italic nor fraktur                                         */
#define ansi_SGR_italicfrakturoff()  ansi_SGR(23)

/*    ## Underline off                                                  */
#define ansi_SGR_underlineoff()      ansi_SGR(24)

/*    ## blink off                                                      */
#define ansi_SGR_blinkoff()          ansi_SGR(25)

/*    ## Inverse off                                                    */
#define ansi_SGR_inverseoff()        ansi_SGR(27)

/*    ## Reveal / conceal off                                           */
#define ansi_SGR_concealoff()        ansi_SGR(28)

/*    ## Not cross-out                                                  */
#define ansi_SGR_crossoutoff()       ansi_SGR(29)

/*    ## Set foreground color 0 to 7  n {1}                             */
#define ansi_SGR_fgcolor(x)          ansi_SGR((x+30))

/*    ## set foreground color , palette on 256 colors n {1}             */
#define ansi_SGR_fg256(x)            ansi_SGR3(38,5,x)

/*    ## set foreground color, RGB 24 bits (3* 256 colors, RGB)
          r {1}, g {2}, b {3}                                           */
#define ansi_SGR_fgRGB(r,g,b)        ansi_SGR5(38,2,r,g,b)    

/*    ## set default foreground color                                   */
#define ansi_SGR_fgdefault()         ansi_SGR(39)

/*    ## Set background color 0 to 7 n{1}                               */
#define ansi_SGR_bgcolor(x)          ansi_SGR((x+40))

/*    ## set background color , palette on 256 colors n{1}              */
#define ansi_SGR_bg256(x)            ansi_SGR3(48,5,x)

/*    ## set background color, RGB 24 bits (3* 256 colors, RGB)
        r{1}, g{2}, b{3}                                                */
#define ansi_SGR_bgRGB(r,g,b)        ansi_SGR5(48,2,r,g,b)    

/*    ## set default background color                                   */
#define ansi_SGR_bgdefault()         ansi_SGR(49)

/*    ## Framed                                                         */
#define ansi_SGR_framed()            ansi_SGR(51)    

/*    ## Encircled                                                      */
#define ansi_SGR_encircled()         ansi_SGR(52)

/*    ## Overlined                                                      */
#define ansi_SGR_overlined()         ansi_SGR(53)

/*    ## Not framed or encirlced                                        */
#define ansi_SGR_framedoff()         ansi_SGR(54)

#define ansi_SGR_encircledoff()      ansi_SGR(54)

/*    ## Not overlined                                                  */
#define ansi_SGR_overlinedoff()      ansi_SGR(55)

/*    ## Foreground Black                                               */
#define ansi_SGR_fg_black()          ansi_SGR_fgcolor(0)

/*    ## Foreground Red                                                 */
#define ansi_SGR_fg_red()            ansi_SGR_fgcolor(1)

/*   ## Foreground Green                                                */
#define ansi_SGR_fg_green()          ansi_SGR_fgcolor(2)

/*    ## Foreground Yellow                                              */
#define ansi_SGR_fg_yellow()         ansi_SGR_fgcolor(3)

/*   ## Foreground Blue                                                 */
#define ansi_SGR_fg_blue()           ansi_SGR_fgcolor(4)

/*   ## Foreground Magenta                                              */
#define ansi_SGR_fg_magenta()        ansi_SGR_fgcolor(5)

/*    ## Foreground Cyan                                                */
#define ansi_SGR_fg_cyan             ansi_SGR_fgcolor(6)

/*    ## Foreground White                                               */
#define ansi_SGR_fg_white()          ansi_SGR_fgcolor(7)
    
/*    ## Background Black                                               */
#define ansi_SGR_bg_black()          ansi_SGR_bgcolor(0)

/*    ## Background Red                                                 */
#define ansi_SGR_bg_red()            ansi_SGR_bgcolor(1)

/*    ## Background Green                                               */
#define ansi_SGR_bg_green()          ansi_SGR_bgcolor(2)

/*    ## Background Yellow                                              */
#define ansi_SGR_bg_yellow()         ansi_SGR_bgcolor(3)

/*    ## Background Blue                                                */
#define ansi_SGR_bg_blue()           ansi_SGR_bgcolor(4)

/*    ## Background Magenta                                             */
#define ansi_SGR_bg_magenta()        ansi_SGR_bgcolor(5)

/*    ## Background Cyan                                                */
#define ansi_SGR_bg_cyan()           ansi_SGR_bgcolor(6)

/*   ## Background White                                                */
#define ansi_SGR_bg_white()          ansi_SGR_bgcolor(7)

#endif

