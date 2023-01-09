; test -
m1  macro
.loop
    mov w{1},{1}
    jnc .loop    
    endm 
label3  ORG     $F000
        ORG     $1000
        NOP
        JUMP    fin
  NOP
  IN    B0
  OUT   B0
label0:  OUT2  B0
  OUT   $E0
  OUT2  $E0
label1:
  db 12,$13,14,"Hello world",0
  LEAF_W0       fin
  LEAF_W1       label0
  LEAF_W2       label1
  LEAF_W3       label0
  LEAN_W0       label1
  LEAN_W1       label0
  LEAN_W2       label2
  LEAN_W3       label99
  DW  0, $FFFF, $AB, 'a', label1,13
label99  
  m1 0
  m1 1
  include "test.inc"
  CLC
  CLZ
  CLV
  CLN
  STC
  STZ
  STV
  STN

  INT_0
  INT_1
  INT_2
  INT_3
  INT_4
  INT_5
  INT_6
  INT_7
  INT_8
  INT_9
  INT_10
  INT_11
  INT_12
  INT_13
  INT_14
  INT_15
  J_W0
  J_W1
  J_W2
  J_W3
  C_W0
  C_W1
  C_W2
  C_W3
  RET
  IRET
  JUMP  fin
  CALL  fin
  JZ    label1
  JNZ   label2
  JC    label2
  JNC   label2
  JN    label2
  JNN   label2
  JV    label2
  JNV   label2
  JA    label2
  JBE   label2
  JGE   label2
  JL    label2
  JG    label2
  JLE   label2
  J     label2
  C     label2
label2:  
aaaa equ 127
  LD    B0,[62000]
  LD    B0,[$F88F]
  LD    B0,[label2]
  LD    B0,[W2]
  LD    B0,[W2 + $8F]
  LD    B0, [   W2 + $8F]
  LD    B0,[SP + W2]
  LD    B0,[SP + $8F]
  LD    B0,[SP + aaaa ]
  LD    B0,[W2 + W3]
  LD    B0,[SP + W2 + $8F]
  LD    B0,[W2 + W3 + $8F   ]
  POP_B0
  POP_B1
  POP_B2
  POP_B3
  POP_B4
  POP_B5
  POP_B6
  POP_B7
  LD    W0,[$F88F]
  LD    W0,[62000]
  LD    W0,[label2]
  LD    W0,[W2]
  LD    W0,[W2 + aaaa]
  LD    W0,[SP + W2]
  LD    W0,[SP + $8F]
  LD    W0,[W2 + W3]
  LD    W0, [SP + W2 + aaaa  ]
  LD    W0,[W2 + W3 + 26]
  POP_W0
  POP_W1
  POP_W2
  POP_W3
  POPA
  POPF
  POPPC
  DROP
  SR    B0,[$F88F]
  SR    B0,[63432]
  SR    B0,[label2]
  SR    B0,[W2]
  SR    B0,[W2+80]
  SR    B0,[SP + W2]
  SR    B0,[SP + $8F]
  SR    B0,[W2 + W3]
  SR    B0,[SP + W2 + $0]
  SR    B0,[W2 + W3 + $8F]
  PUSH_B0
  PUSH_B1
  PUSH_B2
  PUSH_B3
  PUSH_B4
  PUSH_B5
  PUSH_B6
  PUSH_B7
  SR    W0,[label1]
  SR    W0,[aaaa]
  SR    W0,[W2]
  SR    W0,[W2 + 8]
  SR    W0,[SP + W2]
  SR    W0,[SP + aaaa]
  SR    W0,[W2 + W3]
  SR    W0,[SP + W2 + $8F]
  SR    W0,[W2 + W3 + aaaa]
  PUSH_W0
  PUSH_W1
  PUSH_W2
  PUSH_W3
  PUSHA
  PUSHF
  PUSHPC
  DUP
  AND   B0,B6
  OB    B0,B6
  EOR   B0,B6
  BIC   B0,B6
  TST   B0,B6
  SHL   B0,B6
  SHR   B0,B6
  SAR   B0,B6
  ROR   B0,B6
  ROL   B0,B6
  NOT   B0,B6
  ADD   B0,B6
  ADC   B0,B6
  SUB   B0,B6
  SBB   B0,B6
  CMP   B0,B6
  AND   W0,W2
  OR    W0,W2
  EOR   W0,W2
  BIC   W0,W2
  TST   W0,W2
  SHL   W0,W2
  SHR   W0,W2
  SAR   W0,W2
  ROR   W0,W2
  ROL   W0,W2
  NOT   W0,W2
  ADD   W0,W2
  ADC   W0,W2
  SUB   W0,W2
  SBB   W0,W2
  CMP   W0,W2
  MUL   B0,B6
  IMUL  B0,B6
  DIV   B0,B6
  IDIV  B0,B6
  MOV   B0,B6
  SWP   B0,B6
  MUL   W0,W2
  IMUL  W0,W2
  DIV   W0,W2,W3
  IDIV  W0,W2,W3
  MOV   W0,W2
  MOV   SP,W0
  MOV   W0,SP
  SWP   W0,W2
  AND   B0,$8F
  OR    B0,$8F
  EOR   B0,$8F
  BIC   B0,   aaaa
  TST   B0, 'b'
  SHL   B0
  SHR   B0
  SAR   B0
  INC   B0
  DEC   B0
  NEG   B0
  ADD   B0, $8F
  ADC   B0, $8F
  SUB   B0, aaaa
  SBB   B0,aaaa
  CMP   B0,$8F
  AND   W0,$F88F
  OR    W0,$F88F
  EOR   W0,$F88F
  BIC   W0,$F88F
  TST   W0,aaaa
  SHL   W0
  SHR   W0
  SAR   W0
  INC   W0
  DEC   W0
  NEG   W0
  ADD   W0, 63534
  ADC   W0, -32000
  SUB   W0, 0 
  SBB   W0, -43
  CMP   W0, 2222
  MUL   B0,12,B6
  IMUL  B0,15,B6
  DIV   B0,33,B6
  IDIV  B0,88,B6
  MOV   B0,90
  MUL   W0,$F88F,W2
  IMUL  W0,$F88F,W2
  DIV   W0,aaaa,W2
  IDIV  W0,999,W2
  MOV   W0,$F88F
  MOV   SP,$8FE0
  STI
  CLI
  HEVT
  WAIT
  RESET
        
fin:
    HLT     ; stop CPU