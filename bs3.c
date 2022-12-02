#include <stdlib.h>
#include <stdio.h>

// Instruction set
#define NOP 0x00
#define INB 0x01
#define OUTB 0x02
#define OUTB2 0x03
#define OUTBI 0x04
#define OUTB2I 0x05
#define LEAW0 0x10
#define LEAW1 0x11
#define LEAW2 0x12
#define LEAW3 0x13
#define LEAB0 0x14
#define LEAB1 0x15
#define LEAB2 0x16
#define LEAB3 0x17
#define CLC 0x18
#define CLZ 0x19
#define CLV 0x1A
#define CLN 0x1B
#define STC 0x1C
#define STZ 0x1D
#define STV 0x1E
#define STN 0x1F
#define INT0 0x20
#define INT1 0x21
#define INT2 0x22
#define INT3 0x23
#define INT4 0x24
#define INT5 0x25
#define INT6 0x26
#define INT7 0x27
#define INT8 0x28
#define INT9 0x29
#define INT10 0x2A
#define INT11 0x2B
#define INT12 0x2C
#define INT13 0x2D
#define INT14 0x2E
#define INT15 0x2F
#define J_W0 0x30
#define J_W1 0x31
#define J_W2 0x32
#define J_W3 0x33
#define C_W0 0x34
#define C_W1 0x35
#define C_W2 0x36
#define C_W3 0x37
#define RET 0x3C
#define IRET 0x3D
#define J_A 0x3E
#define C_A 0x3F
#define JZ 0x40
#define JNZ 0x41
#define JC 0x42
#define JNC 0x43
#define JN 0x44
#define JNN 0x45
#define JV 0x46
#define JNV 0x47
#define JA 0x48
#define JBE 0x49
#define JGE 0x4A
#define JL 0x4B
#define JG 0x4C
#define JLE 0x4D
#define J_R 0x4E
#define C_R 0x4F
#define LDBm0 0x50
#define LDBm1 0x51
#define LDBm2 0x52
#define LDBm3 0x53
#define LDBm4 0x54
#define LDBm5 0x55
#define LDBm6 0x56
#define LDBm7 0x57
#define POPB0 0x58
#define POPB1 0x59
#define POPB2 0x5A
#define POPB3 0x5B
#define POPB4 0x5C
#define POPB5 0x5D
#define POPB6 0x5E
#define POPB7 0x5F
#define LDWm0 0x60
#define LDWm1 0x61
#define LDWm2 0x62
#define LDWm3 0x63
#define LDWm4 0x64
#define LDWm5 0x65
#define LDWm6 0x66
#define LDWm7 0x67
#define POPW0 0x68
#define POPW1 0x69
#define POPW2 0x6A
#define POPW3 0x6B
#define POPA 0x6C
#define POPF 0x6D
#define POPPC 0x6E
#define DROP 0x6F
#define SRBm0 0x70
#define SRBm1 0x71
#define SRBm2 0x72
#define SRBm3 0x73
#define SRBm4 0x74
#define SRBm5 0x75
#define SRBm6 0x76
#define SRBm7 0x77
#define PUSHB0 0x78
#define PUSHB1 0x79
#define PUSHB2 0x7A
#define PUSHB3 0x7B
#define PUSHB4 0x7C
#define PUSHB5 0x7D
#define PUSHB6 0x7E
#define PUSHB7 0x7F
#define SRWm0 0x80
#define SRWm1 0x81
#define SRWm2 0x82
#define SRWm3 0x83
#define SRWm4 0x84
#define SRWm5 0x85
#define SRWm6 0x86
#define SRWm7 0x87
#define PUSHW0 0x88
#define PUSHW1 0x89
#define PUSHW2 0x8A
#define PUSHW3 0x8B
#define PUSHA 0x8C
#define PUSHF 0x8D
#define PUSHPC 0x8E
#define DUP 0x8F
#define ANDB 0x90
#define ORB 0x91
#define EORB 0x92
#define BICB 0x93
#define TSTB 0x94
#define SHLB 0x95
#define SHRB  0x96
#define SARB 0x97
#define RORB 0x98
#define ROLB 0x99
#define NOTB 0x9A
#define ADDB 0x9B
#define ADCB 0x9C
#define SUBB 0x9D
#define SBBB 0x9E
#define CMPB 0x9F
#define ANDW 0xA0
#define ORW 0xA1
#define EORW 0xA2
#define BICW 0xA3
#define TSTW 0xA4
#define SHLW 0xA5
#define SHRW 0xA6
#define SARW 0xA7
#define RORW 0xA8
#define ROLW 0xA9
#define NOTW 0xAA
#define ADDW 0xAB
#define ADCW 0xAC
#define SUBW 0xAD
#define SBBW 0xAE
#define CMPW 0xAF
#define MULB 0xB0
#define IMULB 0xB1
#define DIVB 0xB2
#define IDIVB 0xB3
#define MOVB 0xB6
#define SWPB 0xB7
#define MULW 0xB8
#define IMULW 0xB9
#define DIVW 0xBA
#define IDIVW 0xBB
#define MOVW 0xBC
#define MOVSW 0xBD
#define MOVWS 0xBE
#define SWPW 0xBF
#define ANDBI 0xC0
#define ORBI 0xC1
#define EORBI 0xC2
#define BICBI 0xC3
#define TSTBI 0xC4
#define SHLB1 0xC5
#define SHRB1 0xC6
#define SARB1 0xC7
#define INCB 0xC8
#define DECB 0xC9
#define NEGB 0xCA
#define ADDBI 0xCB
#define ADCBI 0xCC
#define SUBBI 0xCD
#define SBBBI 0xCE
#define CMPBI 0xCF
#define ANDWI 0xD0
#define ORWI 0xD1
#define EORWI 0xD2
#define BICWI 0xD3
#define TSTWI 0xD4
#define SHLW1 0xD5
#define SHRW1 0xD6
#define SARW1 0xD7
#define INCW 0xD8
#define DECW 0xD9
#define NEGW 0xDA
#define ADDWI 0xDB
#define ADCWI 0xDC
#define SUBWI 0xDD
#define SBBWI 0xDE
#define CMPWI 0xDF
#define MULBI 0xE0
#define IMULBI 0xE1
#define DIVBI 0xE2
#define IDIVBI 0xE3
#define MOVBI 0xE6
#define MULWI 0xE8
#define IMULWI 0xE9
#define DIVWI 0xEA
#define IDIVWI 0xEB
#define MOVWI 0xEC
#define STI 0xFA
#define CLI 0xFB
#define HEVT 0xFC
#define WAIT 0xFD
#define RESET 0xFE
#define HLT 0xFF

