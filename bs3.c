#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <errno.h>

/* Instruction set */
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

#define BS3_STATUS_DEFAULT  0
#define BS3_STATUS_HALT     1
#define BS3_STATUS_WAIT     2
#define BS3_STATUS_HEVT     3
#define BS3_STATUS_RESET    4
#define BS3_STATUS_ESCHYP   5
#define BS3_STATUS_DIVZERO  6
#define BS3_STATUS_BADINSTR 7

#define BS3_INT_NOPENDING   0xFF
#define BS3_INT_BADINSTR    7
#define BS3_INT_TIMER       3
#define BS3_INT_INPUT       4
#define BS3_INT_DIVZERO     6

#define BYTE unsigned char
#define SBYTE signed char
#define WORD unsigned short
#define SWORD signed short
#define BIT unsigned short
#define DWORD unsigned long
#define SDWORD signed long

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

struct bs3_cpu_instr
{
  const char * fullName;  // Machine name (e.g MOVWI)
  const char * name;      // Human name   ( e.g MOV )
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

static struct bs3_cpu_instr bs3Instr[]={
[BS3_INSTR_NOP] = { .fullName = "NOP", .name = "NOP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_INB] = { .fullName = "INB", .name = "IN", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTB] = { .fullName = "OUTB", .name = "OUT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTB2] = { .fullName = "OUTB2", .name = "OUT2", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTBI] = { .fullName = "OUTBI", .name = "OUT", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o b"},
[BS3_INSTR_OUTB2I] = { .fullName = "OUTB2I", .name = "OUT2", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o b"},
[0x06] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x07] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x08] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x09] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0A] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0B] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0C] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0D] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0E] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0F] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_LEAW0] = { .fullName = "LEAW0", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W0,@"},
[BS3_INSTR_LEAW1] = { .fullName = "LEAW1", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W1,@"},
[BS3_INSTR_LEAW2] = { .fullName = "LEAW2", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W2,@"},
[BS3_INSTR_LEAW3] = { .fullName = "LEAW3", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W3,@"},
[BS3_INSTR_LEAB0] = { .fullName = "LEAB0", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W0,@"},
[BS3_INSTR_LEAB1] = { .fullName = "LEAB1", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W1,@"},
[BS3_INSTR_LEAB2] = { .fullName = "LEAB2", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W2,@"},
[BS3_INSTR_LEAB3] = { .fullName = "LEAB3", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W3,@"},
[BS3_INSTR_CLC] = { .fullName = "CLC", .name = "CLC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLZ] = { .fullName = "CLZ", .name = "CLZ", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLV] = { .fullName = "CLV", .name = "CLV", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLN] = { .fullName = "CLN", .name = "CLN", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STC] = { .fullName = "STC", .name = "STC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STZ] = { .fullName = "STZ", .name = "STZ", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STV] = { .fullName = "STV", .name = "STV", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STN] = { .fullName = "STN", .name = "STN", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_INT0] = { .fullName = "INT0", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 0"},
[BS3_INSTR_INT1] = { .fullName = "INT1", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 1"},
[BS3_INSTR_INT2] = { .fullName = "INT2", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 2"},
[BS3_INSTR_INT3] = { .fullName = "INT3", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 3"},
[BS3_INSTR_INT4] = { .fullName = "INT4", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 4"},
[BS3_INSTR_INT5] = { .fullName = "INT5", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 5"},
[BS3_INSTR_INT6] = { .fullName = "INT6", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 6"},
[BS3_INSTR_INT7] = { .fullName = "INT7", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 7"},
[BS3_INSTR_INT8] = { .fullName = "INT8", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 8"},
[BS3_INSTR_INT9] = { .fullName = "INT9", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 9"},
[BS3_INSTR_INT10] = { .fullName = "INT10", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 10"},
[BS3_INSTR_INT11] = { .fullName = "INT11", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 11"},
[BS3_INSTR_INT12] = { .fullName = "INT12", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 12"},
[BS3_INSTR_INT13] = { .fullName = "INT13", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 13"},
[BS3_INSTR_INT14] = { .fullName = "INT14", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 14"},
[BS3_INSTR_INT15] = { .fullName = "INT15", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 15"},
[BS3_INSTR_J_W0] = { .fullName = "J_W0", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_J_W1] = { .fullName = "J_W1", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_J_W2] = { .fullName = "J_W2", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_J_W3] = { .fullName = "J_W3", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_C_W0] = { .fullName = "C_W0", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_C_W1] = { .fullName = "C_W1", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_C_W2] = { .fullName = "C_W2", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_C_W3] = { .fullName = "C_W3", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[0x38] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x39] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x3A] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x3B] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_RET] = { .fullName = "RET", .name = "RET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_IRET] = { .fullName = "IRET", .name = "IRET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_J_A] = { .fullName = "J_A", .name = "J", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o cb"},
[BS3_INSTR_C_A] = { .fullName = "C_A", .name = "C", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o cb"},
[BS3_INSTR_JZ] = { .fullName = "JZ", .name = "JZ", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNZ] = { .fullName = "JNZ", .name = "JNZ", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JC] = { .fullName = "JC", .name = "JC", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNC] = { .fullName = "JNC", .name = "JNC", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JN] = { .fullName = "JN", .name = "JN", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNN] = { .fullName = "JNN", .name = "JNN", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JV] = { .fullName = "JV", .name = "JV", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNV] = { .fullName = "JNV", .name = "JNV", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JA] = { .fullName = "JA", .name = "JA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JBE] = { .fullName = "JBE", .name = "JBE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JGE] = { .fullName = "JGE", .name = "JGE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JL] = { .fullName = "JL", .name = "JL", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JG] = { .fullName = "JG", .name = "JG", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JLE] = { .fullName = "JLE", .name = "JLE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_J_R] = { .fullName = "J_R", .name = "J", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_C_R] = { .fullName = "C_R", .name = "C", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_LDBm0] = { .fullName = "LDBm0", .name = "LD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[dc]"},
[BS3_INSTR_LDBm1] = { .fullName = "LDBm1", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy]"},
[BS3_INSTR_LDBm2] = { .fullName = "LDBm2", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + c]"},
[BS3_INSTR_LDBm3] = { .fullName = "LDBm3", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[SP + Wy]"},
[BS3_INSTR_LDBm4] = { .fullName = "LDBm4", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + c]"},
[BS3_INSTR_LDBm5] = { .fullName = "LDBm5", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy + Wz]"},
[BS3_INSTR_LDBm6] = { .fullName = "LDBm6", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + Wy + c]"},
[BS3_INSTR_LDBm7] = { .fullName = "LDBm7", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + Wz + c]"},
[BS3_INSTR_POPB0] = { .fullName = "POPB0", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B0"},
[BS3_INSTR_POPB1] = { .fullName = "POPB1", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B1"},
[BS3_INSTR_POPB2] = { .fullName = "POPB2", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B2"},
[BS3_INSTR_POPB3] = { .fullName = "POPB3", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B3"},
[BS3_INSTR_POPB4] = { .fullName = "POPB4", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B4"},
[BS3_INSTR_POPB5] = { .fullName = "POPB5", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B5"},
[BS3_INSTR_POPB6] = { .fullName = "POPB6", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B6"},
[BS3_INSTR_POPB7] = { .fullName = "POPB7", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B7"},
[BS3_INSTR_LDWm0] = { .fullName = "LDWm0", .name = "LD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,[dc]"},
[BS3_INSTR_LDWm1] = { .fullName = "LDWm1", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy]"},
[BS3_INSTR_LDWm2] = { .fullName = "LDWm2", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + c]"},
[BS3_INSTR_LDWm3] = { .fullName = "LDWm3", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[SP + Wy]"},
[BS3_INSTR_LDWm4] = { .fullName = "LDWm4", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + c]"},
[BS3_INSTR_LDWm5] = { .fullName = "LDWm5", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy + Wz]"},
[BS3_INSTR_LDWm6] = { .fullName = "LDWm6", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + Wy + c]"},
[BS3_INSTR_LDWm7] = { .fullName = "LDWm7", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + Wz + c]"},
[BS3_INSTR_POPW0] = { .fullName = "POPW0", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_POPW1] = { .fullName = "POPW1", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_POPW2] = { .fullName = "POPW2", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_POPW3] = { .fullName = "POPW3", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_POPA] = { .fullName = "POPA", .name = "POPA", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_POPF] = { .fullName = "POPF", .name = "POPF", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_POPPC] = { .fullName = "POPPC", .name = "POPPC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_DROP] = { .fullName = "DROP", .name = "DROP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_SRBm0] = { .fullName = "SRBm0", .name = "SR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[dc]"},
[BS3_INSTR_SRBm1] = { .fullName = "SRBm1", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy]"},
[BS3_INSTR_SRBm2] = { .fullName = "SRBm2", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + c]"},
[BS3_INSTR_SRBm3] = { .fullName = "SRBm3", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[SP + Wy]"},
[BS3_INSTR_SRBm4] = { .fullName = "SRBm4", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + c]"},
[BS3_INSTR_SRBm5] = { .fullName = "SRBm5", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy + Wz]"},
[BS3_INSTR_SRBm6] = { .fullName = "SRBm6", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + Wy + c]"},
[BS3_INSTR_SRBm7] = { .fullName = "SRBm7", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + Wz + c]"},
[BS3_INSTR_PUSHB0] = { .fullName = "PUSHB0", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B0"},
[BS3_INSTR_PUSHB1] = { .fullName = "PUSHB1", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B1"},
[BS3_INSTR_PUSHB2] = { .fullName = "PUSHB2", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B2"},
[BS3_INSTR_PUSHB3] = { .fullName = "PUSHB3", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B3"},
[BS3_INSTR_PUSHB4] = { .fullName = "PUSHB4", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B4"},
[BS3_INSTR_PUSHB5] = { .fullName = "PUSHB5", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B5"},
[BS3_INSTR_PUSHB6] = { .fullName = "PUSHB6", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B6"},
[BS3_INSTR_PUSHB7] = { .fullName = "PUSHB7", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B7"},
[BS3_INSTR_SRWm0] = { .fullName = "SRWm0", .name = "SR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,[dc]"},
[BS3_INSTR_SRWm1] = { .fullName = "SRWm1", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy]"},
[BS3_INSTR_SRWm2] = { .fullName = "SRWm2", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + c]"},
[BS3_INSTR_SRWm3] = { .fullName = "SRWm3", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[SP + Wy]"},
[BS3_INSTR_SRWm4] = { .fullName = "SRWm4", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + c]"},
[BS3_INSTR_SRWm5] = { .fullName = "SRWm5", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy + Wz]"},
[BS3_INSTR_SRWm6] = { .fullName = "SRWm6", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + Wy + c]"},
[BS3_INSTR_SRWm7] = { .fullName = "SRWm7", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + Wz + c]"},
[BS3_INSTR_PUSHW0] = { .fullName = "PUSHW0", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_PUSHW1] = { .fullName = "PUSHW1", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_PUSHW2] = { .fullName = "PUSHW2", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_PUSHW3] = { .fullName = "PUSHW3", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_PUSHA] = { .fullName = "PUSHA", .name = "PUSHA", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_PUSHF] = { .fullName = "PUSHF", .name = "PUSHF", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_PUSHPC] = { .fullName = "PUSHPC", .name = "PUSHPC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_DUP] = { .fullName = "DUP", .name = "DUP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_ANDB] = { .fullName = "ANDB", .name = "AND", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ORB] = { .fullName = "ORB", .name = "OB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_EORB] = { .fullName = "EORB", .name = "EOR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_BICB] = { .fullName = "BICB", .name = "BIC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_TSTB] = { .fullName = "TSTB", .name = "TST", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SHLB] = { .fullName = "SHLB", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SHRB] = { .fullName = "SHRB", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SARB] = { .fullName = "SARB", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_RORB] = { .fullName = "RORB", .name = "ROR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ROLB] = { .fullName = "ROLB", .name = "ROL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_NOTB] = { .fullName = "NOTB", .name = "NOT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ADDB] = { .fullName = "ADDB", .name = "ADD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ADCB] = { .fullName = "ADCB", .name = "ADC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SUBB] = { .fullName = "SUBB", .name = "SUB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SBBB] = { .fullName = "SBBB", .name = "SBB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_CMPB] = { .fullName = "CMPB", .name = "CMP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ANDW] = { .fullName = "ANDW", .name = "AND", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ORW] = { .fullName = "ORW", .name = "OR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_EORW] = { .fullName = "EORW", .name = "EOR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_BICW] = { .fullName = "BICW", .name = "BIC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_TSTW] = { .fullName = "TSTW", .name = "TST", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SHLW] = { .fullName = "SHLW", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SHRW] = { .fullName = "SHRW", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SARW] = { .fullName = "SARW", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_RORW] = { .fullName = "RORW", .name = "ROR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ROLW] = { .fullName = "ROLW", .name = "ROL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_NOTW] = { .fullName = "NOTW", .name = "NOT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ADDW] = { .fullName = "ADDW", .name = "ADD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ADCW] = { .fullName = "ADCW", .name = "ADC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SUBW] = { .fullName = "SUBW", .name = "SUB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SBBW] = { .fullName = "SBBW", .name = "SBB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_CMPW] = { .fullName = "CMPW", .name = "CMP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_MULB] = { .fullName = "MULB", .name = "MUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_IMULB] = { .fullName = "IMULB", .name = "IMUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_DIVB] = { .fullName = "DIVB", .name = "DIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_IDIVB] = { .fullName = "IDIVB", .name = "IDIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[0xB4] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xB5] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_MOVB] = { .fullName = "MOVB", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SWPB] = { .fullName = "SWPB", .name = "SWP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_MULW] = { .fullName = "MULW", .name = "MUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_IMULW] = { .fullName = "IMULW", .name = "IMUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_DIVW] = { .fullName = "DIVW", .name = "DIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy,Wz"},
[BS3_INSTR_IDIVW] = { .fullName = "IDIVW", .name = "IDIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy,Wz"},
[BS3_INSTR_MOVW] = { .fullName = "MOVW", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_MOVSW] = { .fullName = "MOVSW", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o SP,Wx"},
[BS3_INSTR_MOVWS] = { .fullName = "MOVWS", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,SP"},
[BS3_INSTR_SWPW] = { .fullName = "SWPW", .name = "SWP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ANDBI] = { .fullName = "ANDBI", .name = "AND", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ORBI] = { .fullName = "ORBI", .name = "OR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_EORBI] = { .fullName = "EORBI", .name = "EOR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_BICBI] = { .fullName = "BICBI", .name = "BIC", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_TSTBI] = { .fullName = "TSTBI", .name = "TST", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SHLB1] = { .fullName = "SHLB1", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,1"},
[BS3_INSTR_SHRB1] = { .fullName = "SHRB1", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,1"},
[BS3_INSTR_SARB1] = { .fullName = "SARB1", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,1"},
[BS3_INSTR_INCB] = { .fullName = "INCB", .name = "INC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_DECB] = { .fullName = "DECB", .name = "DEC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_NEGB] = { .fullName = "NEGB", .name = "NEG", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_ADDBI] = { .fullName = "ADDBI", .name = "ADD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ADCBI] = { .fullName = "ADCBI", .name = "ADC", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SUBBI] = { .fullName = "SUBBI", .name = "SUB", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SBBBI] = { .fullName = "SBBBI", .name = "SBB", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_CMPBI] = { .fullName = "CMPBI", .name = "CMP", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ANDWI] = { .fullName = "ANDWI", .name = "AND", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_ORWI] = { .fullName = "ORWI", .name = "OR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_EORWI] = { .fullName = "EORWI", .name = "EOR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_BICWI] = { .fullName = "BICWI", .name = "BIC", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_TSTWI] = { .fullName = "TSTWI", .name = "TST", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SHLW1] = { .fullName = "SHLW1", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,1"},
[BS3_INSTR_SHRW1] = { .fullName = "SHRW1", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,1"},
[BS3_INSTR_SARW1] = { .fullName = "SARW1", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,1"},
[BS3_INSTR_INCW] = { .fullName = "INCW", .name = "INC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_DECW] = { .fullName = "DECW", .name = "DEC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_NEGW] = { .fullName = "NEGW", .name = "NEG", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_ADDWI] = { .fullName = "ADDWI", .name = "ADD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_ADCWI] = { .fullName = "ADCWI", .name = "ADC", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SUBWI] = { .fullName = "SUBWI", .name = "SUB", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SBBWI] = { .fullName = "SBBWI", .name = "SBB", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_CMPWI] = { .fullName = "CMPWI", .name = "CMP", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_MULBI] = { .fullName = "MULBI", .name = "MUL", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_IMULBI] = { .fullName = "IMULBI", .name = "IMUL", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_DIVBI] = { .fullName = "DIVBI", .name = "DIV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_IDIVBI] = { .fullName = "IDIVBI", .name = "IDIV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[0xE4] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xE5] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_MOVBI] = { .fullName = "MOVBI", .name = "MOV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[0xE7] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_MULWI] = { .fullName = "MULWI", .name = "MUL", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_IMULWI] = { .fullName = "IMULWI", .name = "IMUL", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_DIVWI] = { .fullName = "DIVWI", .name = "DIV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_IDIVWI] = { .fullName = "IDIVWI", .name = "IDIV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_MOVWI] = { .fullName = "MOVWI", .name = "MOV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_MOVSI] = { .fullName = "MOVSI", .name = "MOV", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o SP,cb"},
[0XEE] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xEF] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF0] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF1] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF2] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF3] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF4] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF5] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF6] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF7] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF8] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF9] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_STI] = { .fullName = "STI", .name = "STI", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLI] = { .fullName = "CLI", .name = "CLI", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_HEVT] = { .fullName = "HEVT", .name = "HEVT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_WAIT] = { .fullName = "WAIT", .name = "WAIT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_RESET] = { .fullName = "RESET", .name = "RESET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_HLT] = { .fullName = "HLT", .name = "HLT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"}
};

static const char * hexa_digit="0123456789ABCDEF";
 
/* disassemble instruction thanks to the 4 bytes (a, b, c and d) located at particular PC address */
/* return the location of the next instruction */  
WORD bs3_cpu_disassemble(WORD PC, BYTE a, BYTE b, BYTE c, BYTE d, char * result) {
  struct bs3_cpu_instr * instr;
  instr = &bs3Instr[a];
  int idx = 0; /* index in 'result' char array */
  /* first part : address */
  result[idx++] = hexa_digit[(PC & 0xF000) >> 12];
  result[idx++] = hexa_digit[(PC & 0x0F00) >> 8];
  result[idx++] = hexa_digit[(PC & 0x00F0) >> 4];
  result[idx++] = hexa_digit[PC & 0x000F];
  result[idx++] = ':';
  result[idx++] = ' ';
  /* second part : instruction byte dump */
  if (instr->size >= 1) { /* for operator : a */
    result[idx++] = hexa_digit[(a & 0x00F0) >> 4];
    result[idx++] = hexa_digit[a & 0x000F];
    result[idx++] = ' ';
  }
  if (instr->size >= 2) { /* for operator param/immediate : b */
    result[idx++] = hexa_digit[(b & 0x00F0) >> 4];
    result[idx++] = hexa_digit[b & 0x000F];
    result[idx++] = ' ';
  }
  else
  {
    result[idx++] = ' ';
    result[idx++] = ' ';
    result[idx++] = ' ';
  } 
  if (instr->size >= 3) { /* for immediate : c */
    result[idx++] = hexa_digit[(c & 0x00F0) >> 4];
    result[idx++] = hexa_digit[c & 0x000F];
    result[idx++] = ' ';
  }
  else
  {
    result[idx++] = ' ';
    result[idx++] = ' ';
    result[idx++] = ' ';
  } 
  if (instr->size == 4) { /* for immediate : d */
    result[idx++] = hexa_digit[(d & 0x00F0) >> 4];
    result[idx++] = hexa_digit[d & 0x000F];
    result[idx++] = ' ';
  }
  else
  {
    result[idx++] = ' ';
    result[idx++] = ' ';
    result[idx++] = ' ';
  } 
  result[idx++] = ' '; /* separator between dump and disassembled operation */
  /* third part : instruction disassembled */
  union opParam oparam;
  oparam.param = b;
  int opfmtidx=0;
  int opidx=0;
  WORD dest=0;
  while (instr->operandFormat[opfmtidx]) 
  {
    switch (instr->operandFormat[opfmtidx])
    {
      case 'a':
        result[idx++] = hexa_digit[(a & 0x00F0) >> 4];
        result[idx++] = hexa_digit[a & 0x000F];
        break;
      case 'b':
        result[idx++] = hexa_digit[(b & 0x00F0) >> 4];
        result[idx++] = hexa_digit[b & 0x000F];
        break;
      case 'c':
        result[idx++] = hexa_digit[(c & 0x00F0) >> 4];
        result[idx++] = hexa_digit[c & 0x000F];
        break;
      case 'd':
        result[idx++] = hexa_digit[(d & 0x00F0) >> 4];
        result[idx++] = hexa_digit[d & 0x000F];
        break;
      case 'x':
        result[idx++] = hexa_digit[oparam.x2 & 0x000F];
        break;
      case 'X':
        result[idx++] = hexa_digit[oparam.x3 & 0x000F];
        break;
      case 'y':
        result[idx++] = hexa_digit[oparam.y2 & 0x000F];
        break;
      case 'Y':
        result[idx++] = hexa_digit[oparam.y3 & 0x000F];
        break;
      case 'z':
        result[idx++] = hexa_digit[oparam.z2 & 0x000F];
        break;
      case 'o':
        opidx = 0;
        while (instr->name[opidx])
        {
          result[idx++] = instr->name[opidx++];
        }
        result[idx++] = '\t';
        break;
      case '@':
        
        if (instr->size > 1 && instr->hasImmediate && (instr->immType == 'B' || instr->immType == 'W' ))
        {
          if (instr->hasParam) /* immediate on c or (c and d) */
          {
            if (instr->immType = 'B') /* immediate on  c */
            {
              dest = PC + instr->size + (SBYTE)c;
            }
            else /* immediate on dc  (d high byte , c low byte) */
            {
              dest = PC + instr->size + (SWORD)((WORD)c | (((WORD)d )<<8));
            }
          }
          else /* immediate on b or (b and c) */
          {
            if (instr->immType = 'B') /* immediate on b */
            {
              dest = PC + instr->size + (SBYTE)b;
            }
            else /* immediate on cb (c high byte, b low byte) */
            {
              dest = PC + instr->size + (SWORD)((WORD)b | (((WORD)c )<<8));
            }
          }
          result[idx++] = hexa_digit[(dest & 0xF000) >> 12];
          result[idx++] = hexa_digit[(dest & 0x0F00) >> 8];
          result[idx++] = hexa_digit[(dest & 0x00F0) >> 4];
          result[idx++] = hexa_digit[dest & 0x000F];
        }
        else /* Something wrong in the desfinition */
        {
          result[idx++] = '?';
          result[idx++] = '?';
          result[idx++] = '?';
          result[idx++] = '?';
        }
        break;
      default:
        result[idx++] = instr->operandFormat[opfmtidx];
    } /* end of switch case the operand format character interpretation */
    opfmtidx++;
  } /* end for each character in operandFormat */

  result[idx] = 0; /* ASCIIZ null terminator. */
  
  return PC + instr->size; 
}

/* Signal handling (end process, timer alarm) */
static sig_atomic_t end = 0;
static sig_atomic_t timer_alarm = 0;

static void sighandler(int signo)
{
  if (signo == SIGVTALRM) {
    timer_alarm = 1;
  }
  else 
  {
    end = 1;
  }
}

void bs3_hyper_timerset(DWORD microseconds) 
{
  struct timeval interval;
  struct itimerval period;

  interval.tv_sec=(time_t)microseconds / 1000000;
  interval.tv_usec=(long int)(microseconds % 1000000);
  
  
  period.it_interval=interval;
  period.it_value=interval;
  setitimer(ITIMER_VIRTUAL,&period,NULL);
}

void bs3_hyper_timerstop()
{
  bs3_hyper_timerset(0);
}

/* initialize CPU (memory not initialized) */
void bs3_cpu_init(struct bs3_cpu_data * pbs3)
{
  /* PC = [0000] */
  pbs3->r.PC =pbs3->vector[0];
  /* CZVN and I = 0 (disallow interruption by default) */
  pbs3->r.FL = 0;
  /* SP = 0x0400 */
  pbs3->r.SP = 0x0400;
  /* Wx = 0 -> Bx = 0 */
  pbs3->r.W[0] = 0;
  pbs3->r.W[1] = 0;
  pbs3->r.W[2] = 0;
  pbs3->r.W[3] = 0;
  /* reset status and counter/timer */
  pbs3->status = BS3_STATUS_DEFAULT;
  pbs3->counter = pbs3->timer; /* cpu current countdown (initialiazed with timer) */
  pbs3->pending_interrupt = BS3_INT_NOPENDING; /* no pending interrupt */
  pbs3->input_ready = 0x01; /* by default nothing available for input */
  pbs3->output_ready = 0x00; /* by default output is ready to receveive data */
  pbs3->output2_ready = 0x00; /* by default auxiliary output is ready to receveive data. */
  pbs3->msortick = 0; /* by default millisecond timer ( if =1, then cpu clock is used) */
  bs3_hyper_timerstop();
}



/* for interrupt from external (not by program INT instruction) */
void bs3_cpu_interrupt(struct bs3_cpu_data * pbs3, int intnum)
{
  if (pbs3->status == BS3_STATUS_HALT) return;
  if (intnum == BS3_INT_BADINSTR || pbs3->r.I == 1)
  {
    /* PUSH PC, */
     pbs3->r.SP-=2;
     *(WORD *)&(pbs3->m[ pbs3->r.SP ]) = pbs3->r.PC;
    /* PUSH FL */
     pbs3->r.SP-=2;
     *(WORD *)&(pbs3->m[ pbs3->r.SP ]) = pbs3->r.FL;
    /* PC=m[intnum << 1] */
    pbs3->r.PC = pbs3->vector[intnum]; 
    /* I=0 , by default in interrupt handler , disallowed interrupt */
    pbs3->r.I = 0;
    pbs3->pending_interrupt = BS3_INT_NOPENDING;
  } else {
    pbs3->pending_interrupt = intnum;
  }
  if (pbs3->status == BS3_STATUS_WAIT) pbs3->status = BS3_STATUS_DEFAULT;
}


void bs3_cpu_write_byte(struct bs3_cpu_data * pbs3, WORD address, BYTE data) 
{
  if ((address & 0xFF00) == 0x0100) /* System Write I/O */
  {
    switch (address)
    {
      case 0x0100: /* write on input data is ignored */
      case 0x0101: /* write on input status is ignored */
        break;
      case 0x0102:
        if (pbs3->m[0x0103] == 0x00) /* if ok to write on output */ 
        {
          pbs3->m[0x0103] == 0x01; /* output is waiting to be consummed */
          pbs3->m[address] = data; /* output data available */
        }
        break;
      case 0x0103: /* write on output status is ignored */
        break;
      case 0x0104:
        if (pbs3->m[0x0105] == 0x00) /* if ok to write on auxiliary output */
        {
          pbs3->m[0x0106] == 0x01; /* output is waiting to be consummed */
          pbs3->m[address] = data; /* output data available */
        }
        break;
      case 0x0105: /* write on auxiliary output status is ignored */ 
        break;
      case 0x0106: /* low byte of the low 16 bits of the 32 bits timer */
      case 0x0107: /* high byte of the low 16 bits of the 32 bits timer */
      case 0x0108: /* low byte of the high 16 bits of the 32 bits timer */
      case 0x010A: /* ms(0) or tick timer(1) */
        pbs3->m[address] = data;
        break;
      case 0x0109: /* high byte of the high 16 bits of the 3é bits timer : at this write, timer is restarted */
        pbs3->m[address] = data;
        
        if (pbs3->msortick == 0) /* microseconds */ 
        {
          bs3_hyper_timerset(pbs3->timer);
        } 
        else /* timer tick */
        {
          pbs3->counter = pbs3->timer;
          bs3_hyper_timerstop();
        }
        break;
      default:
        /* TODO : manage Write I/O */
        /*        do nothing for now */
        break;
    }
  }
  else
  {
    pbs3->m[address] = data;
  }
}



BYTE bs3_cpu_read_byte(struct bs3_cpu_data * pbs3, WORD address) {
  if ((address & 0xFF00) == 0x0100) /* System I/O */
  {
    switch (address)
    {
      case 0x0100:
        if (pbs3->m[0x101] == 0x00)
        {
          pbs3->m[0x0101] = 0x01; /* core input consummed */
        }
        return pbs3->m[address]; 
        break;
      case 0x0101: /* core input status */
      case 0x0102: /* core output */
      case 0x0103: /* core output status */
      case 0x0104: /* core auxiliary output */
      case 0x0105: /* core auxliiary output status */
      case 0x0106: /* timer 32 low, 16 bits low */
      case 0x0107: /* timer 32 low, 1§ bit high */
      case 0x0108: /* timer 32 high, 16 bits low */
      case 0x0109: /* timer 32 high, 16 bits high */
      case 0x010A: /* msortick */
        return pbs3->m[address];
        break;
      default:
      /* TODO : manage other system  I/O read, return NULL byte for now. */
        return 0;
    }
  }
  else
  {
    return pbs3->m[address];
  }
  return 0;
}


void bs3_cpu_write_word(struct bs3_cpu_data * pbs3, WORD address, WORD data) 
{
  bs3_cpu_write_byte(pbs3, address, (BYTE)(data & 0x00FF));
  bs3_cpu_write_byte(pbs3, address+1, (BYTE)((data>>8) & 0x00FF));
}

WORD bs3_cpu_read_word(struct bs3_cpu_data * pbs3, WORD address) {
  return bs3_cpu_read_byte(pbs3, address) |  (((WORD)bs3_cpu_read_byte(pbs3, address + 1)) << 8);
}


void bs3_cpu_exec(struct bs3_cpu_data * pbs3)
{
  BYTE ope;
  BYTE immB;
  WORD immW;
  union opParam p; /* operator parameter (secondary byte used to parameterise the operation) */
  BYTE b;
  SBYTE sb;
  WORD w;
  SWORD sw;
  DWORD dw;
  SDWORD sdw;
  WORD s1;
  WORD s2;
  WORD s3;
  WORD s4;
  if (pbs3->status == BS3_STATUS_HALT) return; 
  if (pbs3->status == BS3_STATUS_RESET) 
  {
    bs3_cpu_init(pbs3);
  }
  /* is there pending interrupt, with interrupt enabled?*/
  if (pbs3->r.I == 1 && pbs3->pending_interrupt != BS3_INT_NOPENDING) {
    bs3_cpu_interrupt(pbs3, pbs3->pending_interrupt);
  }
  else /* timer interrupt ?*/
  {
    if (pbs3->msortick == 1)
    {
      pbs3->counter--;
      if (pbs3->counter == 0)  
      {
        bs3_cpu_interrupt(pbs3, BS3_INT_TIMER);
        pbs3->counter == pbs3->timer;
      }
    } else {
      if (timer_alarm == 1) {
        bs3_cpu_interrupt(pbs3, BS3_INT_TIMER);
        timer_alarm = 0;
      }
    }
  }
  if (pbs3->status == BS3_STATUS_WAIT) return;
  ope = pbs3->m[pbs3->r.PC]; /* operator at PC */
  switch ( ope ) 
  {
        case BS3_INSTR_NOP:
            pbs3->r.PC++;
            break;
        case BS3_INSTR_INB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0101); // read input status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            pbs3->r.B[p.x3] =  pbs3->r.Z ? pbs3->r.B[p.x3] : bs3_cpu_read_byte(pbs3, 0x0100); // read input if there is something to get
            break;
        case BS3_INSTR_OUTB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0103); // read output status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            bs3_cpu_write_byte(pbs3, 0x0102, pbs3->r.B[p.x3]); // write occurs if output is ready to receive data
            break;
        case BS3_INSTR_OUTB2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0105); // read output status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            bs3_cpu_write_byte(pbs3, 0x0104, pbs3->r.B[p.x3]); // write occurs if output is ready to receive data
        case BS3_INSTR_OUTBI:
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0103); // read output status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            bs3_cpu_write_byte(pbs3, 0x0102, immB); // write occurs if output is ready to receive data
            break;
        case BS3_INSTR_OUTB2I:
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0105); // read output status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            bs3_cpu_write_byte(pbs3, 0x0104, immB); // write occurs if output is ready to receive data
            break;
        case BS3_INSTR_LEAW0:
        case BS3_INSTR_LEAW1:
        case BS3_INSTR_LEAW2:
        case BS3_INSTR_LEAW3:
            pbs3->r.PC++;
            pbs3->r.W[ope & 0x03] = pbs3->r.PC + ((SWORD)bs3_cpu_read_word(pbs3,  pbs3->r.PC)) + 2;
            pbs3->r.PC += 2; 
            break;
        case BS3_INSTR_LEAB0:
        case BS3_INSTR_LEAB1:
        case BS3_INSTR_LEAB2:
        case BS3_INSTR_LEAB3:
            pbs3->r.PC++;
            pbs3->r.W[ope & 0x03] = pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3,  pbs3->r.PC)) + 1;
            pbs3->r.PC ++; 
            break;
        case BS3_INSTR_CLC:
            pbs3->r.PC++;
            pbs3->r.C = 0;
            break;
        case BS3_INSTR_CLZ:
            pbs3->r.PC++;
            pbs3->r.Z = 0;
            break;
        case BS3_INSTR_CLV:
            pbs3->r.PC++;
            pbs3->r.V = 0;
            break;
        case BS3_INSTR_CLN:
            pbs3->r.PC++;
            pbs3->r.N = 0;
            break;
        case BS3_INSTR_STC:
            pbs3->r.PC++;
            pbs3->r.C = 1;
            break;
        case BS3_INSTR_STZ:
            pbs3->r.PC++;
            pbs3->r.Z = 1;
            break;
        case BS3_INSTR_STV:
            pbs3->r.PC++;
            pbs3->r.V = 1;
            break;
        case BS3_INSTR_STN:
            pbs3->r.PC++;
            pbs3->r.N = 1;
            break;
        case BS3_INSTR_INT1:
            pbs3->r.PC++;
            pbs3->status = BS3_STATUS_ESCHYP; /* Hypervisor high level function requested */
            break;
        case BS3_INSTR_INT0:
        case BS3_INSTR_INT2:
        case BS3_INSTR_INT3:
        case BS3_INSTR_INT4:
        case BS3_INSTR_INT5:
        case BS3_INSTR_INT6:
        case BS3_INSTR_INT7:
        case BS3_INSTR_INT8:
        case BS3_INSTR_INT9:
        case BS3_INSTR_INT10:
        case BS3_INSTR_INT11:
        case BS3_INSTR_INT12:
        case BS3_INSTR_INT13:
        case BS3_INSTR_INT14:
        case BS3_INSTR_INT15:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC);
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.FL);
            pbs3->r.PC = pbs3->vector[ope & 0x0F];
            break;
        case BS3_INSTR_J_W0:
        case BS3_INSTR_J_W1:
        case BS3_INSTR_J_W2:
        case BS3_INSTR_J_W3:
            pbs3->r.PC = pbs3->r.W[ope & 0x03];
            break;
        case BS3_INSTR_C_W0:
        case BS3_INSTR_C_W1:
        case BS3_INSTR_C_W2:
        case BS3_INSTR_C_W3:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC); 
            pbs3->r.PC = pbs3->r.W[ope & 0x03];           
            break;
        case BS3_INSTR_RET:
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case BS3_INSTR_IRET:
            pbs3->r.FL = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case BS3_INSTR_J_A:
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.PC + 1);
            break;
        case BS3_INSTR_C_A:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC + 2);
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.PC); 
            break;
        case BS3_INSTR_J_R:
            pbs3->r.PC++;
            pbs3->r.PC = 1 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            break;
        case BS3_INSTR_C_R:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC + 1);
            pbs3->r.PC = 1 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            break;
        case BS3_INSTR_JZ:
            if (pbs3->r.Z == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JNZ:
            if (pbs3->r.Z == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JC:
            if (pbs3->r.C == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JNC:
            if (pbs3->r.C == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JN:
            if (pbs3->r.N == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JNN:
            if (pbs3->r.N == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JV:
            if (pbs3->r.V == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JNV:
            if (pbs3->r.V == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JA:
            if (pbs3->r.C == 1 && pbs3->r.Z == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JBE:
            if (pbs3->r.C == 0 && pbs3->r.Z == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JGE:
            if (pbs3->r.N == pbs3->r.V) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JL:
            if (pbs3->r.N != pbs3->r.V) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JG:
            if (pbs3->r.N == pbs3->r.V && pbs3->r.Z == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_JLE:
            if (pbs3->r.N != pbs3->r.V || pbs3->r.Z == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case BS3_INSTR_LDBm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC));
            pbs3->r.PC += 2; 
            break;
        case BS3_INSTR_LDBm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2]);
            break;
        case BS3_INSTR_LDBm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC ++; 
            break;
        case BS3_INSTR_LDBm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2]);
            break;
        case BS3_INSTR_LDBm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_LDBm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2]);
            break;
        case BS3_INSTR_LDBm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_LDBm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_LDWm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC));
            pbs3->r.PC += 2; 
            break;
        case BS3_INSTR_LDWm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2]);
            break;
        case BS3_INSTR_LDWm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_LDWm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2]);
            break;
        case BS3_INSTR_LDWm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_LDWm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2]);
            break;
        case BS3_INSTR_LDWm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_LDWm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_SRBm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC += 2; 
            break;
        case BS3_INSTR_SRBm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2],  pbs3->r.B[p.x3]);
            break;
        case BS3_INSTR_SRBm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC ++; 
            break;
        case BS3_INSTR_SRBm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2], pbs3->r.B[p.x3]);
            break;
        case BS3_INSTR_SRBm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_SRBm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2], pbs3->r.B[p.x3]);
            break;
        case BS3_INSTR_SRBm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_SRBm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_SRWm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC += 2; 
            break;
        case BS3_INSTR_SRWm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2], pbs3->r.W[p.x2]);
            break;
        case BS3_INSTR_SRWm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_SRWm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2], pbs3->r.W[p.x2]);
            break;
        case BS3_INSTR_SRWm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_SRWm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2], pbs3->r.W[p.x2]);
            break;
        case BS3_INSTR_SRWm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_SRWm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case BS3_INSTR_POPB0:
        case BS3_INSTR_POPB1:
        case BS3_INSTR_POPB2:
        case BS3_INSTR_POPB3:
        case BS3_INSTR_POPB4:
        case BS3_INSTR_POPB5:
        case BS3_INSTR_POPB6:
        case BS3_INSTR_POPB7:
            pbs3->r.PC ++;
            pbs3->r.B[ope & 0x07] = (BYTE)(bs3_cpu_read_word(pbs3, pbs3->r.SP) & 0x00FF);
            pbs3->r.SP += 2; 
            break;
        case BS3_INSTR_POPW0:
        case BS3_INSTR_POPW1:
        case BS3_INSTR_POPW2:
        case BS3_INSTR_POPW3:
            pbs3->r.PC ++;
            pbs3->r.W[ope & 0x03] = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2; 
            break;
        case BS3_INSTR_POPA:
            pbs3->r.PC ++;
            pbs3->r.W[3] = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            pbs3->r.W[2] = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            pbs3->r.W[1] = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            pbs3->r.W[0] = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break; 
        case BS3_INSTR_POPF:
            pbs3->r.PC ++;
            pbs3->r.FL = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case BS3_INSTR_POPPC:
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case BS3_INSTR_DROP:
            pbs3->r.PC ++;
            pbs3->r.SP += 2;
            break;
        case BS3_INSTR_PUSHB0:
        case BS3_INSTR_PUSHB1:
        case BS3_INSTR_PUSHB2:
        case BS3_INSTR_PUSHB3:
        case BS3_INSTR_PUSHB4:
        case BS3_INSTR_PUSHB5:
        case BS3_INSTR_PUSHB6:
        case BS3_INSTR_PUSHB7:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, (WORD)pbs3->r.B[ope & 0x07]);
            break;
        case BS3_INSTR_PUSHW0:
        case BS3_INSTR_PUSHW1:
        case BS3_INSTR_PUSHW2:
        case BS3_INSTR_PUSHW3:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.W[ope & 0x03]);
            break;
        case BS3_INSTR_PUSHA:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.W[0]);
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.W[1]);
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.W[2]);
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.W[3]);
            break;
        case BS3_INSTR_PUSHF:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.FL);
            break;
        case BS3_INSTR_PUSHPC:
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC);
            pbs3->r.PC ++;
            break;
        case BS3_INSTR_DUP:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, bs3_cpu_read_word(pbs3, pbs3->r.SP + 2 ));
            break;
        case BS3_INSTR_ANDB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= pbs3->r.B[p.y3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_ORB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] |= pbs3->r.B[p.y3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_EORB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] ^= pbs3->r.B[p.y3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_BICB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= ~(pbs3->r.B[p.y3]);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_TSTB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = pbs3->r.B[p.x3] & pbs3->r.B[p.y3];
            pbs3->r.N = (b & 0x80) != 0;
            pbs3->r.Z = (b == 0);
            break;
        case BS3_INSTR_SHLB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((WORD)(pbs3->r.B[p.x3])) << pbs3->r.B[p.y3];
            pbs3->r.B[p.x3] =(BYTE)(w & 0x00FF);
            pbs3->r.N = (w & 0x0080) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0100) !=0;
            break;
        case BS3_INSTR_SHRB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  (((WORD)(pbs3->r.B[p.x3])) << 8) >> pbs3->r.B[p.y3];
            pbs3->r.B[p.x3] = (BYTE)((w & 0xFF00)>>8);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0080) !=0;
            break;
        case BS3_INSTR_SARB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((SWORD)(((WORD)(pbs3->r.B[p.x3])) << 8)) >> pbs3->r.B[p.y3];
            pbs3->r.B[p.x3] = (BYTE)((w & 0xFF00)>>8);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0080) !=0;
            break;
        case BS3_INSTR_RORB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = pbs3->r.B[p.x3] & 0x01;
            pbs3->r.B[p.x3] >>= 1;
            pbs3->r.B[p.x3] |= (pbs3->r.C << 7);
            pbs3->r.N = pbs3->r.C;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_ROLB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.B[p.x3] <<= 1;
            pbs3->r.B[p.x3] |= pbs3->r.C;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_NOTB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = ~pbs3->r.B[p.x3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_ADDB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */ 
            s2 = (pbs3->r.B[p.y3] & 0x80) != 0; /* keep sign of second */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)(pbs3->r.B[p.y3]));
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_ADCB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (pbs3->r.B[p.y3] & 0x80) != 0; /* keep sign of second */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)(pbs3->r.B[p.y3])) + pbs3->r.C;
            s3 = (w & 0x0080) != 0; /* keep sign of first+second+carry */
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && ( s3 != s1); 
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_SUBB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b =  (~pbs3->r.B[p.y3]) + 1;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (b & 0x80) == 0; /* keep sign of neg(second) */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b); /* first + neg(second) */
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_SBBB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b =  ~pbs3->r.B[p.y3]; /* not(second) */
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (b & 0x80) == 0; /* keep sign of not(second) */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b) + pbs3->r.C; /* first + not(second) + carry */
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_CMPB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b =  (~pbs3->r.B[p.y3]) + 1;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (b & 0x80) == 0; /* keep sign of neg(second) */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b); /* first + neg(second) */
            b = (BYTE)(w & 0x00FF);
            s3 = (b & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (b & 0x80) != 0;
            pbs3->r.Z = (b == 0);
            break;
        case BS3_INSTR_ANDW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] &= pbs3->r.W[p.y2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_ORW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] |= pbs3->r.W[p.y2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_EORW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] ^= pbs3->r.W[p.y2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_BICW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] &= ~(pbs3->r.W[p.y2]);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_TSTW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w = pbs3->r.W[p.x2] & pbs3->r.W[p.y2];
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case BS3_INSTR_SHLW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((DWORD)(pbs3->r.W[p.x2])) << pbs3->r.B[p.y3];
            pbs3->r.W[p.x2] = dw & 0x0000FFFF;
            pbs3->r.N = (dw & 0x00008000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            pbs3->r.C = (dw & 0x00010000) !=0;
            break;
        case BS3_INSTR_SHRW: // see in doc also as SHRWB
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  (((DWORD)(pbs3->r.W[p.x2])) << 16) >> pbs3->r.B[p.y3];
            pbs3->r.W[p.x2] = (WORD)((dw & 0xFFFF0000)>>16);
            pbs3->r.N = (dw & 0x80000000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            pbs3->r.C = (w & 0x00008000) !=0;
            break;
        case BS3_INSTR_SARW: // see in doc also as SARWB
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((SDWORD)(((DWORD)(pbs3->r.W[p.x2])) << 16)) >> pbs3->r.B[p.y3];
            pbs3->r.W[p.x2] = (WORD)((dw & 0xFFFF0000)>>16);
            pbs3->r.N = (dw & 0x80000000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            pbs3->r.C = (w & 0x00008000) !=0;
            break;
        case BS3_INSTR_RORW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = pbs3->r.W[p.x2] & 0x0001;
            pbs3->r.W[p.x2] >>= 1;
            pbs3->r.W[p.x3] |= (pbs3->r.C << 15);
            pbs3->r.N = pbs3->r.C;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_ROLW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.W[p.x2] <<= 1;
            pbs3->r.W[p.x2] |= pbs3->r.C;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_NOTW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = ~pbs3->r.W[p.x2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_ADDW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            s2 = (pbs3->r.W[p.y2] & 0x8000) != 0; /* keep sign of second */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)(pbs3->r.B[p.y2]));
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first+second */
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_ADCW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            s2 = (pbs3->r.W[p.y2] & 0x8000) != 0; /* keep sign of second */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)(pbs3->r.B[p.y2])) + pbs3->r.C;
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first+second */
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_SUBW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w = ~pbs3->r.W[p.y2] + 1; /* neg(second) */
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            s2 = (w & 0x8000) != 0; /* keep sign of neg(second) */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)w);
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first + neg(second) */
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_SBBW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w = ~pbs3->r.W[p.y2]; /* not(second) */
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            s2 = (w & 0x8000) != 0; /* keep sign of neg(second) */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)w) + pbs3->r.C;
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first + neg(second) */
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_CMPW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w = ~pbs3->r.W[p.y2] + 1; /* neg(second) */
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            s2 = (w & 0x8000) != 0; /* keep sign of neg(second) */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)w);
            w = (WORD)(dw & 0x0000FFFF);
            s3 = (w & 0x8000) != 0; /* keep sign of first + neg(second) */
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case BS3_INSTR_MULB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((WORD)pbs3->r.B[p.x3]) * ((WORD)pbs3->r.B[p.y3]);
            pbs3->r.B[p.y3] = (BYTE)((w & 0xFF00) >> 8); 
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF); 
            pbs3->r.N = (pbs3->r.B[p.y3] & 0x80) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case BS3_INSTR_IMULB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            sw =  ((SWORD)(SBYTE)(pbs3->r.B[p.x3])) * ((SWORD)(SBYTE)(pbs3->r.B[p.y3]));
            pbs3->r.B[p.y3] = (BYTE)((sw & 0xFF00) >> 8); 
            pbs3->r.B[p.x3] = (BYTE)(sw & 0x00FF); 
            pbs3->r.N = (pbs3->r.B[p.y3] & 0x80) != 0;
            pbs3->r.Z = (sw == 0);
            break;
        case BS3_INSTR_DIVB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            if (pbs3->r.B[p.y3] != 0) 
            {
              b = pbs3->r.B[p.x3] / pbs3->r.B[p.y3];
              pbs3->r.B[p.y3] =  pbs3->r.B[p.x3] % pbs3->r.B[p.y3];
              pbs3->r.B[p.x3] = b;
              pbs3->r.N = (b & 0x80) != 0;
              pbs3->r.Z = (b == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_IDIVB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            if (pbs3->r.B[p.y3] != 0) 
            {
              sb = ((SBYTE)(pbs3->r.B[p.x3])) / ((SBYTE)(pbs3->r.B[p.y3]));
              pbs3->r.B[p.y3] =  (BYTE)(((SBYTE)(pbs3->r.B[p.x3])) % ((SBYTE)pbs3->r.B[p.y3]));
              pbs3->r.B[p.x3] = (BYTE)sb;
              pbs3->r.N = (sb & 0x80) != 0;
              pbs3->r.Z = (sb == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_MOVB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = pbs3->r.B[p.y3];
            break;
        case BS3_INSTR_SWPB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = pbs3->r.B[p.x3];
            pbs3->r.B[p.x3] = pbs3->r.B[p.y3];
            pbs3->r.B[p.y3] = b;
            break;
        case BS3_INSTR_MULW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((DWORD)pbs3->r.W[p.x2]) * ((DWORD)pbs3->r.W[p.y2]);
            pbs3->r.W[p.z2] = (WORD)((dw & 0xFFFF0000) >> 16); 
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF); 
            pbs3->r.N = (pbs3->r.W[p.z2] & 0x8000) != 0;
            pbs3->r.Z = (dw == 0);
            break;
        case BS3_INSTR_IMULW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            sdw =  ((SDWORD)(SWORD)(pbs3->r.W[p.x2])) * ((SDWORD)(SWORD)(pbs3->r.W[p.y2]));
            pbs3->r.W[p.z2] = (WORD)((dw & 0xFFFF0000) >> 16); 
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF); 
            pbs3->r.N = (pbs3->r.W[p.z2] & 0x8000) != 0;
            pbs3->r.Z = (dw == 0);
            break;
        case BS3_INSTR_DIVW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            if (pbs3->r.W[p.y2] != 0) 
            {
              w = pbs3->r.W[p.x2] / pbs3->r.W[p.y2];
              pbs3->r.W[p.z2] =  pbs3->r.W[p.x2] % pbs3->r.W[p.y2];
              pbs3->r.W[p.x2] = w;
              pbs3->r.N = (w & 0x8000) != 0;
              pbs3->r.Z = (w == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_IDIVW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            if (pbs3->r.W[p.y2] != 0) 
            {
              sw = ((SWORD)pbs3->r.W[p.x2]) / ((SWORD)pbs3->r.W[p.y2]);
              pbs3->r.W[p.z2] =  ((SWORD)pbs3->r.W[p.x2]) % ((SWORD)pbs3->r.W[p.y2]);
              pbs3->r.W[p.x2] = sw;
              pbs3->r.N = (sw & 0x8000) != 0;
              pbs3->r.Z = (sw == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_MOVW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = pbs3->r.W[p.y2];
        case BS3_INSTR_MOVSW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = pbs3->r.SP;
        case BS3_INSTR_MOVWS:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.SP = pbs3->r.W[p.x2];
        case BS3_INSTR_SWPW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w = pbs3->r.W[p.x2];
            pbs3->r.W[p.x2] = pbs3->r.W[p.y2];
            pbs3->r.W[p.y2] = w;
            break;
        case BS3_INSTR_ANDBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_ORBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] |= b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_EORBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] ^= b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_BICBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= ~b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_TSTBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b &= pbs3->r.B[p.x3];
            pbs3->r.N = (b & 0x80) != 0;
            pbs3->r.Z = (b == 0);
            break;
        case BS3_INSTR_SHLB1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.B[p.x3] & 0x80) !=0;
            pbs3->r.B[p.x3] <<= 1;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_SHRB1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.B[p.x3] & 0x01) !=0;
            pbs3->r.B[p.x3] >>= 1;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_SARB1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((SWORD)(((WORD)(pbs3->r.B[p.x3])) << 8)) >> 1;
            pbs3->r.B[p.x3] = (BYTE)((w & 0xFF00)>>8);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0080) !=0;
            break;
        case BS3_INSTR_INCB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */ 
            pbs3->r.C = pbs3->r.B[p.x3] == 0xFF;
            pbs3->r.B[p.x3]++;
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.V = (s3 == 1 && s1 == 0);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_DECB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */ 
            pbs3->r.C = pbs3->r.B[p.x3] == 0x00;
            pbs3->r.B[p.x3]--;
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.V = (s3 == 0 && s1 == 0);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_NEGB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = (BYTE)(-((SBYTE)(pbs3->r.B[p.x3])));
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_ADDBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */ 
            s2 = (b & 0x80) != 0; /* keep sign of second */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b);
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_ADCBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (b & 0x80) != 0; /* keep sign of second */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b) + pbs3->r.C;
            s3 = (w & 0x0080) != 0; /* keep sign of first+second+carry */
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && ( s3 != s1); 
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_SUBBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = (BYTE)(-(SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (b & 0x80) == 0; /* keep sign of neg(second) */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b); /* first + neg(second) */
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_SBBBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = ~bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (b & 0x80) == 0; /* keep sign of not(second) */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b) + pbs3->r.C; /* first + not(second) + carry */
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF);
            s3 = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BS3_INSTR_CMPBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = (BYTE)(-((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC)));
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */
            s2 = (b & 0x80) == 0; /* keep sign of neg(second) */
            w = ((WORD)(pbs3->r.B[p.x3])) + ((WORD)b); /* first + neg(second) */
            b = (BYTE)(w & 0x00FF);
            s3 = (b & 0x80) != 0;
            pbs3->r.C = (w & 0x0100) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (b & 0x80) != 0;
            pbs3->r.Z = (b == 0);
            break;
        case BS3_INSTR_ANDWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] &= immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_ORWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] |= immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_EORWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] ^= immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_BICWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] &= ~immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_TSTWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            w = pbs3->r.W[p.x2] & immW;
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case BS3_INSTR_SHLW1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.W[p.x2] & 0x8000) !=0;
            pbs3->r.W[p.x2] <<= 1;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_SHRW1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.W[p.x2] & 0x01) !=0;
            pbs3->r.W[p.x2] >>= 1;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_SARW1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((SDWORD)(((DWORD)(pbs3->r.W[p.x2])) << 16)) >> 1;
            pbs3->r.W[p.x2] = (WORD)((dw & 0xFFF0000)>>16);
            pbs3->r.N = (dw & 0x80000000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x00008000) !=0;
            break;
        case BS3_INSTR_INCW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            pbs3->r.C = pbs3->r.W[p.x2] == 0xFFFF;
            pbs3->r.W[p.x2]++;
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.V = (s3 == 1 && s1 == 0);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_DECW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            s1 = (pbs3->r.B[p.x3] & 0x80) != 0; /* keep sign of first */ 
            pbs3->r.C = pbs3->r.W[p.x2] == 0x0000;
            pbs3->r.W[p.x2]--;
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.V = (s3 == 0 && s1 == 0);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_NEGW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = (WORD)(-((SWORD)(pbs3->r.W[p.x2])));
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_ADDWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC +=2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            s2 = (immW & 0x8000) != 0; /* keep sign of second */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)immW);
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_ADCWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */
            s2 = (immW & 0x8000) != 0; /* keep sign of second */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)immW) + pbs3->r.C;
            s3 = (dw & 0x00008000) != 0; /* keep sign of first+second+carry */
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && ( s3 != s1); 
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_SUBWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = (WORD)(-(SWORD)bs3_cpu_read_word(pbs3, pbs3->r.PC));
            pbs3->r.PC += 2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */
            s2 = (immW & 0x8000) == 0; /* keep sign of neg(second) */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)immW); /* first + neg(second) */
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_SBBWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = ~bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */
            s2 = (immW & 0x8000) == 0; /* keep sign of not(second) */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)immW) + pbs3->r.C; /* first + not(second) + carry */
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BS3_INSTR_CMPWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = (WORD)(-((SWORD)bs3_cpu_read_word(pbs3, pbs3->r.PC)));
            pbs3->r.PC += 2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */
            s2 = (immW & 0x8000) == 0; /* keep sign of neg(second) */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)immW); /* first + neg(second) */
            w = (WORD)(dw & 0x0000FFFF);
            s3 = (w & 0x8000) != 0;
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case BS3_INSTR_MULBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((WORD)pbs3->r.B[p.x3]) * ((WORD)immB);
            pbs3->r.B[p.y3] = (BYTE)((w & 0xFF00) >> 8); 
            pbs3->r.B[p.x3] = (BYTE)(w & 0x00FF); 
            pbs3->r.N = (pbs3->r.B[p.y3] & 0x80) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case BS3_INSTR_IMULBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            sw =  ((SWORD)(SBYTE)(pbs3->r.B[p.x3])) * ((SWORD)(SBYTE)(immB));
            pbs3->r.B[p.y3] = (BYTE)((sw & 0xFF00) >> 8); 
            pbs3->r.B[p.x3] = (BYTE)(sw & 0x00FF); 
            pbs3->r.N = (pbs3->r.B[p.y3] & 0x80) != 0;
            pbs3->r.Z = (sw == 0);
            break;
        case BS3_INSTR_DIVBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            if (immB != 0) 
            {
              b = pbs3->r.B[p.x3] / immB;
              pbs3->r.B[p.y3] =  pbs3->r.B[p.x3] % immB;
              pbs3->r.B[p.x3] = b;
              pbs3->r.N = (b & 0x80) != 0;
              pbs3->r.Z = (b == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_IDIVBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            if (immB != 0) 
            {
              sb = ((SBYTE)(pbs3->r.B[p.x3])) / ((SBYTE)(immB));
              pbs3->r.B[p.y3] =  (BYTE)(((SBYTE)(pbs3->r.B[p.x3])) % ((SBYTE)immB));
              pbs3->r.B[p.x3] = (BYTE)sb;
              pbs3->r.N = (sb & 0x80) != 0;
              pbs3->r.Z = (sb == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_MOVBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = immB;
            break;
        case BS3_INSTR_MULWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            dw =  ((DWORD)pbs3->r.W[p.x2]) * ((DWORD)immW);
            pbs3->r.W[p.y2] = (WORD)((dw & 0xFFFF0000) >> 16); 
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF); 
            pbs3->r.N = (pbs3->r.W[p.y2] & 0x8000) != 0;
            pbs3->r.Z = (dw == 0);
            break;
        case BS3_INSTR_IMULWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            sdw =  ((SDWORD)(SWORD)(pbs3->r.W[p.x2])) * ((SDWORD)(SWORD)(immW));
            pbs3->r.W[p.y2] = (WORD)((dw & 0xFFFF0000) >> 16); 
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF); 
            pbs3->r.N = (pbs3->r.W[p.y2] & 0x8000) != 0;
            pbs3->r.Z = (dw == 0);
            break;
        case BS3_INSTR_DIVWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            if (immW != 0) 
            {
              w = pbs3->r.W[p.x2] / immW;
              pbs3->r.W[p.y2] =  pbs3->r.W[p.x2] % immW;
              pbs3->r.W[p.x2] = w;
              pbs3->r.N = (w & 0x8000) != 0;
              pbs3->r.Z = (w == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_IDIVWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            if (immW != 0) 
            {
              sw = ((SWORD)pbs3->r.W[p.x2]) / ((SWORD)immW);
              pbs3->r.W[p.y2] =  ((SWORD)pbs3->r.W[p.x2]) % ((SWORD)immW);
              pbs3->r.W[p.x2] = sw;
              pbs3->r.N = (sw & 0x8000) != 0;
              pbs3->r.Z = (sw == 0);
            }
            else 
            {
              pbs3->status = BS3_STATUS_BADINSTR;
              bs3_cpu_interrupt(pbs3, BS3_INT_DIVZERO);
            }
            break;
        case BS3_INSTR_MOVWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] = immW;
            break;
        case BS3_INSTR_MOVSI:
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.SP = immW;
            break;
        case BS3_INSTR_STI:
            pbs3->r.PC++;
            pbs3->r.I = 1;
            break;
        case BS3_INSTR_CLI:
            pbs3->r.PC++;
            pbs3->r.I = 0;
            break;
        case BS3_INSTR_HEVT:
            pbs3->r.PC++;
            pbs3->status = BS3_STATUS_HEVT;
            break;
        case BS3_INSTR_WAIT:
            pbs3->r.PC++;
            pbs3->status = BS3_STATUS_WAIT;
            break;
        case BS3_INSTR_RESET:
            pbs3->status = BS3_STATUS_RESET;
            break;
        case BS3_INSTR_HLT:
            pbs3->status = BS3_STATUS_HALT;
            break;
        default:
            pbs3->status = BS3_STATUS_BADINSTR;
            bs3_cpu_interrupt(pbs3, BS3_INT_BADINSTR);

  }
}

/* Hypervisor */
void bs3_hyper_reset_memory(struct bs3_cpu_data * pbs3)
{
  WORD i;
  for (i=0; i <= 0xFFFF; i++) pbs3->m[i] = 0;
}

void bs3_hyper_load_memory(struct bs3_cpu_data * pbs3, BYTE * data, long length, WORD address)
{
  long i;
  for (i=0; i < length; i++) pbs3->m[(address + i) & 0xFFFF] = data[i];
}

/* process core I/O between Hypervisor and bs3 CPU */
void bs3_hyper_coreIO(struct bs3_cpu_data * pbs3)
{
  int ret;
  int data;
  /* process output*/ 
  if (pbs3->output_ready == 0x01) { /* something pending for output*/
    data = pbs3->output_data;
    ret = fputc(data, stdout);
    if (ret == EOF) 
    {
      switch (errno)
      {
        case EAGAIN : /* output is not ready, another try is necessary */
          break;
        case EBADF :
        case EIO :
          pbs3->output_ready = 0xFF; /* output is not usable*/
      }
    } 
    else
    {
      pbs3->output_ready == 0x00; /* data sent to output , ouput is ready for another sending*/
    }
  }
  /* process auxiliary output */
  if (pbs3->output2_ready == 0x01) { /*  something pending for auxiliary output */
    data = pbs3->output2_data;
    ret = fputc(data, stderr);
    if (ret == EOF) 
    {
      switch (errno)
      {
        case EAGAIN : /* auxiliary output is not ready, another try is necessary*/
          break;
        case EBADF :
        case EIO :
          pbs3->output2_ready = 0xFF; /* auxiliary output is not usable*/
      }
    } 
    else
    {
      pbs3->output2_ready == 0x00; /* data sent to auxiliary output , auxiliary ouput is ready for another sending*/
    }
  }
  /* process input */
  if (pbs3->input_ready == 0x01) { /* input byte is consummed, then it is an opportunity to provide another input if there is one available */
    struct pollfd pfds[1];
    int ret;
    char c;
    
    /* See if there is data available */
    pfds[0].fd = 0;
    pfds[0].events = POLLIN;
    ret = poll(pfds, 1, 0);
    
    /* Consume Hypervisor input data, to provide as an available input data for the bs3 CPU */
    if (ret > 0 && ((pfds[0].revents & 1) == 1)) {
      read(0, &c, 1);
      pbs3->input_data = (BYTE)c;
      pbs3->input_ready == 0x00;
      bs3_cpu_interrupt(pbs3, BS3_INT_INPUT);
    }
  
  }
}


void bs3_hyper_main(BYTE * program, WORD programsize)
{
    struct termios oldtio, curtio;
    struct sigaction sa;

    /* Save stdin terminal attributes */
    tcgetattr(0, &oldtio);

    /* Make sure we exit cleanly */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sighandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    /* add timer alarm signal handling */
    sigaction(SIGVTALRM, &sa, NULL);

    /* This is needed to be able to tcsetattr() after a hangup (Ctrl-C)
     * see tcsetattr() on POSIX
     */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &sa, NULL);

    /* Set non-canonical no-echo for stdin */
    tcgetattr(0, &curtio);
    curtio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &curtio);

    /* BS3 CPU controlled by the Hypervisor */
    struct bs3_cpu_data cpu;
    /* prepare cpu memory */
    bs3_hyper_reset_memory(&cpu);
    bs3_hyper_load_memory(&cpu, program, programsize, 0);
    /* initialise cpu */
    bs3_cpu_init(&cpu);
    /* main loop */
    while (!end) {
      bs3_hyper_coreIO(&cpu);
      bs3_cpu_exec(&cpu);
      switch (cpu.status)
      {
        case BS3_STATUS_HALT: /* End of CPU, then end of hypervisor */
          end = 1;
          break;
        case BS3_STATUS_RESET: /* CPU reset requested */
          bs3_hyper_reset_memory(&cpu);
          bs3_hyper_load_memory(&cpu, program, programsize, 0);
          break;
        case BS3_STATUS_DEFAULT:
          /* Nothing particular to do for the hypervisor */
          break; 
        case BS3_STATUS_WAIT:
          break;
        case BS3_STATUS_HEVT:
          /* TODO : CPU notification */
          cpu.status = BS3_STATUS_DEFAULT;
          break;
        case BS3_STATUS_ESCHYP:
          /* TODO : CPU invoke hypervisor high level feature*/
          cpu.status = BS3_STATUS_DEFAULT;
          break;
        case BS3_STATUS_BADINSTR:
          /* TODO : Tried to a execute a bad instruction*/
          cpu.status = BS3_STATUS_DEFAULT;
          break;
        case BS3_STATUS_DIVZERO:
          /* TODO : a division by zero occurs*/
          cpu.status = BS3_STATUS_DEFAULT;
          break;
      }
    }
    /* restore terminal attributes */
    tcsetattr(0, TCSANOW, &oldtio);
}



