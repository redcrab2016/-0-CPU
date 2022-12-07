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

#define BS3_INT_NOPENDING 0xFF
#define BS3_INT_BADINSTR 7
#define BS3_INT_TIMER 3
#define BS3_INT_INPUT 4

#define BYTE unsigned char
#define SBYTE signed char
#define WORD unsigned short
#define SWORD signed short
#define BIT unsigned short
#define DWORD unsigned long
#define SDWORD signed long

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

/* operator param (optional second byte of an instruction) */
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
  /* CZVN and I = 0 (disallow interruption) */
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
  WORD w;
  DWORD dw;
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
        case LEAW1:
        case LEAW2:
        case LEAW3:
            pbs3->r.PC++;
            pbs3->r.W[ope & 0x03] = pbs3->r.PC + ((SWORD)bs3_cpu_read_word(pbs3,  pbs3->r.PC)) + 2;
            pbs3->r.PC += 2; 
            break;
        case LEAB0:
        case LEAB1:
        case LEAB2:
        case LEAB3:
            pbs3->r.PC++;
            pbs3->r.W[ope & 0x03] = pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3,  pbs3->r.PC)) + 1;
            pbs3->r.PC ++; 
            break;
        case CLC:
            pbs3->r.PC++;
            pbs3->r.C = 0;
            break;
        case CLZ:
            pbs3->r.PC++;
            pbs3->r.Z = 0;
            break;
        case CLV:
            pbs3->r.PC++;
            pbs3->r.V = 0;
            break;
        case CLN:
            pbs3->r.PC++;
            pbs3->r.N = 0;
            break;
        case STC:
            pbs3->r.PC++;
            pbs3->r.C = 1;
            break;
        case STZ:
            pbs3->r.PC++;
            pbs3->r.Z = 1;
            break;
        case STV:
            pbs3->r.PC++;
            pbs3->r.V = 1;
            break;
        case STN:
            pbs3->r.PC++;
            pbs3->r.N = 1;
            break;
        case INT1:
            pbs3->r.PC++;
            pbs3->status = BS3_STATUS_ESCHYP; /* Hypervisor high level function requested */
            break;
        case INT0:
        case INT2:
        case INT3:
        case INT4:
        case INT5:
        case INT6:
        case INT7:
        case INT8:
        case INT9:
        case INT10:
        case INT11:
        case INT12:
        case INT13:
        case INT14:
        case INT15:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC);
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.FL);
            pbs3->r.PC = pbs3->vector[ope & 0x0F];
            break;
        case J_W0:
        case J_W1:
        case J_W2:
        case J_W3:
            pbs3->r.PC = pbs3->r.W[ope & 0x03];
            break;
        case C_W0:
        case C_W1:
        case C_W2:
        case C_W3:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC); 
            pbs3->r.PC = pbs3->r.W[ope & 0x03];           
            break;
        case RET:
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case IRET:
            pbs3->r.FL = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case J_A:
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.PC + 1);
            break;
        case C_A:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC + 2);
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.PC); 
            break;
        case J_R:
            pbs3->r.PC++;
            pbs3->r.PC = 1 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            break;
        case C_R:
            pbs3->r.PC++;
            pbs3->r.SP -= 2;
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC + 1);
            pbs3->r.PC = 1 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            break;
        case JZ:
            if (pbs3->r.Z == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JNZ:
            if (pbs3->r.Z == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JC:
            if (pbs3->r.C == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JNC:
            if (pbs3->r.C == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JN:
            if (pbs3->r.N == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JNN:
            if (pbs3->r.N == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JV:
            if (pbs3->r.V == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JNV:
            if (pbs3->r.V == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JA:
            if (pbs3->r.C == 1 && pbs3->r.Z == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JBE:
            if (pbs3->r.C == 0 && pbs3->r.Z == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JGE:
            if (pbs3->r.N == pbs3->r.V) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JL:
            if (pbs3->r.N != pbs3->r.V) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JG:
            if (pbs3->r.N == pbs3->r.V && pbs3->Z == 0) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case JLE:
            if (pbs3->r.N != pbs3->r.V || pbs3->Z == 1) 
              pbs3->r.PC = 2 + pbs3->r.PC + ((SBYTE)bs3_cpu_read_byte(pbs3, pbs3->r.PC+1));
            else 
              pbs3->r.PC += 2;
            break;
        case LDBm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC));
            pbs3->r.PC += 2; 
            break;
        case LDBm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2]);
            break;
        case LDBm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC ++; 
            break;
        case LDBm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2]);
            break;
        case LDBm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case LDBm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2]);
            break;
        case LDBm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case LDBm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = bs3_cpu_read_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case LDWm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC));
            pbs3->r.PC += 2; 
            break;
        case LDWm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2]);
            break;
        case LDWm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case LDWm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2]);
            break;
        case LDWm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case LDWm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2]);
            break;
        case LDWm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case LDWm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = bs3_cpu_read_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC));
            pbs3->r.PC++; 
            break;
        case SRBm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC += 2; 
            break;
        case SRBm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2], );
            break;
        case SRBm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC ++; 
            break;
        case SRBm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2], pbs3->r.B[p.x3]);
            break;
        case SRBm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC++; 
            break;
        case SRBm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2], pbs3->r.B[p.x3]);
            break;
        case SRBm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC++; 
            break;
        case SRBm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_byte(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.B[p.x3]);
            pbs3->r.PC++; 
            break;
        case SRWm0:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, bs3_cpu_read_word(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC += 2; 
            break;
        case SRWm1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2], pbs3->r.W[p.x2]);
            break;
        case SRWm2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case SRWm3:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2], pbs3->r.W[p.x2]);
            break;
        case SRWm4:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.SP + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case SRWm5:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2], pbs3->r.W[p.x2]);
            break;
        case SRWm6:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.SP + pbs3->r.W[p.y2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case SRWm7:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            bs3_cpu_write_word(pbs3, pbs3->r.W[p.y2] + pbs3->r.W[p.z2] + bs3_cpu_read_byte(pbs3, pbs3->r.PC), pbs3->r.W[p.x2]);
            pbs3->r.PC++; 
            break;
        case POPB0:
        case POPB1:
        case POPB2:
        case POPB3:
        case POPB4:
        case POPB5:
        case POPB6:
        case POPB7:
            pbs3->r.PC ++;
            pbs3->r.B[ope & 0x07] = (BYTE)(bs3_cpu_read_word(pbs3, pbs3->r.SP) & 0x00FF);
            pbs3->r.SP += 2; 
            break;
        case POPW0:
        case POPW1:
        case POPW2:
        case POPW3:
            pbs3->r.PC ++;
            pbs3->r.W[ope & 0x03] = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2; 
            break;
        case POPA:
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
        case POPF:
            pbs3->r.PC ++;
            pbs3->r.FL = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case POPPC:
            pbs3->r.PC = bs3_cpu_read_word(pbs3, pbs3->r.SP);
            pbs3->r.SP += 2;
            break;
        case DROP:
            pbs3->r.PC ++;
            pbs3->r.SP += 2;
            break;
        case PUSHB0:
        case PUSHB1:
        case PUSHB2:
        case PUSHB3:
        case PUSHB4:
        case PUSHB5:
        case PUSHB6:
        case PUSHB7:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, (WORD)pbs3->r.B[ope & 0x07]);
            break;
        case PUSHW0:
        case PUSHW1:
        case PUSHW2:
        case PUSHW3:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.W[ope & 0x03]);
            break;
        case PUSHA:
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
        case PUSHF:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.FL);
            break;
        case PUSHPC:
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, pbs3->r.PC);
            pbs3->r.PC ++;
            break;
        case DUP:
            pbs3->r.PC ++;
            pbs3->r.SP -= 2; 
            bs3_cpu_write_word(pbs3, pbs3->r.SP, bs3_cpu_read_word(pbs3, pbs3->r.SP + 2 ));
            break;
        case ANDB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= pbs3->r.B[p.y3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case ORB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] |= pbs3->r.B[p.y3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case EORB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] ^= pbs3->r.B[p.y3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BICB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= ~(pbs3->r.B[p.y3]);
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case TSTB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = pbs3->r.B[p.x3] & pbs3->r.B[p.y3];
            pbs3->r.N = (b & 0x80) != 0;
            pbs3->r.Z = (b == 0);
            break;
        case SHLB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((WORD)(pbs3->r.B[p.x3])) << pbs3->r.B[p.y3];
            pbs3->r.B[p.x3] =(BYTE)(w & 0x00FF);
            pbs3->r.N = (w & 0x0080) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0100) !=0;
            break;
        case SHRB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  (((WORD)(pbs3->r.B[p.x3])) << 8) >> pbs3->r.B[p.y3];
            pbs3->r.B[p.x3] = (BYTE)((w & 0xFF00)>>8);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0080) !=0;
            break;
        case SARB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((SWORD)(((WORD)(pbs3->r.B[p.x3])) << 8)) >> pbs3->r.B[p.y3];
            pbs3->r.B[p.x3] = (BYTE)((w & 0xFF00)>>8);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0080) !=0;
            break;
        case RORB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = pbs3->r.B[p.x3] & 0x01;
            pbs3->r.B[p.x3] >>= 1;
            pbs3->r.B[p.x3] |= (pbs3->r.C << 7);
            pbs3->r.N = pbs3->r.C;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case ROLB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.B[p.x3] <<= 1;
            pbs3->r.B[p.x3] |= pbs3->r.C;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case NOTB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = ~pbs3->r.B[p.x3];
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case ADDB:
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
        case ADCB:
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
        case SUBB:
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
        case SBBB:
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
        case CMPB:
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
        case ANDW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] &= pbs3->r.W[p.y2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case ORW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] |= pbs3->r.W[p.y2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case EORW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] ^= pbs3->r.W[p.y2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BICW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] &= ~(pbs3->r.W[p.y2]);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case TSTW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w = pbs3->r.W[p.x2] & pbs3->r.W[p.y2];
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case SHLW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((DWORD)(pbs3->r.W[p.x2])) << pbs3->r.B[p.y3];
            pbs3->r.W[p.x2] = dw & 0x0000FFFF;
            pbs3->r.N = (dw & 0x00008000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            pbs3->r.C = (dw & 0x00010000) !=0;
            break;
        case SHRW: // see in doc also as SHRWB
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  (((DWORD)(pbs3->r.W[p.x2])) << 16) >> pbs3->r.B[p.y3];
            pbs3->r.W[p.x2] = (WORD)((dw & 0xFFFF0000)>>16);
            pbs3->r.N = (dw & 0x80000000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            pbs3->r.C = (w & 0x00008000) !=0;
            break;
        case SARW: // see in doc also as SARWB
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((SDWORD)(((DWORD)(pbs3->r.W[p.x2])) << 16)) >> pbs3->r.B[p.y3];
            pbs3->r.W[p.x2] = (WORD)((dw & 0xFFFF0000)>>16);
            pbs3->r.N = (dw & 0x80000000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            pbs3->r.C = (w & 0x00008000) !=0;
            break;
        case RORW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = pbs3->r.W[p.x2] & 0x0001;
            pbs3->r.W[p.x2] >>= 1;
            pbs3->r.W[p.x3] |= (pbs3->r.C << 15);
            pbs3->r.N = pbs3->r.C;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case ROLW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.W[p.x2] <<= 1;
            pbs3->r.W[p.x2] |= pbs3->r.C;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case NOTW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = ~pbs3->r.W[p.x2];
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case ADDW:
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
        case ADCW:
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
        case SUBW:
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
        case SBBW:
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
        case CMPW:
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
        case MULB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((WORD)pbs3->r.B[p.x3]) * ((WORD)pbs3->r.B[p.y3]);
            pbs3->r.B[p.x3] = w & 0x00FF; 
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case IMULB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            // TODO 
            break;
        case DIVB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            // TODO 
            break;
        case IDIVB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            // TODO 
            break;
        case MOVB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = pbs3->r.B[p.y3];
            break;
        case SWPB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = pbs3->r.B[p.x3];
            pbs3->r.B[p.x3] = pbs3->r.B[p.y3];
            pbs3->r.B[p.y3] = b;
            break;
        case MULW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((DWORD)pbs3->r.B[p.x3]) * ((DWORD)pbs3->r.B[p.y3]);
            pbs3->r.W[p.x2] = dw & 0x0000FFFF; 
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case IMULW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            // TODO 
            break;
        case DIVW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            // TODO 
            break;
        case IDIVW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            // TODO 
            break;
        case MOVW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = pbs3->r.W[p.y2];
        case MOVSW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = pbs3->r.SP;
        case MOVWS:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.SP = pbs3->r.W[p.x2];
        case SWPW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w = pbs3->r.W[p.x2];
            pbs3->r.W[p.x2] = pbs3->r.W[p.y2];
            pbs3->r.W[p.y2] = w;
            break;
        case ANDBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case ORBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] |= b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case EORBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] ^= b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case BICBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] &= ~b;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case TSTBI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b &= pbs3->r.B[p.x3];
            pbs3->r.N = (b & 0x80) != 0;
            pbs3->r.Z = (b == 0);
            break;
        case SHLB1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.B[p.x3] & 0x80) !=0;
            pbs3->r.B[p.x3] <<= 1;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case SHRB1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.B[p.x3] & 0x01) !=0;
            pbs3->r.B[p.x3] >>= 1;
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case SARB1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            w =  ((SWORD)(((WORD)(pbs3->r.B[p.x3])) << 8)) >> 1;
            pbs3->r.B[p.x3] = (BYTE)((w & 0xFF00)>>8);
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x0080) !=0;
            break;
        case INCB:
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
        case DECB:
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
        case NEGB:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.B[p.x3] = (BYTE)(-((SBYTE)(pbs3->r.B[p.x3])));
            pbs3->r.N = (pbs3->r.B[p.x3] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            break;
        case ADDBI:
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
        case ADCBI:
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
        case SUBBI:
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
        case SBBBI:
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
        case CMPBI:
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
        case ANDWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] &= immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case ORWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] |= immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case EORWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] ^= immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case BICWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            pbs3->r.W[p.x2] &= ~immW;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case TSTWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            w = pbs3->r.W[p.x2] & immW;
            pbs3->r.N = (w & 0x8000) != 0;
            pbs3->r.Z = (w == 0);
            break;
        case SHLW1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.W[p.x2] & 0x8000) !=0;
            pbs3->r.W[p.x2] <<= 1;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case SHRW1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.C = (pbs3->r.W[p.x2] & 0x01) !=0;
            pbs3->r.W[p.x2] >>= 1;
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x80) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case SARW1:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            dw =  ((SDWORD)(((DWORD)(pbs3->r.W[p.x2])) << 16)) >> 1;
            pbs3->r.W[p.x2] = (WORD)((dw & 0xFFF0000)>>16);
            pbs3->r.N = (dw & 0x80000000) != 0;
            pbs3->r.Z = (pbs3->r.B[p.x3] == 0);
            pbs3->r.C = (w & 0x00008000) !=0;
            break;
        case INCW:
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
        case DECW:
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
        case NEGW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = (WORD)(-((SWORD)(pbs3->r.W[p.x2])));
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case ADDWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC +=2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */ 
            s2 = (immW & 0x8000) != 0; /* keep sign of second */
            dw = ((DWORD)(pbs3->r.w[p.x2])) + ((DWORD)immW);
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case ADCWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */
            s2 = (immW & 0x8000) != 0; /* keep sign of second */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)immW) + pbs3->r.C;
            s3 = (dww & 0x00008000) != 0; /* keep sign of first+second+carry */
            pbs3->r.W[p.x2] = (WORD)(dw & 0x0000FFFF);
            pbs3->r.C = (dww & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && ( s3 != s1); 
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case SUBWI:
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
            pbs3->r.C = (dww & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case SBBWI:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            immW = ~bs3_cpu_read_word(pbs3, pbs3->r.PC);
            pbs3->r.PC += 2;
            s1 = (pbs3->r.W[p.x2] & 0x8000) != 0; /* keep sign of first */
            s2 = (immW & 0x8000) == 0; /* keep sign of not(second) */
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)immW) + pbs3->r.C; /* first + not(second) + carry */
            pbs3->r.W[p.x2] = (WORD)(dww & 0x0000FFFF);
            s3 = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.C = (dw & 0x00010000) != 0;
            pbs3->r.V = (s1 == s2) && (s3 != s1);
            pbs3->r.N = (pbs3->r.W[p.x2] & 0x8000) != 0;
            pbs3->r.Z = (pbs3->r.W[p.x2] == 0);
            break;
        case CMPWI:
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
            pbs3->r.PC++;
            pbs3->r.I = 1;
            break;
        case CLI:
            pbs3->r.PC++;
            pbs3->r.I = 0;
            break;
        case HEVT:
            pbs3->r.PC++;
            pbs3->status = BS3_STATUS_HEVT;
            break;
        case WAIT:
            pbs3->r.PC++;
            pbs3->status = BS3_STATUS_WAIT;
            break;
        case RESET:
            pbs3->status = BS3_STATUS_RESET;
            break;
        case HLT:
            pbs3->status = BS3_STATUS_HALT;
            break;
        default:
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
      bs3_cpu(&cpu);
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
      }
    }
    /* restore terminal attributes */
    tcsetattr(0, TCSANOW, &oldtio);
}



void main() {
  struct bs3_registers reg;
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