#define BS3_STATUS_DEFAULT 0
#define BS3_STATUS_HALT 1
#define BS3_STATUS_WAIT 2
#define BS3_STATUS_HEVT 3
#define BS3_STATUS_RESET 4
#define BS3_STATUS_ESCHYP 5

#define BS3_INT_BADINSTR 7
#define BS3_INT_TIMER 3
#define BS3_INT_INPUTEVT 4

#define BYTE unsigned char
#define WORD unsigned short
#define BIT unsigned short
#define DWORD unsigned long

struct bs3_registers
{
  union
  {
	  WORD PC;
    struct
    {
      BYTE LPC;
      BYTE HPC;
    };
  };
	union
  {
    WORD FL;
    struct 
    {
      BIT C:1;
      BIT Z:1;
      BIT V:1;
      BIT N:1;
      BIT I:1;
      BIT reserved:11;
    };
  };
  union
  {
	  WORD SP;
    struct
    {
      BYTE LSP;
      BYTE HSP;
    };
  };
	union 
	{
		WORD W[4];
		BYTE B[8];
	};
};

union opParam
{
  BYTE param;
  struct
  {
    BIT x2:2;
    BIT resx2:6;
  };
  struct
  {
    BIT x3:3;
    BIT resx3:5;
  };
  struct
  {
    BIT resy21:4;
    BIT y2:2;
    BIT resy22:2;
  };
  struct
  {
    BIT resy31:4;
    BIT y3:3;
    BIT resy32:1;
  };
  struct
  {
    BIT resz2:6;
    BIT z2:2;
  };
};

