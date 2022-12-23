#ifndef _BS3_H
#define _BS3_H 1

/* Instruction set */
#define BS3_INSTR_LAST 0x107

#define BS3_INSTR_NOP 0x00
#define BS3_INSTR_INB 0x01
#define BS3_INSTR_OUTB 0x02
#define BS3_INSTR_OUTB2 0x03
#define BS3_INSTR_OUTBI 0x04
#define BS3_INSTR_OUTB2I 0x05
#define BS3_INSTR_LEAW0 0x10
#define BS3_INSTR_LEAW1 0x11
#define BS3_INSTR_LEAW2 0x12
#define BS3_INSTR_LEAW3 0x13
#define BS3_INSTR_LEAB0 0x14
#define BS3_INSTR_LEAB1 0x15
#define BS3_INSTR_LEAB2 0x16
#define BS3_INSTR_LEAB3 0x17
#define BS3_INSTR_CLC 0x18
#define BS3_INSTR_CLZ 0x19
#define BS3_INSTR_CLV 0x1A
#define BS3_INSTR_CLN 0x1B
#define BS3_INSTR_STC 0x1C
#define BS3_INSTR_STZ 0x1D
#define BS3_INSTR_STV 0x1E
#define BS3_INSTR_STN 0x1F
#define BS3_INSTR_INT0 0x20
#define BS3_INSTR_INT1 0x21
#define BS3_INSTR_INT2 0x22
#define BS3_INSTR_INT3 0x23
#define BS3_INSTR_INT4 0x24
#define BS3_INSTR_INT5 0x25
#define BS3_INSTR_INT6 0x26
#define BS3_INSTR_INT7 0x27
#define BS3_INSTR_INT8 0x28
#define BS3_INSTR_INT9 0x29
#define BS3_INSTR_INT10 0x2A
#define BS3_INSTR_INT11 0x2B
#define BS3_INSTR_INT12 0x2C
#define BS3_INSTR_INT13 0x2D
#define BS3_INSTR_INT14 0x2E
#define BS3_INSTR_INT15 0x2F
#define BS3_INSTR_J_W0 0x30
#define BS3_INSTR_J_W1 0x31
#define BS3_INSTR_J_W2 0x32
#define BS3_INSTR_J_W3 0x33
#define BS3_INSTR_C_W0 0x34
#define BS3_INSTR_C_W1 0x35
#define BS3_INSTR_C_W2 0x36
#define BS3_INSTR_C_W3 0x37
#define BS3_INSTR_RET 0x3C
#define BS3_INSTR_IRET 0x3D
#define BS3_INSTR_J_A 0x3E
#define BS3_INSTR_C_A 0x3F
#define BS3_INSTR_JZ 0x40
#define BS3_INSTR_JNZ 0x41
#define BS3_INSTR_JC 0x42
#define BS3_INSTR_JNC 0x43
#define BS3_INSTR_JN 0x44
#define BS3_INSTR_JNN 0x45
#define BS3_INSTR_JV 0x46
#define BS3_INSTR_JNV 0x47
#define BS3_INSTR_JA 0x48
#define BS3_INSTR_JBE 0x49
#define BS3_INSTR_JGE 0x4A
#define BS3_INSTR_JL 0x4B
#define BS3_INSTR_JG 0x4C
#define BS3_INSTR_JLE 0x4D
#define BS3_INSTR_J_R 0x4E
#define BS3_INSTR_C_R 0x4F
#define BS3_INSTR_LDBm0 0x50
#define BS3_INSTR_LDBm1 0x51
#define BS3_INSTR_LDBm2 0x52
#define BS3_INSTR_LDBm3 0x53
#define BS3_INSTR_LDBm4 0x54
#define BS3_INSTR_LDBm5 0x55
#define BS3_INSTR_LDBm6 0x56
#define BS3_INSTR_LDBm7 0x57
#define BS3_INSTR_POPB0 0x58
#define BS3_INSTR_POPB1 0x59
#define BS3_INSTR_POPB2 0x5A
#define BS3_INSTR_POPB3 0x5B
#define BS3_INSTR_POPB4 0x5C
#define BS3_INSTR_POPB5 0x5D
#define BS3_INSTR_POPB6 0x5E
#define BS3_INSTR_POPB7 0x5F
#define BS3_INSTR_LDWm0 0x60
#define BS3_INSTR_LDWm1 0x61
#define BS3_INSTR_LDWm2 0x62
#define BS3_INSTR_LDWm3 0x63
#define BS3_INSTR_LDWm4 0x64
#define BS3_INSTR_LDWm5 0x65
#define BS3_INSTR_LDWm6 0x66
#define BS3_INSTR_LDWm7 0x67
#define BS3_INSTR_POPW0 0x68
#define BS3_INSTR_POPW1 0x69
#define BS3_INSTR_POPW2 0x6A
#define BS3_INSTR_POPW3 0x6B
#define BS3_INSTR_POPA 0x6C
#define BS3_INSTR_POPF 0x6D
#define BS3_INSTR_POPPC 0x6E
#define BS3_INSTR_DROP 0x6F
#define BS3_INSTR_SRBm0 0x70
#define BS3_INSTR_SRBm1 0x71
#define BS3_INSTR_SRBm2 0x72
#define BS3_INSTR_SRBm3 0x73
#define BS3_INSTR_SRBm4 0x74
#define BS3_INSTR_SRBm5 0x75
#define BS3_INSTR_SRBm6 0x76
#define BS3_INSTR_SRBm7 0x77
#define BS3_INSTR_PUSHB0 0x78
#define BS3_INSTR_PUSHB1 0x79
#define BS3_INSTR_PUSHB2 0x7A
#define BS3_INSTR_PUSHB3 0x7B
#define BS3_INSTR_PUSHB4 0x7C
#define BS3_INSTR_PUSHB5 0x7D
#define BS3_INSTR_PUSHB6 0x7E
#define BS3_INSTR_PUSHB7 0x7F
#define BS3_INSTR_SRWm0 0x80
#define BS3_INSTR_SRWm1 0x81
#define BS3_INSTR_SRWm2 0x82
#define BS3_INSTR_SRWm3 0x83
#define BS3_INSTR_SRWm4 0x84
#define BS3_INSTR_SRWm5 0x85
#define BS3_INSTR_SRWm6 0x86
#define BS3_INSTR_SRWm7 0x87
#define BS3_INSTR_PUSHW0 0x88
#define BS3_INSTR_PUSHW1 0x89
#define BS3_INSTR_PUSHW2 0x8A
#define BS3_INSTR_PUSHW3 0x8B
#define BS3_INSTR_PUSHA 0x8C
#define BS3_INSTR_PUSHF 0x8D
#define BS3_INSTR_PUSHPC 0x8E
#define BS3_INSTR_DUP 0x8F
#define BS3_INSTR_ANDB 0x90
#define BS3_INSTR_ORB 0x91
#define BS3_INSTR_EORB 0x92
#define BS3_INSTR_BICB 0x93
#define BS3_INSTR_TSTB 0x94
#define BS3_INSTR_SHLB 0x95
#define BS3_INSTR_SHRB  0x96
#define BS3_INSTR_SARB 0x97
#define BS3_INSTR_RORB 0x98
#define BS3_INSTR_ROLB 0x99
#define BS3_INSTR_NOTB 0x9A
#define BS3_INSTR_ADDB 0x9B
#define BS3_INSTR_ADCB 0x9C
#define BS3_INSTR_SUBB 0x9D
#define BS3_INSTR_SBBB 0x9E
#define BS3_INSTR_CMPB 0x9F
#define BS3_INSTR_ANDW 0xA0
#define BS3_INSTR_ORW 0xA1
#define BS3_INSTR_EORW 0xA2
#define BS3_INSTR_BICW 0xA3
#define BS3_INSTR_TSTW 0xA4
#define BS3_INSTR_SHLW 0xA5
#define BS3_INSTR_SHRW 0xA6
#define BS3_INSTR_SARW 0xA7
#define BS3_INSTR_RORW 0xA8
#define BS3_INSTR_ROLW 0xA9
#define BS3_INSTR_NOTW 0xAA
#define BS3_INSTR_ADDW 0xAB
#define BS3_INSTR_ADCW 0xAC
#define BS3_INSTR_SUBW 0xAD
#define BS3_INSTR_SBBW 0xAE
#define BS3_INSTR_CMPW 0xAF
#define BS3_INSTR_MULB 0xB0
#define BS3_INSTR_IMULB 0xB1
#define BS3_INSTR_DIVB 0xB2
#define BS3_INSTR_IDIVB 0xB3
#define BS3_INSTR_MOVB 0xB6
#define BS3_INSTR_SWPB 0xB7
#define BS3_INSTR_MULW 0xB8
#define BS3_INSTR_IMULW 0xB9
#define BS3_INSTR_DIVW 0xBA
#define BS3_INSTR_IDIVW 0xBB
#define BS3_INSTR_MOVW 0xBC
#define BS3_INSTR_MOVSW 0xBD
#define BS3_INSTR_MOVWS 0xBE
#define BS3_INSTR_SWPW 0xBF
#define BS3_INSTR_ANDBI 0xC0
#define BS3_INSTR_ORBI 0xC1
#define BS3_INSTR_EORBI 0xC2
#define BS3_INSTR_BICBI 0xC3
#define BS3_INSTR_TSTBI 0xC4
#define BS3_INSTR_SHLB1 0xC5
#define BS3_INSTR_SHRB1 0xC6
#define BS3_INSTR_SARB1 0xC7
#define BS3_INSTR_INCB 0xC8
#define BS3_INSTR_DECB 0xC9
#define BS3_INSTR_NEGB 0xCA
#define BS3_INSTR_ADDBI 0xCB
#define BS3_INSTR_ADCBI 0xCC
#define BS3_INSTR_SUBBI 0xCD
#define BS3_INSTR_SBBBI 0xCE
#define BS3_INSTR_CMPBI 0xCF
#define BS3_INSTR_ANDWI 0xD0
#define BS3_INSTR_ORWI 0xD1
#define BS3_INSTR_EORWI 0xD2
#define BS3_INSTR_BICWI 0xD3
#define BS3_INSTR_TSTWI 0xD4
#define BS3_INSTR_SHLW1 0xD5
#define BS3_INSTR_SHRW1 0xD6
#define BS3_INSTR_SARW1 0xD7
#define BS3_INSTR_INCW 0xD8
#define BS3_INSTR_DECW 0xD9
#define BS3_INSTR_NEGW 0xDA
#define BS3_INSTR_ADDWI 0xDB
#define BS3_INSTR_ADCWI 0xDC
#define BS3_INSTR_SUBWI 0xDD
#define BS3_INSTR_SBBWI 0xDE
#define BS3_INSTR_CMPWI 0xDF
#define BS3_INSTR_MULBI 0xE0
#define BS3_INSTR_IMULBI 0xE1
#define BS3_INSTR_DIVBI 0xE2
#define BS3_INSTR_IDIVBI 0xE3
#define BS3_INSTR_MOVBI 0xE6
#define BS3_INSTR_MULWI 0xE8
#define BS3_INSTR_IMULWI 0xE9
#define BS3_INSTR_DIVWI 0xEA
#define BS3_INSTR_IDIVWI 0xEB
#define BS3_INSTR_MOVWI 0xEC
#define BS3_INSTR_MOVSI 0xED
#define BS3_INSTR_STI 0xFA
#define BS3_INSTR_CLI 0xFB
#define BS3_INSTR_HEVT 0xFC
#define BS3_INSTR_WAIT 0xFD
#define BS3_INSTR_RESET 0xFE
#define BS3_INSTR_HLT 0xFF

