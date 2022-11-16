# \0  CPU 16/32 bits:  

## Specification
	The CPU may works in two modes
		16 bits : 16 bits address, 8/16 bits value (default at CPU boot or after reset)
		32 bits : 32 bits address, 16/32 bits value
	All data(dynamic/immediate) in memory are little-endian (first LSB , then MSB)
		Hexadecimal 16 bits example value AB12 is arranged in memory a 12 AB
		Hexadecimal 32 bits example value AB1200FF is arranged in memory as FF 00 12 AB
	16/32 bits intruction compatibility
		One code may works on 16/32 bits, but with particular attention to:
			Memory occupation is not the same (e.g 16 vs 32 bits stack value size)
			ROL/ROR instruction behaviour is data size dependant
			Beware of any value shift operations (SHL, SHR, ASL)
			advisable to use the 16 bits behavior of dynamic data access in your 32/16 bits compatible code (Read/Write mask flags)
			No compatibility for instructions using 2 immediates value (e.g immediate adresse, immediate value like MOV [0A00],FE10 : 0A00 immediate address and FE10 immediate value)
			Same operational instruction set for 16/32 bits
			Immediate data compatibility between 16/32 bits (for instruction using only one immediate value)
				NOP is byte value 00 then for instance the byte sequence 20 00 A0 00 00 has the same behavior in 16 and 32 bits CPU mode
					Represent 3 instructions in 16 bits CPU mode
						JMP A000   ; 20 00 A0
						NOP        ; 00
						NOP        ; 00
					Represent 1 instruction in 32 bits CPU mode
						JMP 0000A000 ; 20 00 A0 00 00

	Each instruction is composed by:
		operator [modifer] [immediate data]
			operator, is always 8 bits for up to 256 operations
			modifier (optional), is always 8 bits for a potential of 256 modifiers for an operation (operation extension)
			immediate data (optional), between 8 and 64 bits representing one or two different value
	Each instruction size may be:
		in 16 bits mode from 8 to 48 bits (1 to 6 bytes) 
			operation(8 bits) modifier(0/8 bits) immediate data(0/8/16/8+16=24/16+16=32 bits)
		in 32 bits mode	from 8 to 80 bits (1 to 10 bytes)
			operation(8 bits) modifier(0/8 bits) immediate data(0/16/32/16+32=48/32+32=64 bits)

### Register
	I : 16/32 bits intruction pointer   : FFFC (16 bits at boot/reset)
	S : 16/32 bits stack pointer        : 0100 (16 bits at boot/reset)
	L : low 8/16 bits of R              : 00 (8 bits at boot/reset)
	H : hight 8/16 bits of R            : 00 (8 bits at boot/reset)
	R : 16/32 bits                      : 0000 (16 bits at boot/reset)
	A : 16/32 bits (used for adressing) : 0000 (16 bits at boot/reset)
	F : 8/16 bits flags ([b15] - - - - - - WriteMask ReadMask[b8] [b7]Negative Overflow Interrupt Stack DirectionSource DirectionDestination Zero Carry[b0]  ) 16 (_ _ _ _ _ _ _ _ , _ _ I _ _ _ _ _)  
	As register but it is memory stack access
	SW : 16 bits head of stack value ( same as W[S] ) : W[0100]
	SV : 8 bits head of stack ( same as V[S]) : V[0100]

### Memory map CPU:

16bits

    0000 - 001F : 30 bytes :16 times 16 bits interruption address initialized by CPU to FFFF
    FFFC - FFFE : 3 bytes : 0010 0000 xxxxxxxx XXXXXXXX : 20 xx XX : expected JMP HHHH HHHH address to bootstrap 
    FFFF - FFFF : 1 byte : 0010 0011 : 23 : expected IRET
    
32bits

    0001 0000 - 0000 0400 : 1024 bytes : 256 time 32 bits interruption address (should be initialized to 0000FFFF)
    