struct bs3_cpu_data
{
  struct bs3_registers r;
  union
  {
    BYTE m[65536];
    struct {
      WORD vector[16]; // interrupt vectors
      BYTE input_data;   // core I/O input
      BYTE input_ready;  // core I/O input status
      BYTE output_data;  // core I/O output
      BYTE output_ready; // core I/O output status
      BYTE output2_data; // core I/O auxiliary output
      BYTE output2_ready; // core I/O auxilairy output status
      DWORD timer;        // system timer 
      BYTE input_datablock_ready;
      BYTE reserved;
      WORD input_datablock_address;
      WORD input_datablock_length;
      WORD hypervisor_event_id;
      BYTE hypervisor_event_data[206];
    };
  };
  BYTE status;
  DWORD counter; // countdown counter intialized at 'timer' value
  BYTE pending_interrupt;
};

// initialize CPU (memory not initialized)
void bs3_cpu_init(struct bs3_cpu_data * pbs3)
{
  // PC = [0000]
  pbs3->r.PC =pbs3->vector[0];
  // CZVN and I = 0 (disallow interruption)
  pbs3->r.FL = 0;
  // SP = 0x0400
  pbs3->r.SP = 0x0400;
  // Wx = 0 -> Bx = 0
  pbs3->r.W[0] = 0;
  pbs3->r.W[1] = 0;
  pbs3->r.W[2] = 0;
  pbs3->r.W[3] = 0;
  // reset status and counter/timer
  pbs3->status = BS3_STATUS_DEFAULT;
  pbs3->counter = 0;
  pbs3->timer = 0;
}


// for interrupt from external (not by program INT instruction)
void bs3_cpu_interrupt(struct bs3_cpu_data * pbs3, int intnum)
{
  if (intnum == BS3_INT_BADINSTR || pbs3->r.I == 0)
  {
    // PUSH PC,
     pbs3->r.SP-=2;
     *(WORD *)&(pbs3->m[ pbs3->r.SP ]) = pbs3->r.PC;
    // PUSH FL
     pbs3->r.SP-=2;
     *(WORD *)&(pbs3->m[ pbs3->r.SP ]) = pbs3->r.FL;
    //PC=m[intnum << 1]
    pbs3->r.PC = pbs3->vector[intnum]; 
    // I=0
    pbs3->r.I = 0;
    pbs3->pending_interrupt = 0xFF;
  } else {
    pbs3->pending_interrupt = intnum;
  }
}

// to be called by Hypervisor
void bs3_cpu_input_data(struct bs3_cpu_data * pbs3, BYTE data) 
{
  pbs3->input_data = data;
  pbs3->input_data_ready = 0;
  bs3_cpu_interrupt( pbs3, BS3_INT_INPUTEVT);
}

// to be called by hypervisor
void bs3_cpu_input_status(struct bs3_cpu_data * pbs3, BYTE status)
{
  pbs3->input_ready = status;
}

// to be called by hypervisor
void bs3_cpu_output_status(struct bs3_cpu_data * pbs3, BYTE status)
{
  pbs3->output_ready = status;
}

// to be called by hypervisor
void bs3_cpu_output2_status(struct bs3_cpu_data * pbs3, BYTE status)
{
  pbs3->output2_ready = status;
}