#define BS3_INSTR_DB  0x100
#define BS3_INSTR_DW  0x101
#define BS3_INSTR_EQU 0x102
#define BS3_INSTR_DIST 0x103
#define BS3_INSTR_ORG 0x104
#define BS3_INSTR_MACRO 0x105
#define BS3_INSTR_ENDM 0x106
#define BS3_INSTR_INCLUDE 0x107

/* BS3 CPU Status */
#define BS3_STATUS_DEFAULT  0
#define BS3_STATUS_HALT     1
#define BS3_STATUS_WAIT     2
#define BS3_STATUS_HEVT     3
#define BS3_STATUS_RESET    4
#define BS3_STATUS_ESCHYP   5
#define BS3_STATUS_DIVZERO  6
#define BS3_STATUS_BADINSTR 7

/* IRQ and other Interrupt state/reference */
#define BS3_INT_NOPENDING   0xFF
#define BS3_INT_BADINSTR    7
#define BS3_INT_TIMER       3
#define BS3_INT_INPUT       4
#define BS3_INT_DIVZERO     6

/* BS3 internal types */
#define BYTE    unsigned  char
#define WORD    unsigned  short
#define DWORD   unsigned  long
#define SBYTE   signed    char
#define SWORD   signed    short
#define SDWORD  signed    long
#define BIT     unsigned  short