void main() {
  struct bs3_registers reg;
  char line[100];
  WORD start = 0x1FFE;
  int i =0;
  for (i = 0; i <= 255; i++) 
  {
    start = bs3_cpu_disassemble(start,(BYTE)i, 0xE0, 0x8F, 0xF8, line);
    printf("%s\n",line);
  }
  reg.PC = 0xFAAF;
  reg.FL = 0x005A;
  reg.SP = 0x0400;
  reg.W[0] = 0xAAFF;
  BYTE x = 0xFF;
  reg.W[0] = reg.W[0] + ((SBYTE)x);
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
  printf("sizeof cpu %d\n", sizeof(struct bs3_cpu_data));
  
    struct termios oldtio, curtio;
    struct sigaction sa;

    /* Save stdin terminal attributes */
    tcgetattr(0, &oldtio);

    /* Make sure we exit cleanly */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sighandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    /* add timer alarm signal handling */
    sigaction(SIGVTALRM, &sa, NULL);

    /* This is needed to be able to tcsetattr() after a hangup (Ctrl-C)
     * see tcsetattr() on POSIX
     */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &sa, NULL);

    /* Set non-canonical no-echo for stdin */
    tcgetattr(0, &curtio);
    curtio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &curtio);

    /* main loop */
    while (!end) {
            struct pollfd pfds[1];
            int ret;
            char c;

            /* See if there is data available */
            pfds[0].fd = 0;
            pfds[0].events = POLLIN;
            ret = poll(pfds, 1, 0);

            /* Consume data */
            if (ret > 0 && ((pfds[0].revents & 1) == 1) ) {
                    /* printf("Data available\n"); */
                    read(0, &c, 1);
                    int d = (int) c;
                    int e = pfds[0].revents;
                    printf("revents %X, ret =%X, c=%d\n",e, ret,d);
            }
    }

    /* restore terminal attributes */
    tcsetattr(0, TCSANOW, &oldtio);

}