### Value and access modes
		...V : V means 8 bits target in 16 bits mode, 16 bits in 32 bits mode
		...W : W means 16 bits target in 16 bits mode, 32 bits in 32 bits mode
#### immediate value : 
		a/2a bits modes : if 16bits or 32 bits CPU mode
		im4/8 : immediate 4/8 bits ( for 16 and 32 bits mode respectively), iiii (unsigned value bteween 0 and 15) or iiiiiiii ( unsigned value between 0 and 255)
		imV.simV : 16 bits mode : immediate 8 bits, iiiiiiii (unsigned value between 0 and 255), siiiiiii (signed value between -128 and + 127)
		           32 bits mode : immediate 16 bits, iiiiiiii IIIIIIII (unsigned value between 0 65535) , iiiiiiii SIIIIIII (signed value between -32768 and + 32767) little endian LSB then MSB 
		imW,simW : 16 bits mode : immediate 16 bits, iiiiiiii IIIIIIII (unsigned value between 0 65535) , iiiiiiii SIIIIIII (signed value between -32768 and + 32767) little endian LSB then MSB
		           32 bits mode : immediate 32 bits, iiiiiiii iiiiiiii IIIIIIII IIIIIIII 'unsigned value between 0 and 4294967295‬, ... signed -2147483648‬ to 21474836487 
#### register value
		1m = 1 in 16 bits mode, 2 in 32 bits mode
		2m = 2 in 16 bits mode, 4 in 32 bits mode

		regV : register 8 in 16 bits mode, 16 bits in 32 bits mode
			reg:RR => (L:00, H:01, F:10 V8:11 )
				SV can been seen as V[S] (for NOT,INC,DEC,NEG and ZERO) or V[S+1m] (for PUSH)
		reg1W : register 16 in 16 bit mode, 32 bits in 32 bits mode
			reg:RR => (R:00, A:01, S:10, V16:11)
				SW can been seen as W[S] (for NOT,INC,DEC,NEG and ZERO) or W[S+2m] ( for PUSH)
#### memory value with access mode
		immediate data representing address
			relative address:  addr-rel
				16 bits mode
					8 bits signed data saaaaaaa
				32 bits mode
					16 bits signed data sAAAAAAAaaaaaaa : offset byte 0 iiiiiiii, offset byte +1 sIIIIIII
			absolute address: addr-abs
				16 bits mode
					16 bits data AAAAAAAAaaaaaaaa : offset byte 0 aaaaaaaa, offset byte +1 AAAAAAAA
				32 bits mode
					32 bits data HHHHHHHHhhhhhhhhLLLLLLLLllllllll : ofset byte 0 llllllll, offset byte +1 LLLLLLLL, offset byte +2 hhhhhhhh, offset byte +3 HHHHHHHH 
			
		MMaddressing : does represent the immdiate data depending of the memory addressing mode (MM). It can be nothing(based on register), addr-abs or addr-rel
			
		- meV: memory explicit 8 bits target : (if Read or Write Mask flag set then 8 bits access 16 bits otherwise)
			V[imW]   : (MM=00 addr-abs) at 16/32bits immediate address   
			V[A]       : (MM=01) at address in register A, same as A+0
			V[A+imV]  : (MM=10 addr-rel) at address register A + 8/16 bits immediate offset
			V[[A]]     : (MM=11) A is the address where is locted the addres of the 8/16 bits value
		- meW : memory explicit 16 bits target (if Read or Write Mask flag set then 16 bits access 32 bits otherwise)
			W[imW]  : (MM=00 addr-abs) at 16/32bits immediate address
			W[A]      : (MM=01) : at address in register A, same as A+0
			W[A+imV] : (MM=10 addr-rel) at address register A + 8/16 bits immediate offset
			W[[A]]    : (MM=11) A is the adress where is located the address of the 16/32 bits value
### Operation

Horizontal : MSB
Vertical : LSB

