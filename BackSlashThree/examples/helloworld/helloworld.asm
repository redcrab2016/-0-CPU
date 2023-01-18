; Hello World example
    org     $0000       ; int_0 , cpu reset PC address location
    dw      start       ; set boot address on int_0 adress

start   org $0400       ; first address usable for program
    lean_w0 msg         ; set register W0 with msg addr by offset ref
loop:                   ; for each character to print
    ld      b3, [w0]    ; get a character
    cmp     b3, 0       ; test if it is the last character
    jz      bye         ; if yes then stop program
retry:
    out     b3          ; print out the character
    jz      retry       ; if output is not ready then retry
    inc     w0          ; after print out, select next character
    j       loop        ; print out next character
bye:    
    hlt                 ; end of Hello World, stop

; Data area
msg:
    db "Hello world.",10,0        