void bs3_cpu(struct bs3_cpu_data * pbs3)
{
  // is there pending interrupt ?
  if (pbs3->r.I == 0 && pbs3->pending_interrupt != 0xFF) {
    bs3_cpu_interrupt(pbs3, pbs3->pending_interrupt);
  } 
  else 
  {
    if (pbs3->r.I == 0)
      if (--pbs3->counter == 0) 
      {
        pbs3->counter = pbs3->timer;
         bs3_cpu_interrupt(pbs3, BS3_INT_TIMER);
      } 
  }
  switch ( pbs3->m[pbs3->r.PC] ) 
  {
        case NOP:
            pbs3->r.PC++;
            break;
        case INB:
            break;
        case OUTB:
            break;
        case OUTB2:
            break;
        case OUTBI:
            break;
        case OUTB2I:
            break;
        case LEAW0:
            break;
        case LEAW1:
            break;
        case LEAW2:
            break;
        case LEAW3:
            break;
        case LEAB0:
            break;
        case LEAB1:
            break;
        case LEAB2:
            break;
        case LEAB3:
            break;
        case CLC:
            break;
        case CLZ:
            break;
        case CLV:
            break;
        case CLN:
            break;
        case STC:
            break;
        case STZ:
            break;
        case STV:
            break;
        case STN:
            break;
        case INT0:
            break;
        case INT1:
            break;
        case INT2:
            break;
        case INT3:
            break;
        case INT4:
            break;
        case INT5:
            break;
        case INT6:
            break;
        case INT7:
            break;
        case INT8:
            break;
        case INT9:
            break;
        case INT10:
            break;
        case INT11:
            break;
        case INT12:
            break;
        case INT13:
            break;
        case INT14:
            break;
        case INT15:
            break;
        case J_W0:
            break;
        case J_W1:
            break;
        case J_W2:
            break;
        case J_W3:
            break;
        case C_W0:
            break;
        case C_W1:
            break;
        case C_W2:
            break;
        case C_W3:
            break;
        case RET:
            break;
        case IRET:
            break;
        case J_A:
            break;
        case C_A:
            break;
        case JZ:
            break;
        case JNZ:
            break;
        case JC:
            break;
        case JNC:
            break;
        case JN:
            break;
        case JNN:
            break;
        case JV:
            break;
        case JNV:
            break;
        case JA:
            break;
        case JBE:
            break;
        case JGE:
            break;
        case JL:
            break;
        case JG:
            break;
        case JLE:
            break;
        case J_R:
            break;
        case C_R:
            break;
        case LDBm0:
            break;
        case LDBm1:
            break;
        case LDBm2:
            break;
        case LDBm3:
            break;
        case LDBm4:
            break;
        case LDBm5:
            break;
        case LDBm6:
            break;
        case LDBm7:
            break;
        case POPB0:
            break;
        case POPB1:
            break;
        case POPB2:
            break;
        case POPB3:
            break;
        case POPB4:
            break;
        case POPB5:
            break;
        case POPB6:
            break;
        case POPB7:
            break;
        case LDWm0:
            break;
        case LDWm1:
            break;
        case LDWm2:
            break;
        case LDWm3:
            break;
        case LDWm4:
            break;
        case LDWm5:
            break;
        case LDWm6:
            break;
        case LDWm7:
            break;
        case POPW0:
            break;
        case POPW1:
            break;
        case POPW2:
            break;
        case POPW3:
            break;
        case POPA:
            break;
        case POPF:
            break;
        case POPPC:
            break;
        case DROP:
            break;
        case SRBm0:
            break;
        case SRBm1:
            break;
        case SRBm2:
            break;
        case SRBm3:
            break;
        case SRBm4:
            break;
        case SRBm5:
            break;
        case SRBm6:
            break;
        case SRBm7:
            break;
        case PUSHB0:
            break;
        case PUSHB1:
            break;
        case PUSHB2:
            break;
        case PUSHB3:
            break;
        case PUSHB4:
            break;
        case PUSHB5:
            break;
        case PUSHB6:
            break;
        case PUSHB7:
            break;
        case SRWm0:
            break;
        case SRWm1:
            break;
        case SRWm2:
            break;
        case SRWm3:
            break;
        case SRWm4:
            break;
        case SRWm5:
            break;
        case SRWm6:
            break;
        case SRWm7:
            break;
        case PUSHW0:
            break;
        case PUSHW1:
            break;
        case PUSHW2:
            break;
        case PUSHW3:
            break;
        case PUSHA:
            break;
        case PUSHF:
            break;
        case PUSHPC:
            break;
        case DUP:
            break;
        case ANDB:
            break;
        case ORB:
            break;
        case EORB:
            break;
        case BICB:
            break;
        case TSTB:
            break;
        case SHLB:
            break;
        case SHRB:
            break;
        case SARB:
            break;
        case RORB:
            break;
        case ROLB:
            break;
        case NOTB:
            break;
        case ADDB:
            break;
        case ADCB:
            break;
        case SUBB:
            break;
        case SBBB:
            break;
        case CMPB:
            break;
        case ANDW:
            break;
        case ORW:
            break;
        case EORW:
            break;
        case BICW:
            break;
        case TSTW:
            break;
        case SHLW:
            break;
        case SHRW:
            break;
        case SARW:
            break;
        case RORW:
            break;
        case ROLW:
            break;
        case NOTW:
            break;
        case ADDW:
            break;
        case ADCW:
            break;
        case SUBW:
            break;
        case SBBW:
            break;
        case CMPW:
            break;
        case MULB:
            break;
        case IMULB:
            break;
        case DIVB:
            break;
        case IDIVB:
            break;
        case MOVB:
            break;
        case SWPB:
            break;
        case MULW:
            break;
        case IMULW:
            break;
        case DIVW:
            break;
        case IDIVW:
            break;
        case MOVW:
            break;
        case MOVSW:
            break;
        case MOVWS:
            break;
        case SWPW:
            break;
        case ANDBI:
            break;
        case ORBI:
            break;
        case EORBI:
            break;
        case BICBI:
            break;
        case TSTBI:
            break;
        case SHLB1:
            break;
        case SHRB1:
            break;
        case SARB1:
            break;
        case INCB:
            break;
        case DECB:
            break;
        case NEGB:
            break;
        case ADDBI:
            break;
        case ADCBI:
            break;
        case SUBBI:
            break;
        case SBBBI:
            break;
        case CMPBI:
            break;
        case ANDWI:
            break;
        case ORWI:
            break;
        case EORWI:
            break;
        case BICWI:
            break;
        case TSTWI:
            break;
        case SHLW1:
            break;
        case SHRW1:
            break;
        case SARW1:
            break;
        case INCW:
            break;
        case DECW:
            break;
        case NEGW:
            break;
        case ADDWI:
            break;
        case ADCWI:
            break;
        case SUBWI:
            break;
        case SBBWI:
            break;
        case CMPWI:
            break;
        case MULBI:
            break;
        case IMULBI:
            break;
        case DIVBI:
            break;
        case IDIVBI:
            break;
        case MOVBI:
            break;
        case MULWI:
            break;
        case IMULWI:
            break;
        case DIVWI:
            break;
        case IDIVWI:
            break;
        case MOVWI:
            break;
        case STI:
            break;
        case CLI:
            break;
        case HEVT:
            break;
        case WAIT:
            break;
        case RESET:
            break;
        case HLT:
            break;
        default:
            bs3_cpu_interrupt(pbs3, BS3_INT_BADINSTR);

  }
}

void main() {
  struct bs3_registers reg;
  reg.PC = 0xFAAF;
  reg.FL = 0x005A;
  reg.SP = 0x0400;
  reg.W[0] = 0xAAFF;
  printf("PC %X\n",reg.PC);
  printf("FL %X\n",reg.FL);
  printf("SP %X\n",reg.SP);
  printf("W[0] %X\n",reg.W[0]);
  printf("B[0] %X\n",reg.B[0]);
  printf("B[1] %X\n",reg.B[1]);
  printf("C %X\n",reg.C);
  printf("Z %X\n",reg.Z);
  printf("V %X\n",reg.V);
  printf("N %X\n",reg.N);
  printf("I %X\n",reg.I);
  printf("reserved %X\n",reg.reserved);
}