/* BS3 CPU Registers structure */
struct bs3_registers
{
  WORD PC;
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
  WORD SP;
	union 
	{
		WORD W[4];
		BYTE B[8];
	};
};

/* operator param (optional second byte of an instruction) */
union opParam
{
  BYTE param;
  struct /* Wx, Wy and Wz reference in opParam */
  {
    BIT x2:2;
    BIT resx2:2;
    BIT y2:2;
    BIT z2:2;
  };
  struct /* Bx and By reference in opParam */
  {
    BIT x3:3;
    BIT resx3:1;
    BIT y3:3;
    BIT resy3:1;
  };
};

/* BS3 CPU Structure */
struct bs3_cpu_data
{
  struct bs3_registers r;
  union
  {
    BYTE m[65536];
    struct {
      WORD vector[16]; /* interrupt vectors */
      BYTE input_datablock_ready;
      BYTE reserved;
      WORD input_datablock_address;
      WORD input_datablock_length;
      BYTE hypervisor_reserved[10];
      WORD hypervisor_event_id;
      BYTE hypervisor_event_data[206];
      BYTE input_data;   /* core I/O input */
      BYTE input_ready;  /* core I/O input status */
      BYTE output_data;  /* core I/O output */
      BYTE output_ready; /* core I/O output status */
      BYTE output2_data; /* core I/O auxiliary output */
      BYTE output2_ready; /* core I/O auxilairy output status */
      DWORD timer;        /* system timer */ 
      BYTE msortick; /* microsecond(0) or cpu clock(1) */ 
    };
  };
  BYTE status;
  DWORD counter; /* countdown counter intialized at 'timer' value */
  BYTE pending_interrupt;
};

