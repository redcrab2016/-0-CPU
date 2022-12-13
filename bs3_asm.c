#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "bs3.h"

struct bs3_asm_line
{
  WORD linenum;
  char label[32];
  char ope[32];
  BYTE nbParam;
  char param1[20];
  char param2[20];
  char param3[20];
  char param4[20];
};

struct bs3_asm_line bs3_asm[65536];

 
#define BS3_ASM_PASS1_PARSE_STATE_START  0 
#define BS3_ASM_PASS1_PARSE_STATE_LABEL  1 
#define BS3_ASM_PASS1_PARSE_STATE_ALABEL 2 
#define BS3_ASM_PASS1_PARSE_STATE_OPE    3 
#define BS3_ASM_PASS1_PARSE_STATE_AOPE   4 
#define BS3_ASM_PASS1_PARSE_STATE_PARM1  5 
#define BS3_ASM_PASS1_PARSE_STATE_APARM1 6 
#define BS3_ASM_PASS1_PARSE_STATE_PARM2  7 
#define BS3_ASM_PASS1_PARSE_STATE_APARM2 8 
#define BS3_ASM_PASS1_PARSE_STATE_PARM3  9 
#define BS3_ASM_PASS1_PARSE_STATE_APARM3 10 
#define BS3_ASM_PASS1_PARSE_STATE_PARM4  11
#define BS3_ASM_PASS1_PARSE_STATE_APARM4 12

#define BS3_ASM_PASS1_PARSE_RETURN_OK    0
#define BS3_ASM_PASS1_PARSE_RETURN_NOPE  1
#define BS3_ASM_PASS1_PARSE_RETURN_FAIL  2
#define BS3_ASM_PASS1_PARSE_ERR_EMPTYLABEL  3
#define BS3_ASM_PASS1_PARSE_ERR_BADLABEL  4

 
int bs3_asm_pass1_oneline(WORD bs3_asm_idx, WORD linenum, char * oneLine)
{
  int idxLine = 0;
  int state = -1 ; 
  char c;
  char isok= 1;
  char err=BS3_ASM_PASS1_PARSE_RETURN_OK;
  bs3_asm[bs3_asm_idx].linenum = linenum;
  bs3_asm[bs3_asm_idx].label[0] = 0;
  bs3_asm[bs3_asm_idx].ope[0] = 0;
  bs3_asm[bs3_asm_idx].nbParam = 0;
  bs3_asm[bs3_asm_idx].param1[0] = 0;
  bs3_asm[bs3_asm_idx].param2[0] = 0;
  bs3_asm[bs3_asm_idx].param3[0] = 0;
  bs3_asm[bs3_asm_idx].param4[0] = 0;
  
  while (c = oneLine[idxline] && isok)
  {
      if (c == '\n' || c== '\r' || c== ';') break;
      switch(state)
      {
        case BS3_ASM_PASS1_PARSE_STATE_START:
          switch (c)
          {
            case ' ':
            case '\t':
              state = BS3_ASM_PASS1_PARSE_STATE_ALABEL;
              break;
            case ':':
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_EMPTYLABEL;
              break;
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_':
              state = BS3_ASM_PASS1_PARSE_STATE_LABEL;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;
          }
          break;
        case BS3_ASM_PASS1_PARSE_STATE_LABEL:
        break;
        case BS3_ASM_PASS1_PARSE_STATE_ALABEL: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_OPE: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_AOPE: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_PARM1: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_APARM1: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_PARM2: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_APARM2: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_PARM3: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_APARM3: 
        break;
        case BS3_ASM_PASS1_PARSE_STATE_PARM4:
        break;
        case BS3_ASM_PASS1_PARSE_STATE_APARM4:
        break;
      }
  }
  if (isok && state ==  BS3_ASM_PASS1_PARSE_STATE_START) return BS3_ASM_PASS1_PARSE_RETURN_NOPE;
  if (!isok) 
  {
    if (err == BS3_ASM_PASS1_PARSE_RETURN_OK) return BS3_ASM_PASS1_PARSE_RETURN_FAIL;
    return err;
  }
  
} 

void main() {
 
}
