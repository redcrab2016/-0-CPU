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

#include "bs3.h"
#include "bs3_bus.h"

/* Signal handling (end process, timer alarm) */
static sig_atomic_t end = 0;
static sig_atomic_t timer_alarm = 0;

static void sighandler(int signo)
{
  if (signo == SIGALRM /* or SIGVTALRM */) { 
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
  setitimer(ITIMER_REAL /* or ITIMER_VIRTUAL */ ,&period,NULL);
}

void bs3_hyper_timerstop()
{
  bs3_hyper_timerset(0);
}


struct bs3_cpu_data * bs3_cpu = NULL; /* bs3 cpu used by bs3 bus when cpu memory need to be access by the bus*/

extern struct bs3_device dev_memory;
extern struct bs3_device dev_rtc72421;
extern struct bs3_device dev_bs3irqctrl;
extern struct bs3_device dev_bs3inout;

void bs3_hyper_device_prepare()
{
  bs3_bus_plugdevice(&dev_memory);
  bs3_bus_plugdevice(&dev_bs3inout);
  bs3_bus_plugdevice(&dev_rtc72421);
  bs3_bus_plugdevice(&dev_bs3irqctrl);
}

void bs3_hyper_device_start()
{
  /* if call twice without stop, then it is a restart*/
  bs3_bus_start();
}

void bs3_hyper_device_stop()
{
  bs3_bus_stop();
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
  pbs3->msortick = 0; /* by default microsecond timer ( if =1, then cpu clock is used) */
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
 //     case 0x0100: /* write on input data is ignored */
 //     case 0x0101: /* write on input status is ignored */
 //       break;
 //     case 0x0102:
 //       if (pbs3->m[0x0103] == 0x00) /* if ok to write on output */ 
 //       {
 //         pbs3->m[0x0103]  = 0x01; /* output is waiting to be consummed */
 //         pbs3->m[address] = data; /* output data available */
 //       }
 //       break;
 //     case 0x0103: /* write on output status is ignored */
 //       break;
 //     case 0x0104:
 //       if (pbs3->m[0x0105] == 0x00) /* if ok to write on auxiliary output */
 //       {
//          pbs3->m[0x0105]  = 0x01; /* output is waiting to be consummed */
//          pbs3->m[address] = data; /* output data available */
//        }
//        break;
//      case 0x0105: /* write on auxiliary output status is ignored */ 
//        break;
      case 0x0108: /* low byte of the low 16 bits of the 32 bits timer */
      case 0x0109: /* high byte of the low 16 bits of the 32 bits timer */
      case 0x010A: /* low byte of the high 16 bits of the 32 bits timer */
      case 0x010C: /* ms(0) or tick timer(1) */
        pbs3->m[address] = data;
        break;
      case 0x010B: /* high byte of the high 16 bits of the 3é bits timer : at this write, timer is restarted */
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
        bs3_bus_writeByte(address,data);
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
//      case 0x0100:
//        if (pbs3->m[0x101] == 0x00)
//        {
//          pbs3->m[0x0101] = 0x01; /* core input consummed */
//        }
//        return pbs3->m[address]; 
//        break;
//      case 0x0101: /* core input status */
//      case 0x0102: /* core output */
//      case 0x0103: /* core output status */
//      case 0x0104: /* core auxiliary output */
//      case 0x0105: /* core auxliiary output status */
//      case 0x0106: 
//      case 0x0107:
      case 0x0108: /* timer 32 low, 16 bits low */
      case 0x0109: /* timer 32 low, 16 bit high */
      case 0x010A: /* timer 32 high, 16 bits low */
      case 0x010B: /* timer 32 high, 16 bits high */ 
      case 0x010C: /* msortick */
        return pbs3->m[address];
        break;
      default:
      /* manage other system  I/O read, */
        return bs3_bus_readByte(address);
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


BYTE bs3_memory_readbyte(WORD address)
{
  if (bs3_cpu) return bs3_cpu->m[address];
  return 0;
}

void bs3_memory_writebyte(WORD address, BYTE data)
{
  if (bs3_cpu == 0) return;
  bs3_cpu->m[address] = data;
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
  int busInterrupt;
  if (pbs3->status == BS3_STATUS_HALT) return; 
  if (pbs3->status == BS3_STATUS_RESET) 
  {
    bs3_cpu_init(pbs3);
    return;
  }
  /* is there pending interrupt, with interrupt enabled?*/
  busInterrupt = -1;
  if (pbs3->r.I == 1 && pbs3->pending_interrupt != BS3_INT_NOPENDING) {
    busInterrupt = pbs3->pending_interrupt;
    bs3_cpu_interrupt(pbs3, pbs3->pending_interrupt);
    
  }
  else /* bus or timer interrupt ?*/
  {
    if (pbs3->msortick == 1)
    {
      pbs3->counter--;
      if (pbs3->counter == 0)  
      {
        bs3_cpu_interrupt(pbs3, BS3_INT_TIMER);
        busInterrupt = BS3_INT_TIMER;
        pbs3->counter == pbs3->timer;
      }
    } else {
      if (timer_alarm == 1) {
        bs3_cpu_interrupt(pbs3, BS3_INT_TIMER);
        busInterrupt = BS3_INT_TIMER;
        timer_alarm = 0;
      }
    }
    /* bus interrupt ?*/
    if (busInterrupt == -1 && pbs3->r.I == 1) /* do not consume interrupt from bus if cpu mask interrupt */
    {
      busInterrupt = bs3_bus_getinterrupt();
      if (busInterrupt > -1) bs3_cpu_interrupt(pbs3, busInterrupt);
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
            if (!pbs3->r.Z) bs3_cpu_write_byte(pbs3, 0x0102, pbs3->r.B[p.x3]); // write occurs if output is ready to receive data
            break;
        case BS3_INSTR_OUTB2:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0105); // read output status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            if (!pbs3->r.Z) bs3_cpu_write_byte(pbs3, 0x0104, pbs3->r.B[p.x3]); // write occurs if output is ready to receive data
        case BS3_INSTR_OUTBI:
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0103); // read output status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            if (!pbs3->r.Z) bs3_cpu_write_byte(pbs3, 0x0102, immB); // write occurs if output is ready to receive data
            break;
        case BS3_INSTR_OUTB2I:
            pbs3->r.PC++;
            immB = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            b = bs3_cpu_read_byte(pbs3, 0x0105); // read output status
            pbs3->r.Z = (b == 0x01); 
            pbs3->r.V = (b == 0xFF);
            if (!pbs3->r.Z) bs3_cpu_write_byte(pbs3, 0x0104, immB); // write occurs if output is ready to receive data
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
            if (pbs3->r.C == 0 || pbs3->r.Z == 1) 
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
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)(pbs3->r.W[p.y2]));
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
            dw = ((DWORD)(pbs3->r.W[p.x2])) + ((DWORD)(pbs3->r.W[p.y2])) + pbs3->r.C;
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
            break;
        case BS3_INSTR_MOVSW:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.SP = pbs3->r.W[p.x2];
            break;
        case BS3_INSTR_MOVWS:
            pbs3->r.PC++;
            p.param = bs3_cpu_read_byte(pbs3, pbs3->r.PC);
            pbs3->r.PC++;
            pbs3->r.W[p.x2] = pbs3->r.SP;
            break;
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
  long i;
  for (i=0; i < 65536; i++) pbs3->m[i] = 0;
}

