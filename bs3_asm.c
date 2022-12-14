#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "bs3.h"

struct bs3_asm_line
{
  WORD linenum;
  char line[74];
  union {
    SBYTE label;
    SBYTE column; /* if parsing error, then it indicates the character index in the line where the error has been detected */
  }
  SBYTE ope;
  BYTE nbParam;
  SBYTE param[40];
  BYTE paramType[40];
};

/* param types */
#define BS3_ASM_PARAM_TYPE_UNKNOWN       0
#define BS3_ASM_PARAM_TYPE_REGISTER_B    1
#define BS3_ASM_PARAM_TYPE_REGISTER_W    2
#define BS3_ASM_PARAM_TYPE_REGISTER_SP   3
#define BS3_ASM_PARAM_TYPE_DECIMAL       4
#define BS3_ASM_PARAM_TYPE_HEXA          5
#define BS3_ASM_PARAM_TYPE_STRING        6
#define BS3_ASM_PARAM_TYPE_CHAR          7
#define BS3_ASM_PARAM_TYPE_SYMBOL        8
#define BS3_ASM_PARAM_TYPE_M0DECIMAL     9
#define BS3_ASM_PARAM_TYPE_M0HEXA        10
#define BS3_ASM_PARAM_TYPE_M0SYMBOL      11
#define BS3_ASM_PARAM_TYPE_M1            12
#define BS3_ASM_PARAM_TYPE_M2DECIMAL     13
#define BS3_ASM_PARAM_TYPE_M2HEXA        14
#define BS3_ASM_PARAM_TYPE_M2SYMBOL      15
#define BS3_ASM_PARAM_TYPE_M3            16
#define BS3_ASM_PARAM_TYPE_M4DECIMAL     17
#define BS3_ASM_PARAM_TYPE_M4HEXA        18
#define BS3_ASM_PARAM_TYPE_M4SYMBOL      19
#define BS3_ASM_PARAM_TYPE_M5            20
#define BS3_ASM_PARAM_TYPE_M6DECIMAL     21
#define BS3_ASM_PARAM_TYPE_M6HEXA        22
#define BS3_ASM_PARAM_TYPE_M6SYMBOL      23
#define BS3_ASM_PARAM_TYPE_M7DECIMAL     24
#define BS3_ASM_PARAM_TYPE_M7HEXA        25
#define BS3_ASM_PARAM_TYPE_M7SYMBOL      26


/* one asm line pass 1 parsing states */ 
#define BS3_ASM_PASS1_PARSE_STATE_START        0 
#define BS3_ASM_PASS1_PARSE_STATE_LABEL        1 
#define BS3_ASM_PASS1_PARSE_STATE_ALABEL       2 
#define BS3_ASM_PASS1_PARSE_STATE_OPE          3 
#define BS3_ASM_PASS1_PARSE_STATE_AOPE         4 
#define BS3_ASM_PASS1_PARSE_STATE_PARAM        5 
#define BS3_ASM_PASS1_PARSE_STATE_PARAM_STRING 6 
#define BS3_ASM_PASS1_PARSE_STATE_PARAM_CHAR   7 
#define BS3_ASM_PASS1_PARSE_STATE_PARAM_NUMBER 8
#define BS3_ASM_PASS1_PARSE_STATE_PARAM_HEXA   9
#define BS3_ASM_PASS1_PARSE_STATE_APARAM       10 