| Hex | 0-        | 1-       | 2-            | 3-               | 4-             | 5-            | 6-            | 7-           | 8-           | 9-           | A-           | B-           | C-            | D-        | E-   | F-        |
|-----|-----------|----------|---------------|------------------|----------------|---------------|---------------|--------------|--------------|--------------|--------------|--------------|---------------|-----------|------|-----------|
| -0  | NOP       | .        | JMP (abs)     | JMP (rel)        | PUSH L         | PUSH R        | POP L         | POP R        | NOT L        | INC L        | DEC L        | NEG L        | ZERO L        | SHL mod2  | STC  | RML       |
| -1  | MOV       | MOV mod1 | CALL (abs)    | CALL (rel)       | PUSH H         | PUSH A        | POP H         | POP A        | NOT H        | INC H        | DEC H        | NEG H        | ZERO H        | SHR mod2  | STZ  | RMH       |
| -2  | AND       | AND mod1 | RET (abs)     | JMP W[A+L]       | PUSH F         | MOV W[A],S    | POP F         | MOV S,W[A]   | NOT V[S+1m]  | INC V[S+1m]  | DEC V[S+1m]  | NEG V[S+1m]  | ZERO V[S+1m]  | ROL mod2  | STN  | WML       |
| -3  | OR        | OR mod1  | IRET (abs)    | CALL W[A+L]      | PUSH I+simV    | PUSH I+simW   | POP V[S+1m]   | POP W[S+2m]  | NOT V[S]/SV  | INC V[S]/SV  | DEC V[S]/SV  | NEG V[S]     | ZERO V[S]/SV  | ROR mod2  | STO  | WMH       |
| -4  | XOR       | XOR mod1 | LOOP R (abs)  | LOOP R (rel)     | PUSH imV       | PUSH imW      | .             | .            | NOT V[imW]   | INC V[imW]   | DEC V[imW]   | NEG V[imW]   | ZERO V[imW]   | ASL mod2  | STI  | .         |
| -5  | ADD       | ADD mod1 | LOOP A (abs)  | LOOP A (rel)     | PUSH H,L,F/R,F | PUSH R,A      | POP F,L,H/F,R | POP A,R      | NOT V[A]     | INC V[A]     | DEC V[A]     | NEG V[A]     | ZERO V[A]     | .         | STS  | .         |
| -6  | SUB       | SUB mod1 | LOOP S (abs)  | LOOP S (rel)     | .              | PUSH R,A,F    | .             | POP F,A,R    | NOT V[A+imV] | INC V[A+imV] | DEC V[A+imV] | NEG V[A+imV] | ZERO V[A+imV] | .         | STDS | .         |
| -7  | ADC       | ADC mod1 | LOOP SW (abs) | LOOP SW (rel)    | .              | PUSH I        | .             | POP I        | NOT V[[A]]   | INC V[[A]]   | DEC V[[A]]   | NEG V[[A]]   | ZERO V[[A]]   | .         | STDD | .         |
| -8  | SBB       | SBB mod1 | JC (abs)      | JC (rel)         | PUSH V[imW]    | PUSH W[imW]   | POP V[imW]    | POP W[imW]   | NOT R        | INC R        | DEC R        | NEG R        | ZERO R        | SWAP mod3 | CLC  | MODE32 A  |
| -9  | TST       | TST mod1 | JZ (abs)      | JZ (rel)         | PUSH V[A]      | PUSH W[A]     | POP V[A]      | POP W[A]     | NOT A        | INC A        | DEC A        | NEG A        | ZERO A        | .         | CLZ  | MODE16 A  |
| -A  | CMP       | CMP mod1 | JS (abs)      | JS (rel)         | PUSH V[A+imV]  | PUSH W[A+imV] | POP V[A+imV]  | POP W[A+imV] | NOT W[S+2m]  | INC W[S+2m]  | DEC W[S+2m]  | NEG W[S+2m]  | ZERO W[S+2m]  | .         | CLN  | INT im4/8 |
| -B  | MOVS 1    | .        | JO (abs)      | JO (rel)         | PUSH V[[A]]    | PUSH W[[A]]   | POP V[[A]]    | POP W[[A]]   | NOT W[S]/SW  | INC W[S]/SW  | DEC W[S]/SW  | NEG W[S]/SW  | ZERO W[S]/SW  | .         | CLO  | BRKP      |
| -C  | MOVS L    | LODS L   | JNC (abs)     | JNC (rel)        | DUPV 4         | DUPW 4        | DROPV 4       | DROPW 4      | NOT W[imW]   | INC W[imW]   | DEC W[imW]   | NEG W[imW]   | ZERO W[imW]   | .         | CLI  | .         |
| -D  | MOVS V[A] | LODS R   | JNZ (abs)     | JNZ (rel)        | DUPV 1         | DUPW 1        | DROPV 1       | DROPW 1      | NOT W[A]     | INC W[A]     | DEC W[A]     | NEG W[A]     | ZERO W[A]     | .         | CLS  | RESET     |
| -E  | MOVS R    | STOS L   | JNS (abs)     | JNS (rel)        | DUPV 2         | DUPW 2        | DROPV 2       | DROPW 2      | NOT W[A+imV] | INC W[A+imV] | DEC W[A+imV] | NEG W[A+imV] | ZERO W[A+imV] | .         | CLDS | WAIT      |
| -F  | MOVS W[A] | STOS R   | JNO (abs)     | JNO (rel)        | DUPV 3         | DUPW 3        | DROPV 3       | DROPW 3      | NOT W[[A]]   | INC W[[A]]   | DEC W[[A]]   | NEG W[[A]]   | ZERO W[[A]]   | .         | CLDD | HALT      |