void bs3_hyper_load_memory(struct bs3_cpu_data * pbs3, struct bs3_asm_code_map * pcodemap)
{
  long i;
  for (i=0; i < 65536; i++) 
  { 
    if (pcodemap->inUse[i])  pbs3->m[i & 0xFFFF] = pcodemap->code[i];
  }
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
      pbs3->output_ready = 0x00; /* data sent to output , ouput is ready for another sending*/
      /* fflush(stdout); */
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
      pbs3->output2_ready = 0x00; /* data sent to auxiliary output , auxiliary ouput is ready for another sending*/
    }
  }
  /* process input */
  if (pbs3->input_ready == 0x01) { /* input byte is consummed, then it is an opportunity to provide another input if there is one available */
    struct pollfd pfds[1];
    int ret;
    char c;
    /* empiric wait for input latency to avoid too much poll syscall (slow down the BS3 emulator)*/
    static unsigned long curwait = 1;
    static unsigned long maxwait = BS3_HYPER_INPUT_INITWAIT;

    curwait--;
    if (curwait == 0) /* ==0 then it is time to check for pending input */
    {
      /* See if there is data available */
      pfds[0].fd = 0;
      pfds[0].events = POLLIN;
      ret = poll(pfds, 1, 0);
      
      /* Consume Hypervisor input data, to provide as an available input data for the bs3 CPU */
      if (ret > 0 && ((pfds[0].revents & 1) == 1)) {
        maxwait = BS3_HYPER_INPUT_INITWAIT;
        curwait = 1;
        read(0, &c, 1);
        pbs3->input_data = (BYTE)c;
        pbs3->input_ready = 0x00;
        bs3_cpu_interrupt(pbs3, BS3_INT_INPUT);
      } 
      else
      {
        curwait = maxwait;
        maxwait = maxwait << 1;
        if (maxwait > BS3_HYPER_INPUT_MAXWAIT) maxwait = BS3_HYPER_INPUT_MAXWAIT;
      }
    }  
  }
}