/* Error messages */
#define BS3_ASM_PASS1_PARSE_ERR_OK           0
#define BS3_ASM_PASS1_PARSE_ERR_NOPE         1
#define BS3_ASM_PASS1_PARSE_ERR_FAIL         2
#define BS3_ASM_PASS1_PARSE_ERR_LINETOOLONG  3
#define BS3_ASM_PASS1_PARSE_ERR_EMPTYLABEL   4
#define BS3_ASM_PASS1_PARSE_ERR_BADLABEL     5
#define BS3_ASM_PASS1_PARSE_ERR_BADOPE       6
#define BS3_ASM_PASS1_PARSE_ERR_BADPARAM     7
#define BS3_ASM_PASS1_PARSE_ERR_BADSTRING    8
#define BS3_ASM_PASS1_PARSE_ERR_BADCHAR      9
#define BS3_ASM_PASS1_PARSE_ERR_BADNUMBER    10
#define BS3_ASM_PASS1_PARSE_ERR_BADREGISTER  11
#define BS3_ASM_PASS1_PARSE_ERR_BADSYMBOL    12
#define BS3_ASM_PASS1_PARSE_ERR_KEYWORD      13

static const char * bs3_asm_message[]=
{
  [BS3_ASM_PASS1_PARSE_ERR_OK]           = "Parse succeeded",
  [BS3_ASM_PASS1_PARSE_ERR_NOPE]         = "Nothing to parse",
  [BS3_ASM_PASS1_PARSE_ERR_FAIL]         = "General error",
  [BS3_ASM_PASS1_PARSE_ERR_LINETOOLONG]  = "Line too long (max 72 characters)",
  [BS3_ASM_PASS1_PARSE_ERR_EMPTYLABEL]   = "Empty label",
  [BS3_ASM_PASS1_PARSE_ERR_BADLABEL]     = "Bad label definition",
  [BS3_ASM_PASS1_PARSE_ERR_BADOPE]       = "Bad operation definition",
  [BS3_ASM_PASS1_PARSE_ERR_BADPARAM]     = "Bad parameter definition",
  [BS3_ASM_PASS1_PARSE_ERR_BADSTRING]    = "Bad string parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADCHAR]      = "Bad one character parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADNUMBER]    = "Bad decimal parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADREGISTER]  = "Bad register parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADSYMBOL]    = "Bad symbol parameter",
  [BS3_ASM_PASS1_PARSE_ERR_KEYWORD]      = "Keyword used in parameter"
};

struct bs3_asm_line bs3_asm[65536];

