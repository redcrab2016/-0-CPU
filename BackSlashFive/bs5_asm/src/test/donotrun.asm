// This is a source code not intended to be executed
// The aim is to check the assembly result
; 31 core instructions
;	mov (Z flag )
;		mov Rx, 0
mov R0, 0
mov R5, 0
;		mov low Rx, 0
mov low R0, 0
mov low R5, 0
;		mov high Rx, 0
mov high R0, 0
mov high R5, 0
;		mov Rx, Ry
mov R5, R4
;		mov Rx, [Ry]
mov R5, [R4]
;		mov [Rx], Ry
mov [R5], R4
;		mov low R0, imm8
mov low R0, 0
mov low R0, 255
;		mov high R0, imm8
mov high R0, 0
mov high R0, 255
;		mov low R0, low Rx
mov low R0, low R5
;		mov high R0, low Rx
mov high R0, low R5
;		mov low R0, high Rx
mov low R0, high R5
;		mov high R0, high Rx
mov high R0, high R5
;		mov C, Rx:imm4
mov C, R5:15
;		mov Rx:imm4, C
mov r5:15,C
;		mov Rx:imm4, 0
mov R5:15,0
;		mov Rx:imm4, 1
mov R5:7,1

;	add ( Z, C flags)
;		add Rx, Ry
add R5,r4
;		add Rx, [Ry]
add R5, [R4]
;		add R0, imm4
add R0, 0
add R0, 15
;		add Rx, 1
add R5, 1
;		add R15, simm8
add R15, +127
add R15, -128

;	sub ( Z, C flags)
;		sub Rx, Ry
sub R5, R4
;		sub Rx, [Ry]
sub r5, [r4]
;		sub R0, imm4
sub R0, 15
;		sub Rx, 1
sub R5, 1
;	shl ( Z, C flags)
;		shl R0, imm4
shl R0, 7
;	shr ( Z, C flags)
;		shr R0, imm4
shr R0, 7	
;	and (Z flag)
;		and R0, Rx
and R0, R5
;	or (Z flag)
;		or R0, Rx
or R0, R5	
;	not (Z flag)
;		not Rx:imm4
not R5:15
;		not Rx
not R5

;12 microprograms for missing register versatility (Rx with x != 0)
;ccc f mov low Rx, imm8 
mov low R5, 127
;ccc f  mov high Rx, imm8
mov high R5, 127
;ccc f add Rx, imm4
add R5, 12
;ccc f sub Rx, imm4
sub R5, 15
;ccc f shl Rx, imm4
shl R5, 7
;ccc f shr Rx, imm4
shr R5, 7
;ccc f mov low Rx, low Ry
mov low R5, low R4
;ccc f mov low Rx, high Ry
mov low R5, high R4
;ccc f mov high Rx, low Ry
mov high R5, low r4
;ccc f mov high Rx, high Ry
mov high r5, high r4
;ccc f and Rx, Ry
and R5, r4
;ccc f or  Rx, Ry
or R5, R4

;12 microprograms to handle 16 bits immediate
;ccc f mov Rx, imm16   ; (Rx != R0 and imm16 > 15)
mov R5, -32768
;ccc f mov R0, imm16   ; ( imm16 != 0 )
mov R0, 65535
;ccc f add Rx, imm16   ; (Rx != R0 and imm16 > 15)
add R5, #0xa5A5
;ccc f sub Rx, imm16   ; (Rx != R0 and imm16 > 15)
sub R5, 18
;ccc f mov Rx, [imm16] ; ( Rx can be R0 )
mov R5, [1024]
mov R0, [#0xCAFA]
;ccc f mov [imm16], Rx ; ( Rx != R0 )
mov [#0xCAFE], R5
;ccc f add Rx, [imm16] ; ( Rx != R0 )
add R5, [-1024]
;ccc f sub Rx, [imm16] ; ( Rx != R0 )
sub R5, [#0x1234]
;ccc f and Rx, imm16   ; (Rx != R0)
and R5, #0xAA55	
;ccc f or Rx, imm16    ; (Rx != R0)
or R5, 2048
;ccc f and Rx, [imm16] ; (Rx != R0)
and R5, [-1]
;ccc f or Rx, [imm16]  ; (Rx != R0)
or R5, [-32768]

; Other basic test
lastinstr:
xs nf or R5, [ptr lastinstr]
xc fl add R5, low ptr lastinstr
ne nf mov low R5, low ptr lastinstr
; raw data
dw 0, ptr lastinstr, low ptr lastinstr, high ptr lastinstr
org #0x1000
dw "Hello World, this is a string", 13,10, 0
.letter: dw 'a','b','c'
dw 0, loffset .letter