void bs3_hyper_main(struct bs3_asm_code_map * pcodemap, void (*debugf)(struct bs3_cpu_data *)) /* BYTE * program, WORD programsize) */
{
    struct termios oldtio, curtio;
    struct sigaction sa;
    BYTE prevBS3status;
    /* Save stdin terminal attributes */
    tcgetattr(0, &oldtio);

    /* Make sure we exit cleanly */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sighandler;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGQUIT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    /* add timer alarm signal handling */
    sigaction(SIGALRM /* or SIGVTALRM */ , &sa, NULL); 

    /* This is needed to be able to tcsetattr() after a hangup (Ctrl-C)
     * see tcsetattr() on POSIX
     */
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = SIG_IGN;
    sigaction(SIGTTOU, &sa, NULL);

    /* Set non-canonical no-echo for stdin */
    tcgetattr(0, &curtio);
    curtio.c_lflag &= ~(ICANON | ECHO);
    //curtio.c_lflag &= ~( ECHO);
    tcsetattr(0, TCSANOW, &curtio);

    /* BS3 CPU controlled by the Hypervisor */
    struct bs3_cpu_data cpu;
    bs3_cpu = &cpu;
    /* prepare cpu memory */
    bs3_hyper_reset_memory(&cpu);
    bs3_hyper_load_memory(&cpu, pcodemap);
    /* initialise cpu */
    bs3_cpu_init(&cpu);
    /* initialize the devices*/
    bs3_hyper_device_prepare();
    bs3_hyper_device_start();
    /* main loop */
    while (!end) {
      /*bs3_hyper_coreIO(&cpu);*/
      if (debugf)
      {
        prevBS3status = cpu.status;
        (*debugf)(&cpu);
        if (cpu.status == BS3_STATUS_RESET && prevBS3status != cpu.status) /* CPU reset requested by debuuger */
        {
          bs3_hyper_device_start();
          bs3_hyper_reset_memory(&cpu);
          bs3_hyper_load_memory(&cpu, pcodemap); /*program, programsize, 0); */
        }
      } 
      bs3_cpu_exec(&cpu);
      
      switch (cpu.status)
      {
        case BS3_STATUS_HALT: /* End of CPU, then end of hypervisor */
          if (debugf)
          {
            (*debugf)(&cpu);
            if (cpu.status == BS3_STATUS_HALT) end = 1;
          }
          else
            end = 1;
          break;
        case BS3_STATUS_RESET: /* CPU reset requested by program */
          bs3_hyper_device_start();
          bs3_hyper_reset_memory(&cpu);
          bs3_hyper_load_memory(&cpu, pcodemap); /*program, programsize, 0); */
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
    bs3_hyper_device_stop();
}

/* declare memory device */
struct bs3_device dev_memory = 
{
    .name="Memory",
    .address=0x0000,
    .mask=0x0000,
    .startdevice = NULL, /* no start device function */
    .stopdevice = NULL,  /* no stop device function */
    .readByte = &bs3_memory_readbyte,
    .writeByte = &bs3_memory_writebyte,
    .interruptNumber = 0 /* No interrupt */
};