int bs3_asm_pass1_oneline(struct bs3_asm_line * bs3line, WORD linenum, const char * oneLine)
{
  int idxLine = 0;
  int state = -1 ; 
  int i; /* general index for iteration */
  int j; /* general index for iteration */
  int k; /* general index for iteration */
  char c; /* a character in the asm line */
  char isok= 1; /* 1 parsing is ok, 0 there is an error of parsing */
  char err=BS3_ASM_PASS1_PARSE_ERR_OK; /* by default there is no error */
  /* initialise the asm line result structure */
  bs3line->linenum = linenum;  /* init: set the current line in source code         */
  bs3line->line[0] = 0;        /* init: the asm line in structure is empty          */
  bs3line->label = -1;         /* init: no label defined in the parsed asm line     */
  bs3line->ope = -1;           /* init: no operation defined in the parsed asm line */
  bs3line->nbParam = 0;        /* init: no parameter in the parsed asm line         */
  int eol; /* 1 end of line , 0 not end of line */
  /* get the asm line */
  while (oneLine[idxLine] && idxLine < 72)
  {
    bs3line->line[idxLine] = oneLine[idxLine];
    idxLine++;
  }
  bs3line->line[idxLine++] = '\n';
  bs3line->line[idxLine] = '0';
  0; /* ASCIIZ null terminate string */
  /* check asm line is not too long */
  if (idxLine >= 72 && oneLine[idxLine]) 
  {
     bs3line->column = 72;
    return BS3_ASM_PASS1_PARSE_ERR_LINETOOLONG;
  }
  
  /* parse the asm line */
  idxLine = 0;
  eol = 0;
  while (c = oneLine[idxLine] && isok)
  {
      if (c == '\n' || c== '\r') 
      {
        bs3line->line[idxLine] = 0;
        break;
      }
      switch(state)
      {
        case BS3_ASM_PASS1_PARSE_STATE_START: /* start parsing */
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              eol = 1;
              bs3line->line[idxLine] = 0;
            case ' ':
            case '\t':
              state = BS3_ASM_PASS1_PARSE_STATE_ALABEL;
              break;
            case ':':
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_EMPTYLABEL;
              break;
            case 'a' ... 'z':
              bs3line->line[idxLine] -= 32; /* to upper case label */ 
            case 'A' ... 'Z':
            case '_':
            case '.':
              state = BS3_ASM_PASS1_PARSE_STATE_LABEL;
              bs3line->label = (SBYTE)idxLine;
              idxLine++;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;
              break;
          }
          break;
        case BS3_ASM_PASS1_PARSE_STATE_LABEL: /* label parsing */
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              eol = 1; /* end of label by end of line detection */
            case ' ':
            case '\t':
            case ':':
              /* check if we have at least one character if it is a local label */
              if (bs3lie->line[bs3lie->label] == '.' && bs3lie->label == idxLine-1) 
              {
                isok = 0;
                err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;
                break;
              }
              /* correct end of label */
              bs3line->line[idxLine] = 0;
              state = BS3_ASM_PASS1_PARSE_STATE_ALABEL;
              idxLine++;
              break;
            case 'a' ... 'z':
               bs3line->line[idxLine] -= 32; /* to upper case label */ 
            case 'A' ... 'Z':
            case '0' ... '9':
            case '_':
              idxLine++;
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;
              break;
          }
          break;
        case BS3_ASM_PASS1_PARSE_STATE_ALABEL: /* search start of instruction parsing */
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              eol = 1;
            case ' ':
            case '\t':
              idxLine++;
              break;
            case 'a' ... 'z':
              bs3line->line[idxLine] -= 32; /* to upper case operator */ 
            case 'A' ... 'Z':
            case '_':
              bs3line->ope = (SBYTE)idxLine;
              idxLine++;
              state = BS3_ASM_PASS1_PARSE_STATE_OPE;
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADOPE;
              break;
          } 
          break;
        case BS3_ASM_PASS1_PARSE_STATE_OPE: /* parse operator */
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              eol = 1;
            case ' ':
            case '\t':
              bs3line->line[idxLine] = 0;
              idxLine++;
              state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
              break;
            case 'a' ... 'z':
              bs3line->line[idxLine] -= 32; /* to upper case operator */
            case 'A' ... 'Z':
            case '0' ... '9':
            case '_':
              idxLine++;
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADOPE;
              break;
          }
          break;
        case BS3_ASM_PASS1_PARSE_STATE_AOPE: /* searching for parameter */
          state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
          break;
        case BS3_ASM_PASS1_PARSE_STATE_PARAM:
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              eol = 1;
            case ',':
              bs3line->line[idxLine] = 0;
              idxLine++;
              state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
            case 'a' ... 'z':
              bs3line->line[idxLine] -= 32; /* to upper case parameter that is a symbol */
            case 'A' ... 'Z':
            case '0' ... '9':
            case ' ': /* legal in [...] parameter */
            case '\t': /* legal in [...] parameter */
            case '_':
            case '+':
            case '-':
            case ']':
              idxLine++;
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADPARAM;
          }
          break;
        case BS3_ASM_PASS1_PARSE_STATE_PARAM_STRING: 
          switch (c)
          {
            case '\n':
            case '\r':
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADSTRING;
              break;
            case '"':
              idxLine++;
              c = oneLine[idxLine];
              switch (c)
              {
                case '\r':
                case '\n':
                case ' ':
                case '\t':
                case ',':
                  bs3line->line[idxLine] = 0;
                  idxLine++;
                  state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
                  break;
                case ';':
                  bs3line->line[idxLine] = 0;
                  state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
                  break;
                default:
                  isok = 0;
                  err = BS3_ASM_PASS1_PARSE_ERR_BADSTRING;
              }
              break;
            default:
              idxLine++;
          }  
          break;
        case BS3_ASM_PASS1_PARSE_STATE_PARAM_CHAR:
          switch (c)
          {
            case '\n':
            case '\r':
            case '\'':
            case ';':
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADCHAR;
              break;
            default:
              idxLine++;
              c = oneLine[idxLine];
              switch (c)
              {
                case '\'':
                  idxLine++;
                  c = oneLine[idxLine]:
                  switch (c)
                  {
                    case '\n':
                    case '\r':
                    case ' ':
                    case ',':
                    case '\t':
                      bs3line->line[idxLine] = 0;
                      idxLine++;
                      state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
                      break;
                    case ';':
                      bs3line->line[idxLine] = 0;
                      state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
                    default:
                      isok = 0;
                      err = BS3_ASM_PASS1_PARSE_ERR_BADPARAM;
                  }
                  break;
                default:
                  isok = 0;
                  err = BS3_ASM_PASS1_PARSE_ERR_BADCHAR;
              }
              break;              
          } 
          break;
        case BS3_ASM_PASS1_PARSE_STATE_PARAM_NUMBER:
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              bs3line->line[idxLine] = 0;
              eol = 1;
              break;
            case ' ':
            case '\t':
            case ',':
              bs3line->line[idxLine] = 0;
              idxLine++;
              state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
              break;
            case '0' ... '9':
              idxLine++;
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADNUMBER;
              break;
          } 
          break;
        case BS3_ASM_PASS1_PARSE_STATE_PARAM_HEXA:
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              bs3line->line[idxLine] = 0;
              eol = 1;
              break;
            case ' ':
            case '\t':
            case ',':
              bs3line->line[idxLine] = 0;
              idxLine++;
              state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
              break;
            case 'a' ... 'f':
              bs3line->line[idxLine] -= 32;
            case 'A' ... 'F':
            case '0' ... '9':
              idxLine++;
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADHEXA;
              break;
          } 
          break;
        case BS3_ASM_PASS1_PARSE_STATE_APARAM:
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              eol = 1;
              break;
            case ' ':
            case '\t':
              idxLine++;
              break;
            case ',':
              if (bs3line->nbParam == 0)
              {
                isok = 0;
                state = BS3_ASM_PASS1_PARSE_ERR_BADPARAM;
              }
              else
              {
                idxLine++;
              }
              break;
            case 'a' ... 'z':
              bs3line->line[idxLine] -= 32; /* to upper case parameter that is a symbol */
            case 'A' ... 'Z':
            case '.':
            case '_':
            case '+':
            case '-':
            case '[':
            case '0' ... '9':
            case '"':
            case '\'':
            case '$':
              bs3line->nbParam++;
              bs3line->param[bs3line->nbParam-1] = (SBYTE)idxLine;
              bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_UNKNOWN;
              idxLine++;
              switch (c)
              {
                case '"':
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM_STRING;
                  bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_STRING;
                  break;
                case '\'':
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM_CHAR;
                  bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_CHAR;
                  break;
                case '+':
                case '-':
                case '0' ... '9':
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM_NUMBER;
                  bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_DECIMAL;
                  break;
                case '$':
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM_HEXA;
                  bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_HEXA;
                default:
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM
                  break;
              }
              break;
            default:
              isok = 0;
              err =  BS3_ASM_PASS1_PARSE_ERR_BADPARAM;
          } 
          break;
      }
      if (eol & isok) break;
  } /*  end loop of each character in the asm line */
  if (isok && (state ==  BS3_ASM_PASS1_PARSE_STATE_START || (bs3line->ope == -1 && bs3line->label == -1) ) ) return BS3_ASM_PASS1_PARSE_ERR_NOPE;
  
  /*Parameter post treatment*/
  for (i =0; i< bs3line->nbParam && isok; i++) /* for each param */
  {
   /* for unknown param type ... */
   if (bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_UNKNOWN)
   {
     /* remove 'space' */
     j = bs3line->param[i];
     while (c = bs3line->line[j]) {
       switch (c)
       {
         case ' ':
         case '\t':
           k = j;
           while (c = bs3line->line[k])
           {
             bs3line->line[k] = bs3line->line[k+1];
             k++;
           }
       } /* end switch */
       j++;
     } /* end while */
     j = bs3line->param[i];
     c = bs3line->line[j];
     /* does the param is a B register */
     if (bs3line->line[j] == 'B' && bs3line->line[j+1] >= '0' && bs3line->line[j+1] <= '7' && bs3line->line[j+2] == 0)
     {
       bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_REGISTER_B;
       continue; /* continue with the next param */
     }
     /* does the param is a W register */
     if (bs3line->line[j] == 'W' && bs3line->line[j+1] >= '0' && bs3line->line[j+1] <= '3' && bs3line->line[j+2] == 0)
     {
       bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_REGISTER_W;
       continue; /* continue withe the next param */
     }
     /* does the param is SP register */
     if (bs3line->line[j] == 'S' && bs3line->line[j+1] == 'P' && bs3line->line[j+2] == 0)
     {
       bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_REGISTER_SP;
       continue; /* continue withe the next param */
     }
     /* check incorrect register B or W , for instance B8 to B9, or W4 to W9, but accept B80 for instance (single digit register)*/
     if (bs3line->line[j] == 'B' && bs3line->line[j+1] >= '8' && bs3line->line[j+1] <= '9' && bs3line->line[j+2] == 0)
     {
       isok = 0;
       err = BS3_ASM_PASS1_PARSE_ERR_BADREGISTER;
       idxLine = j;
       continue;
     }
     if (bs3line->line[j] == 'W' && bs3line->line[j+1] >= '4' && bs3line->line[j+1] <= '9' && bs3line->line[j+2] == 0)
     {
       isok = 0;
       err = BS3_ASM_PASS1_PARSE_ERR_BADREGISTER;
       idxLine = j;
       continue;
     }
     
     /* look like a symbol or reserved keyword */
     if ((bs3line->line[j] >= 'A' && bs3line->line[j] <= 'W') || (bs3line->line[j] == '_') || (bs3line->line[j] == '.'))
     {
       j++
       while ( c = bs3line->line[j] )
       {
         switch (c)
         {
           case 'A' ... 'Z':
           case '0' ... '9':
           case '_':
             break;
           default:
             isok = 0;
             err = BS3_ASM_PASS1_PARSE_ERR_BADSYMBOL;
             idxLine = j;
         }
         if (!isok) break;
         j++;
       }
       if (!isok) continue;
       j = bs3line->param[i];
       c = bs3line->line[j];
       /* check if not a reserved word full/human instruction */
       for (k = 0 ; k < 256; k++)
       {
         if ( strcmp( bs3Instr[k].fullName, &bs3line->line[j] ) == 0 || strcmp( bs3Instr[k].name, &bs3line->line[j] ) == 0 )
         {
           isok = 0;
           err = BS3_ASM_PASS1_PARSE_ERR_KEYWORD;
           idxLine = j;
           break;
         } 
       }
       if (!isok) continue;
       BS3_ASM_PARAM_TYPE_SYMBOL;
       bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_SYMBOL;
       continue; /* continue withe the next param */

     }
     /* look like a memory mode access */
     
     
     
   } /* end of if unknown param type */
   
  } /* end of for each param */
  
  
  if (!isok) 
  {
    bs3line->column  = (SBYTE)idxLine;
    idxLine = 0;
    while (oneLine[idxLine] && idxLine < 72)
    {
      bs3line->line[idxLine] = oneLine[idxLine];
      idxLine++;
    }
    bs3line->line[idxLine]=0;
    if (err == BS3_ASM_PASS1_PARSE_ERR_OK) return BS3_ASM_PASS1_PARSE_ERR_FAIL;
    return err;
  }
  
  
  
  return BS3_ASM_PASS1_PARSE_ERR_OK;
  
} 

void main() {
 
}