(rel) : relative branching (immediate data is a branching relative to I, I=I+SignedImmediate)

(abs) : absolute adressing (immediate data is a direct branching, I=immediate)

mod1, mod2, mod3 : operation modifier (intruction extension) decribed in "Modifiers" below

1m : is 1 in 16 bits CPU mode, 2 in 32 bits CPU mode

2m : is 2 in 16 bits CPU mode, 4 in 32 bits CPU mode


### Modifiers
#### mod1
	V (8 bits in 16 bits mode, 16 bits in 32 bits mode)
		regV, imV    : #0000 RR 00  imV  
		regV, meV     : #0001 RR MM  MMaddressing
		regV, regV    : #0010 RR RR  /
		meV, imV     : #0011 MM 00  imV MMaddressing 
		meV, regV     : #0100 MM RR  MMaddressing
		V[ds], V[ss]  : #0101 DD SS / (ds = S + (DD + 1)m, ss = S + (SS + 1)m)
		regV, V[ss]   : #0110 RR SS / (ss = S + (SS + 1)m)
		V[ds], regV   : #0111 DD RR / (ds = S + (DD + 1)m)
	W (16 bits in 16 bits mode, 32 bits in 32 bits mode)
		regW, imW   : #1000 RR 00 imW
		regW, meW    : #1001 RR MM MMadressing
		regW, regW   : #1010 RR RR /
		meW, imW    : #1011 MM 00 imW MMaddressing
		meW, regW    : #1100 MM RR MMaddressing
		W[ds], W[ss] : #0101 DD SS / (ds = S + (DD + 1)2m, ss = S + (SS + 1)2m)
		regW, W[ss]  : #0110 RR SS / (ss = S + (SS + 1)2m)
		W[ds], regW  : #0111 DD RR / (ds = S + (DD + 1)2m)

#### mod2
	V (8 bits in 16 bits mode, 16 bits in 32 bits mode) : 0
		cst N  : 0 NNN
			regV : #00NNN 0 RR /
			memV : #00NNN 1 MM MMaddressing
		regV  : 1 0RR
			regV : #010RR 0 RR /
			memV : #010RR 1 MM MMaddressing
		regW : 1 1RR
			regV : #011RR 0 RR /
			memV : #011RR 1 MM MMaddressing
	W (16 bits in 16 bits mode, 32 bits in 32 bits mode) : 1
		cst   : 0 NNN
			regW : #10NNN 0 RR /
			memW : #10NNN 1 MM MMaddressing
		regV  : 1 0RR
			regW : #110RR 0 RR /
			memW : #110RR 1 MM MMaddressing
		regW : 1 1RR
			regW : #111RR 0 RR /
			memW : #111RR 1 MM MMaddressing

