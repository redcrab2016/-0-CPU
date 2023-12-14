// This is a source code not intended to be executed
// The aim is to check the assembly result
; 31 core instructions
;	mov (Z flag )
;		mov Rx, 0
mov R0, 0
mov R5, 0
.petitlabel:
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

;63 Microprograms to handle stack (backward memory stacking, stack addr in R13, local context in stack addr in R12)
;  Push value into stack (4 microprograms)
;ccc f mov STACK, Rx
mov stack, R5
;ccc f mov STACK, [Rx]  (R0 modified)
ss mov stack, [R5]
;ccc f mov STACK, imm16 (R0 modified)
gt fl mov stack, 32767
;ccc f mov STACK, [imm16] ( R0 modified)
ll nf mov stack, [ptr lastinstr]
;  Pop value from stack (3 microprograms)
;ccc f mov Rx, STACK (if R15 then it is like a return from call procedure)
mov R0, stack
mov R15, stack
;ccc f mov [Rx], STACK ( Rx != R0, R0 modified)
ac mov [R5], stack
;ccc f mov [imm16], STACK (R0 modified)
as mov [ptr lastinstr], stack
;  Call procedure  (5 microprograms)
callproc:
;ccc f mov STACK:R15, Rx (Rx != R0 & Rx != R15)
eq mov stack:R15, R5
;ccc f mov STACK:R15, [Rx] (Rx != R0 & Rx != R15)
mov stack:R15, [R5]
;ccc f mov STACK:R15, imm16 (R0 modified)
mov stack:R15, #0x7000
;ccc f mov STACK:R15, [imm16] (R0 modified)
mov stack:R15, [#0x8000]
;ccc f add STACK:R15, simm8  (near address, R0 modified)
al nf add stack:R15 loffset callproc
;   Write to stack/local context (24 microprograms)
;ccc f mov STACK:imm4, Rx (Rx != R0, R0 modified)
mov stack:#0xA, R5
;ccc f mov STACK:imm4, imm16 ( R0 modified)
mov stack:10, 18
;ccc f mov STACK:imm4, [Rx] (Rx != R0 , R0 modified)
mov stack:15, [R5]
;ccc f mov STACK:imm4, [imm16] (R0 modified)
mov stack:0, [4096]
;ccc f mov STACK:imm16, Rx (Rx != R0 , R0 modified)
mov stack:#0xA000, R5
;ccc f mov STACK:imm16a, imm16b ( R0 modified)
mov stack:-32768, 32767
;ccc f mov STACK:imm16, [Rx] (Rx != R0 , R0 modified)
mov stack:-18, [R5]
;ccc f mov STACK:imm16a, [imm16b] ( R0 modified)
mov stack:4096, [ptr callproc]
;ccc f mov STACK:Rx, Ry  (Rx and Ry != R15 and R0)
mov stack:R5,R8
;ccc f mov STACK:Rx, imm16 (Rx != R0 and R15, R0 modified)
mov stack:R5, 14000
;ccc f mov STACK:Rx, [Ry] (Rx,Ry != R0 , R0 modified)
mov stack:R5, [R4]
;ccc f mov STACK:Rx, [imm16] (Rx != R0 , R0 modified)
mov stack:R5, [-15]
;ccc f mov LOCAL:imm4, Rx (Rx != R0, R0 modified)
mov local:13, R5
;ccc f mov LOCAL:imm4, imm16 ( R0 modified)
ss mov local:10, 4095
;ccc f mov LOCAL:imm4, [Rx] (Rx != R0 , R0 modified)
mov local:0, [R5]
;ccc f mov LOCAL:imm4, [imm16] ( R0 modified)
mov local:7, [65535]
;ccc f mov LOCAL:imm16, Rx (Rx != R0 , R0 modified)
mov local:-1, R1
;ccc f mov LOCAL:imm16a, imm16b ( R0 modified)
mov local:4095, 4096
;ccc f mov LOCAL:imm16, [Rx] (Rx != R0 , R0 modified)
mov local:255, [R5]
;ccc f mov LOCAL:imm16a, [imm16b] ( R0 modified)
mov local:256, [255]
;ccc f mov LOCAL:Rx, Ry  (Rx and Ry != R15 and R0)
mov local:R5,R8
;ccc f mov LOCAL:Rx, imm16 (Rx != R0 , R0 modified)
mov local:R5, 0
;ccc f mov LOCAL:Rx, [Ry] (Rx,Ry != R0 , R0 modified)
mov local:R3, [R4]
;ccc f mov LOCAL:Rx, [imm16] (Rx != R0 , R0 modified)
mov local:R4, [ptr callproc]
;	Read from stack/local context (18 microprograms)
;ccc f mov Rx, STACK:imm4 (Rx != R0 , R0 modified)
mov R11, stack:0
;ccc f mov [Rx], STACK:imm4 (Rx != R0 , R0 modified)
mov [R10], stack:15
;ccc f mov [imm16], STACK:imm4 ( R0 modified)
mov [0], stack:7
;ccc f mov Rx, STACK:imm16 (Rx != R0 , R0 modified)
mov R4, stack:255
;ccc f mov [Rx], STACK:imm16 (Rx != R0 , R0 modified)
mov [R5], stack:-32768
;ccc f mov [imm16a], STACK:imm16b ( R0 modified)
mov [#0x7000], stack:#0xFF00
;ccc f mov Rx, STACK:Ry (Rx,Ry != R0 , R0 modified)
gs fl mov Rx, stack:R10
;ccc f mov [Rx], STACK:Ry (Rx,Ry != R0 , R0 modified)
gc nf mov [R5], stack:R4
;ccc f mov [imm16], STACK:Rx
mov [16], stack:R5
;ccc f mov Rx, LOCAL:imm4 (Rx != R0 , R0 modified)
mov R5, local:10
;ccc f mov [Rx], LOCAL:imm4 (Rx != R0 , R0 modified)
mov [R5], local:#0xf 
;ccc f mov [imm16], LOCAL:imm4 (R0 modified)
ne mov [#0xff00], local:4
;ccc f mov Rx, LOCAL:imm16 (Rx != R0 , R0 modified)
eq mov R4, local:15000
;ccc f mov [Rx], LOCAL:imm16 (Rx != R0 , R0 modified)
mov [R5], local:2000
;ccc f mov [imm16a], LOCAL:imm16b ( R0 modified)
mov [2048], local:9048
;ccc f mov Rx, LOCAL:Ry (Rx,Ry != R0 , R0 modified)
mov R1, local:R1
;ccc f mov [Rx], LOCAL:Ry (Rx,Ry != R0 , R0 modified)
mov [R2], local:R2
;ccc f mov [imm16], LOCAL:Rx (Rx != R0 , R0 modified)
mov [12000], local:R5
;	Local context / Stack context setting (2 microprograms)
;ccc f mov STACK, LOCAL 
mov stack, local
;ccc f mov LOCAL, STACK 
mov local, stack
;	Standard instructions (add, sub, shl, shr, and, or , not) by stack (7 microprograms)
;ccc f add STACK
gt fl add stack
;ccc f sub STACK ; stack head = stack head - stack+1
le nf sub stack
;ccc f shl STACK, imm4
zc shl stack, 4
;ccc f shr STACK, imm4
cc shr stack, 2
;ccc f and STACK
and stack
;ccc f or STACK
or stack
;ccc f not STACK
not stack


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
