
#include "bs3_asm.h"
#include "bs3_bus.h"

static const char * hexa_digit="0123456789ABCDEF";
 
const char * bs3_cpu_memory_dump_template = "0000  00 00 00 00 00 00 00 00  00 00 00 00 00 00 00 00  ................";

void bs3_cpu_memory_dump(struct bs3_cpu_data * pbs3, WORD addr, char * result)
{
  int i;
  BYTE value;
  int stridx;
  /* check if it is ok to continue */
  if (pbs3 == ((void *)0) || result == ((void *)0)) return;
  /* get the template into the result*/
  for (i = 0; bs3_cpu_memory_dump_template[i]; i++ ) result[i] = bs3_cpu_memory_dump_template[i];
  result[i] = 0; /* be sure that return result is ASCIIZ */
  /* fill the address */
  result[0] = hexa_digit[((addr >> 12) & 0x0F)];
  result[1] = hexa_digit[((addr >> 8 ) & 0x0F)];
  result[2] = hexa_digit[((addr >> 4 ) & 0x0F)];
  result[3] = hexa_digit[( addr        & 0x0F)];
  /* dump 16 bytes from addr to addr + 15 */ 
  for (i = 0; i < 16; i++)
  {
    value = bs3_bus_readByte(addr + i);
    /* set hexa byte value at right location */
    stridx = i * 3 + 6 + ((i<8)?0:1);
    result[stridx + 0] = hexa_digit[ (value >> 4) & 0x0F];
    result[stridx + 1] = hexa_digit[  value       & 0x0F];
    /* set character glyph */
    if (value > 31 && value < 127) result[56 + i] = value;/* print out ASCII non-contol chacracters and avoid UTF misunderstanding */
  }

}

/* disassemble instruction thanks to the 4 bytes (a, b, c and d) located at particular PC address */
/* return the location of the next instruction */  

WORD bs3_cpu_disassemble(struct bs3_cpu_data * pbs3, char * result)
{
  return bs3_cpu_disassemble_(pbs3->r.PC, 
                              bs3_bus_readByte(pbs3->r.PC), 
                              bs3_bus_readByte((pbs3->r.PC+1) & 0xFFFF), 
                              bs3_bus_readByte((pbs3->r.PC+2) & 0xFFFF), 
                              bs3_bus_readByte((pbs3->r.PC+3) & 0xFFFF),
                              result);
}

WORD bs3_cpu_disassemble_(WORD PC, BYTE a, BYTE b, BYTE c, BYTE d, char * result) {
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
        else /* Something wrong in the definition */
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
/*
I0|N0|V0|Z0|C0|W3  |W2  |W1  |W0  |
PC  |SP  |[SP]|B7B6|B5B4|B3B2|B1B0|
0000|0400|3F08|03A2|02B5|C0A3|0000|
*/
/*                                       01  4  7  10 13                                                           72   77   82   87   92   97   102    */
static const char * bs3_cpu_state_model="I_|N_|V_|Z_|C_|W3  |W2  |W1  |W0  |\nPC  |SP  |[SP]|B7B6|B5B4|B3B2|B1B0|\n____|____|____|____|____|____|____|\n\0";

void bs3_cpu_state(struct bs3_cpu_data * pbs3, char * result)
{
  int it = 0;
  /* copy template */
  while (bs3_cpu_state_model[it])
  {
    result[it] = bs3_cpu_state_model[it];
    it++;
  }
  result[it]=0;
  /* set the flags */
  result[1]  = hexa_digit[pbs3->r.I]; /* I flag */
  result[4]  = hexa_digit[pbs3->r.N]; /* N flag */
  result[7]  = hexa_digit[pbs3->r.V]; /* V flag */
  result[10] = hexa_digit[pbs3->r.Z]; /* Z flag */
  result[13] = hexa_digit[pbs3->r.C]; /* C flag */
  /* PC register */
  result[72] = hexa_digit[(pbs3->r.PC & 0xF000) >> 12]; 
  result[73] = hexa_digit[(pbs3->r.PC & 0x0F00) >> 8];
  result[74] = hexa_digit[(pbs3->r.PC & 0x00F0) >> 4]; 
  result[75] = hexa_digit[pbs3->r.PC & 0x000F];
  /* SP register */
  result[77] = hexa_digit[(pbs3->r.SP & 0xF000) >> 12]; 
  result[78] = hexa_digit[(pbs3->r.SP & 0x0F00) >> 8];
  result[79] = hexa_digit[(pbs3->r.SP & 0x00F0) >> 4]; 
  result[80] = hexa_digit[pbs3->r.SP & 0x000F];
  /* Word at SP register : [SP] */
  result[82] = hexa_digit[(bs3_bus_readByte(pbs3->r.SP+1) & 0xF0) >> 4]; 
  result[83] = hexa_digit[bs3_bus_readByte(pbs3->r.SP+1) & 0x0F];
  result[84] = hexa_digit[(bs3_bus_readByte(pbs3->r.SP) & 0xF0) >> 4]; 
  result[85] = hexa_digit[bs3_bus_readByte(pbs3->r.SP) & 0x0F];
  /* W3 register | B7 and B6 registers */
  result[87] = hexa_digit[(pbs3->r.W[3] & 0xF000) >> 12]; 
  result[88] = hexa_digit[(pbs3->r.W[3] & 0x0F00) >> 8];
  result[89] = hexa_digit[(pbs3->r.W[3] & 0x00F0) >> 4]; 
  result[90] = hexa_digit[pbs3->r.W[3] & 0x000F];
  /* W2 register | B5 and B4 registers */
  result[92] = hexa_digit[(pbs3->r.W[2] & 0xF000) >> 12]; 
  result[93] = hexa_digit[(pbs3->r.W[2] & 0x0F00) >> 8];
  result[94] = hexa_digit[(pbs3->r.W[2] & 0x00F0) >> 4]; 
  result[95] = hexa_digit[pbs3->r.W[2] & 0x000F];
  /* W1 register | B3 and B2 registers */
  result[97] = hexa_digit[(pbs3->r.W[1] & 0xF000) >> 12]; 
  result[98] = hexa_digit[(pbs3->r.W[1] & 0x0F00) >> 8];
  result[99] = hexa_digit[(pbs3->r.W[1] & 0x00F0) >> 4]; 
  result[100] = hexa_digit[pbs3->r.W[1] & 0x000F];
  /* W0 register | B1 and B0 registers */
  result[102] = hexa_digit[(pbs3->r.W[0] & 0xF000) >> 12]; 
  result[103] = hexa_digit[(pbs3->r.W[0] & 0x0F00) >> 8];
  result[104] = hexa_digit[(pbs3->r.W[0] & 0x00F0) >> 4]; 
  result[105] = hexa_digit[pbs3->r.W[0] & 0x000F];
}