#### mod3
	V (8 bits in 16 bits mode, 16 bits in 32 bits mode): 0
		regV, regV : #0000 RR RR /
		meV, regV or regV,meV : #0001 RR MM  MMaddressing
		regV, V[ss] or V[ss], regV : #0010 RR SS  (ss = S + (SS + 1)m)
		V[ds], V[ss] or V[ss], V[ds] : #0011 DD SS (ds = S + (DD + 1)m, ss = S + (SS + 1)m)
	W (16 bits in 16 bits mode, 32 bits in 32 bits mode) : 1
		regW, regW : #1000 RR RR /
		meW, regW or regW, meW : #1001 RR MM  MMaddressing
		regW, W[ss] or W[ss], regW : #0010 RR SS  (ss = S + SS + 1)
		W[ds], W[ss] or W[ss], W[ds] : #0011 DD SS (ds = S + DD + 1, ss = S + SS + 1)

## Design
    NOP $00 00 0000
    MOV $00 00 0001 [address]= value with address = W[S] and value = stackword[S+2m] then DROP BYTE 3 or 4 (8 or 16 bits). stackword=8 if stack flag =0 stackword=16 otherwise 
    AND $00 00 0011 
    OR  $00 00 0011
    XOR $00 00 0100
    ADD $00 00 0101
    SUB $00 00 0110
    ADC $00 00 0111
    SBB $00 00 1000
    TST $00 00 1001
    CMP $00 00 1010
	(10 operations, 0 modifiers)
	operate directly on stack (if Stack flag = 0 then 8 bits operation, is stack flag = 1 then 16 bits operation)

    MOVS 1 $00 00 1011
    MOVS L $00 00 1100 
    MOVS V[A] $00 00 1101 
    MOVS R $00 00 1110 
    MOVS W[A] $00 00 1111 
	Direction(source/destination) flag =1 progress in address backward , forward otherwise

    RESERVED  $00 01 0000 : reserved for future use

    MOV  $00 01 0001
    AND  $00 01 0010
    OR   $00 01 0011
    XOR  $00 01 0100
    ADD  $00 01 0101
    SUB  $00 01 0110
    SDC  $00 01 0111
    SBB  $00 01 1000
    TST  $00 01 1001
    CMP  $00 01 1010
	( 10 operations, 256 modifiers, )
	V (8 bits in 16 bits mode, 16 bits in 32 bits mode)
		regV, imV    : #0000 RR 00  imV  
		regV, meV     : #0001 RR MM  MMaddressing
		regV, regV    : #0010 RR RR  /
		meV, imV     : #0011 MM 00  imV MMaddressing 
		meV, regV     : #0100 MM RR  MMaddressing
		V[ds], V[ss]  : #0101 DD SS / (ds = S + (DD + 1)m, ss = S + (SS + 1)m)
		regV, V[ss]   : #0110 RR SS / (ss = S + (SS + 1)m)
		V[ds], regV   : #0111 DD RR / (ds = S + (DD + 1)m)
	W (16 bits in 16 bits mode, 32 bits in 32 bits mode)
		regW, imW   : #1000 RR 00 imW
		regW, meW    : #1001 RR MM MM adressing data
		regW, regW   : #1010 RR RR /
		meW, imW    : #1011 MM 00 imW MMaddressing
		meW, regW    : #1100 MM RR MMaddressing
		W[ds], W[ss] : #0101 DD SS / (ds = S + (DD + 1)2m, ss = S + (SS + 1)2m)
		regW, W[ss]  : #0110 RR SS / (ss = S + (SS + 1)2m)
		W[ds], regW  : #0111 DD RR / (ds = S + (DD + 1)2m)

 
    RESERVED $00 01 1011 : reserved for future use
    LODS L $00 01 1100
    LODS R $00 01 1101 
    STOS L $00 01 1110 
    STOS R $00 01 1111



    $00 1xxxxx
    JMP/CALL/RET/LOOP (32 operation, 0 modifer)
	long absolute : 0
		direct: 0000 addr-abs
		CALL  : 0001 addr-abs
		RET   : 0010 /
		IRET   : 0011 /
		LOOP  : 01RR addr-abs (16/32 bits RR--, if RR != 0 I = address)
		if carry : 1000 addr-abs
		if zero  : 1001 addr-abs
		if Negative  : 1010 addr-abs
		if Overflow : 1011 addr-abs
		if not carry : 1100 addr-abs
		if not zero  : 1101 addr-abs
		if not sign  : 1110 addr-abs
		if not overflow : 1111 addr-abs
	short relative : 1     
		direct: 0000 addr-rel
		CALL  : 0001 addr-rel 
		JMPA  : 0010 / = JMP [A + L]
		CALA   : 0011 = CALL [A + L]
		LOOP  : 01RR addr-rel (16 bits RR--, if RR != 0 I = address)
		if arry : 1000 addr-rel
		if zero  : 1001 addr-rel
		if Negative  : 1010 addr-rel
		if Overflow : 1011 addr-rel
		if not carry : 1100 addr-rel
		if not zero  : 1101 addr-rel
		if not sign  : 1110 addr-rel
		if not overflow : 1111 addr-rel



    STACK $01 xxxxxx
	(64 operations, 0 modifier)
	PUSH/DUP : 0
		V (8 bits in 16 bits mode, 16 bits in 32 bits mode) : 0
			regV : 00 RR / or simV if RR=SV ( PUSH I+simV)
			imV : 01 00  imV
			XXX  : 01 01 /
			XXX  ; 01 10 / 
			XXX  : 01 11 /
			meV  : 10 MM  MMadressing
			DUP  : 11 nn ( Duplicate 8/16 bits stack head nn times, 00 = 4 times)
		W (16 bits in 16 bits mode, 32 bits in 32 bits mode) : 1
			regW : 00 RR  / or simW if RR=SW (PUSH I+simW)
			immW : 01 00 iiiiiiii IIIIIIII /
			XXX  : 01 01 /
			XXX  ; 01 10 / 
			XXX  : 01 11 /
			meW  : 10 MM MMaddresing
			DUP   : 11 nn ( Duplicate 16/32 bits stack head nn times, 00 = 4 times)
	POP/DROP : 1	
		V (8 bits in 16 bits mode, 16 bits in 32 bits mode) : 0
			regV : 00 RR /
			meV  : 10 MM  MMadressing
			DROP  : 11 nn (POP is a DROP 8/16bits nn times, 00 = 4 times)
		W (16 bits in 16 bits mode, 32 bits in 32 bits mode) : 1
			regW 00 RR
			meW  10 MM  MMadressing
			DROP : 11 nn ( POP is a DROP 16/32 bits nn times , 00 = 4 times)

    NOT  $10 00 xxxx 
    INC, $10 01 xxxx
    DEC, $10 10 xxxx
    NEG  $10 11 xxxx
    ZERO $11 00 xxxx
	( 5*16 operations, 0 modifier)
	V (8 bits in 16 bits mode, 16 bits in 32 bits mode): 0
		regV : 00 RR /
		meV  : 01 MM MMaddressing
	W (16 bits in 16 bits mode, 32 bits in 32 bits mode): 1
		regW: 10 RR /
		meW : 11 MM  MMaddressing

    SHL, $11 01 0000
    SHR, $11 01 0001
    ROL, $11 01 0010
    ROR, $11 01 0011
    ASL  $11 01 0100
	(5 operations, 256 modifiers)
	V (8 bits in 16 bits mode, 16 bits in 32 bits mode) : 0
		cst N  : 0 NNN
			regV : #00NNN 0 RR /
			memV : #00NNN 1 MM MMaddressing
		regV  : 1 0RR
			regV : #010RR 0 RR /
			memV : #010RR 1 MM MMaddressing
		regW : 1 1RR
			regV : #011RR 0 RR /
			memV : #011RR 1 MM MMaddressing
	W (16 bits in 16 bits mode, 32 bits in 32 bits mode) : 1
		cst   : 0 NNN
			regW : #10NNN 0 RR /
			memW : #10NNN 1 MM MMaddressing
		regV  : 1 0RR
			regW : #110RR 0 RR /
			memW : #110RR 1 MM MMaddressing
		regW : 1 1RR
			regW : #111RR 0 RR /
			memW : #111RR 1 MM MMaddressing

    RESERVED $11 01 0101 : reserved for future
    RESERVED $11 01 0110 : reserved for future
    RESERVED $11 01 0111 : reserved for future

    SWAP  $11 01 1000
	( 1 operations, 64 modifiers)
	V (8 bits in 16 bits mode, 16 bits in 32 bits mode): 0
		regV, regV : #0000 RR RR /
		meV, regV or regV,meV : #0001 RR MM  MMaddressing
		regV, V[ss] or V[ss], regV : #0010 RR SS  (ss = S + (SS + 1)m)
		V[ds], V[ss] or V[ss], V[ds] : #0011 DD SS (ds = S + (DD + 1)m, ss = S + (SS + 1)m)
	W (16 bits in 16 bits mode, 32 bits in 32 bits mode) : 1
		regW, regW : #1000 RR RR /
		meW, regW or regW, meW : #1001 RR MM  MMaddressing
		regW, W[ss] or W[ss], regW : #0010 RR SS  (ss = S + SS + 1)
		W[ds], W[ss] or W[ss], W[ds] : #0011 DD SS (ds = S + DD + 1, ss = S + SS + 1)
    RESERVED $11 01 1001 : reserved for future
    RESERVED $11 01 1010 : reserved for future
    RESERVED $11 01 1011 : reserved for future
    RESERVED $11 01 1100 : reserved for future
    RESERVED $11 01 1101 : reserved for future
    RESERVED $11 01 1110 : reserved for future
    RESERVED $11 01 1111 : reserved for future

    SET $11 10 0xxx
	(8 Operations range, but defined 4 operations, 0 modifier)
	carry : 000
	zero  : 001
	negative  : 010
	overflow : 011
	interrupt : 100
	stack: 101
	direction: 110
	XXX  : 111 : reserved for future flags

    CLEAR $11 10 1xxx
	(8 Operations range, but defined 4 operations, 0 modifier)
	carry : 000
	zero  : 001
	negative  : 010
	overflow : 011
	interrupt: 100
	stack: 101
	direction: 110
	XXX : 111 : reseerved for future flags

    RESERVED $11 11 0000
    RESERVED $11 11 0001
    RESERVED $11 11 0010
    RESERVED $11 11 0011
    RESERVED $11 11 0100
    RESERVED $11 11 0101
    RESERVED $11 11 0110
    RESERVED $11 11 0111
    RESERVED $11 11 1000
    RESERVED $11 11 1001
    RESERVED $11 11 1010


    INT $11 11 1010
	(1 operations, 16/256 modifiers)
	0000 xxxx interrupt number
	0000 0000 Break point (Inner Debug) same as BRKP
	0000 0001 Timer tick
	0000 1111 kernel service
	0000 1xxx : device IRQ (8 irq)
	0001 0000
	... (software interrupt) 32 bits mode only
	1111 1111
    BRKP $11 11 1011
	(1 operation , 0 modifier)
	same as INT 0 (but single byte intruction)
    RESERVED   $11 11 1100 : reserved for future use
    RESET $11 11 1101
    WAIT  $11 11 1110
    HLT   $11 11 1111 
	( 1 Operation, 0 modifiers)
	 stop processor 