/* BS3 CPU disassemble instruction definition structure */
struct bs3_cpu_instr
{
  const char * fullName;  // Machine name (e.g MOVWI)
  const char * name;      // Human name   ( e.g MOV )
  int  nbParam; /* mnemonic number of param */
  union {
    BYTE ptype[3];
    struct {
      BYTE  p1type;
      BYTE  p2type;
      BYTE  p3type;
    };
  };
  const char * asmpattern; /* assembly coding pattern */
      /* coding string char, interpret each char by :
           o : operator code (BYTE)
           p : operator parameter (BYTE)
           b : byte immediate (BYTE)
           w : word immediate / address label (WORD)
           r : signed byte immediate representing PC relative address, provided by label (SBYTE)
           R : signed word immediate representing PC relative address, provided by label (SWORD)
           x : nothing to assemble
           * : (after b or w) multiple byte or word
      */  
  int  opeType; 
  int  size;          // from 1 to 4
  int  hasParam;      // 1 or 0
  int  hasImmediate;  // 1 or 0
  char immType; // 'B' or 'W'
  const char * operandFormat; // format parameter a b c d x X y Y z @ o
  //a for byte at PC in 2 hexa characters
  //b for byte at PC+1 in 2 hexa characters
  //c for byte at PC+2 in 2 hexa characters
  //d for byte at PC+3 in 2 hexa characters
  //x for param x2 in one digit from 0 to 3 
  //X for param x3 in one digit from 0 to 7
  //y for param y2 in one digit from 0 to 3
  //Y for param y3 in one digit from 0 to 7
  //z for param z2 in one digit from 0 to 3
  //@ for PC + SimmB/W in 4 hexacharacter
  //o for instruction name(short) in 5 characters (right space tail padding)
};


extern struct bs3_cpu_instr bs3Instr[];


WORD bs3_cpu_disassemble(WORD PC, BYTE a, BYTE b, BYTE c, BYTE d, char * result);
void bs3_cpu_state(struct bs3_cpu_data * pbs3, char * result);


void bs3_hyper_timerset(DWORD microseconds);
void bs3_hyper_timerstop();
void bs3_cpu_init(struct bs3_cpu_data * pbs3);
void bs3_cpu_interrupt(struct bs3_cpu_data * pbs3, int intnum);
void bs3_cpu_write_byte(struct bs3_cpu_data * pbs3, WORD address, BYTE data);
BYTE bs3_cpu_read_byte(struct bs3_cpu_data * pbs3, WORD address);
void bs3_cpu_write_word(struct bs3_cpu_data * pbs3, WORD address, WORD data);
WORD bs3_cpu_read_word(struct bs3_cpu_data * pbs3, WORD address);
void bs3_cpu_exec(struct bs3_cpu_data * pbs3);
void bs3_hyper_reset_memory(struct bs3_cpu_data * pbs3);
void bs3_hyper_load_memory(struct bs3_cpu_data * pbs3, BYTE * data, long length, WORD address);
void bs3_hyper_coreIO(struct bs3_cpu_data * pbs3);
void bs3_hyper_main(BYTE * program, WORD programsize);

#endif