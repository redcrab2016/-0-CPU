#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "bs3.h"

#define BS3_ASM_LINE_SIZE  72
#define BS3_ASM_LINE_BUFFER  (BS3_ASM_LINE_SIZE + 2)

/* param types : mask 0xF0 for type, mask 0x0F for sub type */
#define BS3_ASM_PARAM_TYPE_UNKNOWN       0x00
#define BS3_ASM_PARAM_TYPE_DECIMAL       0x10
#define BS3_ASM_PARAM_TYPE_HEXA          0x20
#define BS3_ASM_PARAM_TYPE_CHAR          0x30
#define BS3_ASM_PARAM_TYPE_SYMBOL        0x40
#define BS3_ASM_PARAM_TYPE_STRING        0x50
#define BS3_ASM_PARAM_TYPE_REGISTER      0x60
#define BS3_ASM_PARAM_TYPE_REGISTER_B    0x61
#define BS3_ASM_PARAM_TYPE_REGISTER_W    0x62
#define BS3_ASM_PARAM_TYPE_REGISTER_SP   0x63
#define BS3_ASM_PARAM_TYPE_M0            0x70
#define BS3_ASM_PARAM_TYPE_M0DECIMAL     0x71
#define BS3_ASM_PARAM_TYPE_M0HEXA        0x72
#define BS3_ASM_PARAM_TYPE_M0CHAR        0x73
#define BS3_ASM_PARAM_TYPE_M0SYMBOL      0X74
#define BS3_ASM_PARAM_TYPE_M1            0x80
#define BS3_ASM_PARAM_TYPE_M2            0x80
#define BS3_ASM_PARAM_TYPE_M2DECIMAL     0x81
#define BS3_ASM_PARAM_TYPE_M2HEXA        0x82
#define BS3_ASM_PARAM_TYPE_M2CHAR        0x83
#define BS3_ASM_PARAM_TYPE_M2SYMBOL      0x84
#define BS3_ASM_PARAM_TYPE_M3            0x90
#define BS3_ASM_PARAM_TYPE_M4            0xA0
#define BS3_ASM_PARAM_TYPE_M4DECIMAL     0xA1
#define BS3_ASM_PARAM_TYPE_M4HEXA        0xA2
#define BS3_ASM_PARAM_TYPE_M4CHAR        0xA3
#define BS3_ASM_PARAM_TYPE_M4SYMBOL      0xA4
#define BS3_ASM_PARAM_TYPE_M5            0xB0
#define BS3_ASM_PARAM_TYPE_M6            0xC0
#define BS3_ASM_PARAM_TYPE_M6DECIMAL     0xC1
#define BS3_ASM_PARAM_TYPE_M6HEXA        0xC2
#define BS3_ASM_PARAM_TYPE_M6CHAR        0xC3
#define BS3_ASM_PARAM_TYPE_M6SYMBOL      0xC4
#define BS3_ASM_PARAM_TYPE_M7            0xD0
#define BS3_ASM_PARAM_TYPE_M7DECIMAL     0xD1
#define BS3_ASM_PARAM_TYPE_M7HEXA        0xD2
#define BS3_ASM_PARAM_TYPE_M7CHAR        0xD3
#define BS3_ASM_PARAM_TYPE_M7SYMBOL      0xD4
#define BS3_ASM_PARAM_TYPE_LABEL         0xE0


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
#define BS3_ASM_PASS1_PARSE_ERR_ADDRMODE     14

/* Symbol type */
#define BS3_ASM_SYMBOLTYPE_UNKNOWN       0x00
#define BS3_ASM_SYMBOLTYPE_DECIMAL       0x10
#define BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE  0x10
#define BS3_ASM_SYMBOLTYPE_DECIMAL_WORD  0x11
#define BS3_ASM_SYMBOLTYPE_DECIMAL_SBYTE 0x18
#define BS3_ASM_SYMBOLTYPE_DECIMAL_SWORD 0x19
#define BS3_ASM_SYMBOLTYPE_DECIMAL_BIG   0x1F
#define BS3_ASM_SYMBOLTYPE_HEXA          0x20
#define BS3_ASM_SYMBOLTYPE_HEXA_BYTE     0x20
#define BS3_ASM_SYMBOLTYPE_HEXA_WORD     0x21
#define BS3_ASM_SYMBOLTYPE_HEXA_BIG      0x2F
#define BS3_ASM_SYMBOLTYPE_CHAR          0x30
#define BS3_ASM_SYMBOLTYPE_REGISTER      0x40
#define BS3_ASM_SYMBOLTYPE_REGISTER_BYTE 0x40
#define BS3_ASM_SYMBOLTYPE_REGISTER_WORD 0x41
#define BS3_ASM_SYMBOLTYPE_REGISTER_SP   0x42
#define BS3_ASM_SYMBOLTYPE_KEYWORD       0x50
#define BS3_ASM_SYMBOLTYPE_SYMBOL        0x60
#define BS3_ASM_SYMBOLTYPE_LABEL         0x70

struct bs3_asm_line
{
  WORD linenum;
  char line[BS3_ASM_LINE_BUFFER];
  union {
    SBYTE label;
    SBYTE column; /* if parsing error, then it indicates the character index in the line where the error has been detected */
  }
  SBYTE ope;
  BYTE nbParam;
  SBYTE param[40]; /* store the index inside line[] */
  BYTE paramType[40];
  long paramValue; /* meaning depends of paramType, BS3_ASM_PARAM_TYPE...DECIMAL/HEXA/CHAR the value , SYMBOL/LABEL the index in the param... */
};

static const char * bs3_asm_message[]=
{
  [BS3_ASM_PASS1_PARSE_ERR_OK]           = "Parse succeeded",
  [BS3_ASM_PASS1_PARSE_ERR_NOPE]         = "Nothing to parse",
  [BS3_ASM_PASS1_PARSE_ERR_FAIL]         = "General error",
  [BS3_ASM_PASS1_PARSE_ERR_LINETOOLONG]  = "Line too long (max 72 characters)",
  [BS3_ASM_PASS1_PARSE_ERR_EMPTYLABEL]   = "Empty label",
  [BS3_ASM_PASS1_PARSE_ERR_BADLABEL]     = "Bad label",
  [BS3_ASM_PASS1_PARSE_ERR_BADOPE]       = "Bad operation",
  [BS3_ASM_PASS1_PARSE_ERR_BADPARAM]     = "Bad parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADSTRING]    = "Bad string parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADCHAR]      = "Bad one character parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADNUMBER]    = "Bad decimal parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADREGISTER]  = "Bad register parameter",
  [BS3_ASM_PASS1_PARSE_ERR_BADSYMBOL]    = "Bad symbol parameter",
  [BS3_ASM_PASS1_PARSE_ERR_KEYWORD]      = "Keyword used in parameter",
  [BS3_ASM_PASS1_PARSE_ERR_ADDRMODE]     = "Addressing mode error"
};

struct bs3_asm_line bs3_asm[65536];

int bs3_asm_pass1_symboltype(const char * symbol, int length, long * pvalue)
{
  int i;
  int l;
  long value = 0;
  char temp[BS3_ASM_LINE_BUFFER];
  if (symbol == 0) return BS3_ASM_SYMBOLTYPE_UNKNOWN;
  if (length <= 0) {length = 0; while (symbol[length]) length++; }
  if (length == 0) return  BS3_ASM_SYMBOLTYPE_UNKNOWN;
  
  if (symbol[0] == '$') /* possible hexadecimal value */
  {
    l = length - 1;
    i= 1;
    for (i = 1; i <= l; i++)
    {
      switch (symbol[i])
      {
        case '0' ... '9': 
          value = (value << 4) | (symbol[i] - '0');
          break;
        case 'a' ... 'f':
          value = (value << 4) | (symbol[i] - 'a' + 10);
          break;
        case 'A' ... 'F':
          value = (value << 4) | (symbol[i] - 'A' + 10);
          break;
        default:
          return BS3_ASM_SYMBOLTYPE_UNKNOWN;
      }
    }
    if (pvalue) *pvalue = value;
    if (length <= 3) return BS3_ASM_SYMBOLTYPE_HEXA_BYTE;
    if (length <= 5) return BS3_ASM_SYMBOLTYPE_HEXA_WORD;
    return BS3_ASM_SYMBOLTYPE_HEXA_BIG;
  }
  
  if (symbol[0] == '\'' ) /* possible character */
  {
    if (length != 3) return BS3_ASM_SYMBOLTYPE_UNKNOWN;
    if (symbol[2] != '\'') return BS3_ASM_SYMBOLTYPE_UNKNOWN;
    if (symbol[1] <  ' ')  return BS3_ASM_SYMBOLTYPE_UNKNOWN;
    if (pvalue) *pvalue = symbol[1];
    return BS3_ASM_SYMBOLTYPE_CHAR;
  }
  
  if (symbol[0] >= '0' && symbol[0] <= '9') /* possible decimal value */
  {
    l = length - 1;
    i= 1
    for (i = 0; i <= l; i++)
    {
      switch (symbol[i])
      {
        case '0' ... '9':
          value = (value * 10) + (symbol[i] - '0');
          break;
        default:
          return BS3_ASM_SYMBOLTYPE_UNKNOWN;
      }
    }
    if (pvalue) *pvalue = value;
    if (length <= 2) return BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE;
    if (length == 3 &&
           ( symbol[0] < '2' ||
             (symbol[0] == '2' && symbol[1] < '5') ||
             (symbol[0] == '2' && symbol[1] == '5' && symbol[2] < '6')
           )
        ) return BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE;
    if (length == 4) return BS3_ASM_SYMBOLTYPE_DECIMAL_WORD;
    if (length == 5 &&
          (  
            symbol[0] < '6' ||
            ( symbol[0] == '6' && symbol[1] < '5') ||
            ( symbol[0] == '6' && symbol[1] == '5' && symbol[2] < '5') ||
            ( symbol[0] == '6' && symbol[1] == '5' && symbol[2] == '5' && symbol[3] < '3') ||
            ( symbol[0] == '6' && symbol[1] == '5' && symbol[2] == '5' && symbol[3] = '3' && symbol[4] < '6')
          )
        ) return BS3_ASM_SYMBOLTYPE_DECIMAL_WORD;
    return BS3_ASM_SYMBOLTYPE_DECIMAL_BIG;
  }
  
  if (symbol[0] == '+' || symbol[0] == '-') /* possible signed decimal value */
  {
    l = length - 1;
    i= 1;
    for (i = 1; i <= l; i++)
    {
      switch (symbol[i])
      {
        case '0' ... '9':
          value = value * 10 + (symbol[i] - '0');
          break;
        default:
          return BS3_ASM_SYMBOLTYPE_UNKNOWN;
      }
    }
    if (pvalue) *pvalue = symbol[0] == '-' ? -value : value;
    if (length <= 4) return BS3_ASM_SYMBOLTYPE_DECIMAL_SBYTE;
    if (length == 4 &&
           (
              symbol[1] <  '1' ||
             (symbol[1] == '1' && symbol[2] < '2') ||
             (symbol[1] == '1' && symbol[2] == '2' && symbol[3] < '8' && symbol[0] == '+') ||
             (symbol[1] == '1' && symbol[2] == '2' && symbol[3] < '9' && symbol[0] == '-') 
           )
        ) return BS3_ASM_SYMBOLTYPE_DECIMAL_SBYTE;
    if (length == 5) return BS3_ASM_SYMBOLTYPE_DECIMAL_SWORD;
    if (length == 6 &&
          (  
              symbol[0] <  '3' ||
            ( symbol[0] == '3' && symbol[1] < '2') ||
            ( symbol[0] == '3' && symbol[1] == '2' && symbol[2] < '7') ||
            ( symbol[0] == '3' && symbol[1] == '2' && symbol[2] == '7' && symbol[3] < '6') ||
            ( symbol[0] == '3' && symbol[1] == '2' && symbol[2] == '7' && symbol[3] = '6' && symbol[4] < '8' && symbol[0] == '+') ||
            ( symbol[0] == '3' && symbol[1] == '2' && symbol[2] == '7' && symbol[3] = '6' && symbol[4] < '9' && symbol[0] == '-')
          )
        ) return BS3_ASM_SYMBOLTYPE_DECIMAL_SWORD;
    return BS3_ASM_SYMBOLTYPE_DECIMAL_BIG;
  }
  
  if (length == 2) /* possible register */
  {
    if (symbol[0] == 'W' && symbol[1] >= '0' && symbol[1] <= '3')
    {
      if (pvalue) *pvalue = symbol[1] - '0';
      return BS3_ASM_SYMBOLTYPE_REGISTER_WORD;
    }
    if (symbol[0] == 'B' && symbol[1] >= '0' && symbol[1] <= '7')
    {
      if (pvalue) *pvalue = symbol[1] - '0';
      return BS3_ASM_SYMBOLTYPE_REGISTER_BYTE;
    }
    if (symbol[0] == 'S' && symbol[1] == 'P')
    {
      return BS3_ASM_SYMBOLTYPE_REGISTER_SP;
    }
  }
  
  if ((symbol[0] >= 'A' && symbol[0] <= 'Z') || symbol[0] == '.' || symbol[0] == '_') /* possible symbol, label, keyword */
  {
    for (i = 1 ; i < length; i++) /* for each character in Symbol, verify it does comply */
    {
      switch (symbol[i])
      {
        case 'A' ... 'Z':
        case '0' ... '9':
        case '_':
          break;
        default:
          return BS3_ASM_SYMBOLTYPE_UNKNOWN;
      }
    } /* end for each character in Symbol*/
    
    if (symbol[0] == '.') return BS3_ASM_SYMBOLTYPE_LABEL;
    
    /* check if it is a keyword */
    for (i = 0 ; i < length; i++) temp[i] = symbol[i];
    temp[i] = 0;
    for (l = 0 ; l < 256; l++)
    {
      if ( strcmp( bs3Instr[l].fullName,temp ) == 0 )
      {
        if (pvalue && ( symbol[0] != 'D' || symbol[1] != 'B' || length != 2 )) *pvalue = l;
        return BS3_ASM_SYMBOLTYPE_KEYWORD;
      }
      if (strcmp( bs3Instr[l].name, temp ) == 0 )
      {
        if (pvalue) *pvalue = -1;
        return BS3_ASM_SYMBOLTYPE_KEYWORD;
      } 
    }
    return BS3_ASM_SYMBOLTYPE_SYMBOL; 
  } /* end if possible symbol, label, keyword */ 
  
  return BS3_ASM_SYMBOLTYPE_UNKNOWN;
}

int bs3_asm_pass1_symboltype(const char * symbol)
{
 return bs3_asm_pass1_symboltype(symbol, 0, 0);
}

int bs3_asm_pass1_symboltype(const char * symbol, int length)
{
 return bs3_asm_pass1_symboltype(symbol, length, 0);
}

int bs3_asm_pass1_symboltype(const char * symbol, long * value)
{
 return bs3_asm_pass1_symboltype(symbol, 0, value);
}


int bs3_asm_pass1_oneline(struct bs3_asm_line * bs3line, WORD linenum, const char * oneLine)
{
  int idxLine = 0;
  int state = -1 ; 
  int symboltype;
  long value;
  int i; /* general index for iteration */
  int j; /* general index for iteration */
  int k; /* general index for iteration */
  int l; /* general index for iteration or length of string */
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
  while (oneLine[idxLine] && idxLine < BS3_ASM_LINE_SIZE)
  {
    bs3line->line[idxLine] = oneLine[idxLine];
    idxLine++;
  }
  bs3line->line[idxLine++] = '\n';
  bs3line->line[idxLine] = '0'; /* ASCIIZ null terminate string */
  /* check asm line is not too long */
  if (idxLine >= BS3_ASM_LINE_SIZE && oneLine[idxLine]) 
  {
     bs3line->column = BS3_ASM_LINE_SIZE;
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
            case '\'':
            case '$':
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
   /* for decimal,hexa,char param type, determine its value*/
   if (bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_DECIMAL ||
       bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_HEXA    ||
       bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_CHAR)
   {
     j = bs3line->param[i];
     c = bs3line->line[j];
     symboltype = bs3_asm_pass1_symboltype(&bs3line->line[j], &value);
     /* no check on symbol type because we trust paramType[i] */
     bs3line->paramValue[i] = value;
     continue; /* continue with next param */
   }

   /* for symbol param type, determine if it is not a label in fact (local label : start with dot '.') */
   if (bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_SYMBOL)
   {
     j = bs3line->param[i];
     c = bs3line->line[j];
     if (c == '.')  bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_LABEL;
     continue; /* continue with next param */
   }
   /* for unknown param type ... */
   if (bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_UNKNOWN)
   {
     /* remove 'space' (' ' and '\t') : condensate the parameter : example if "B 0" then it will be seen as "B0" then Byte register zero */
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

     /* check incorrect register B or W , for instance B8 to B9, or W4 to W9, but accept B80 for instance (multiple digit register alike)*/
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

 
     symboltype =  bs3_asm_pass1_symboltype(&bs3line->line[j], &value);
     switch (symboltype)
     {
        case BS3_ASM_SYMBOLTYPE_REGISTER_BYTE:
          bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_REGISTER_B;
          bs3line->paramValue[i] = value; /* register number between 0 and 7 */
          continue; /* continue with the next param */
        case BS3_ASM_SYMBOLTYPE_REGISTER_WORD:
          bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_REGISTER_W;
          bs3line->paramValue[i] = value; /* register number between 0 and 3 */
          continue; /* continue with the next param */
        case BS3_ASM_SYMBOLTYPE_REGISTER_SP:
          bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_REGISTER_SP;
          continue; /* continue with the next param */
        case BS3_ASM_SYMBOLTYPE_KEYWORD:
          isok = 0;
          err = BS3_ASM_PASS1_PARSE_ERR_KEYWORD;
          idxLine = j;
          continue;
        case BS3_ASM_SYMBOLTYPE_SYMBOL:
          bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_SYMBOL;
          continue; /* continue with the next param */
        case BS3_ASM_SYMBOLTYPE_LABEL:
          bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_LABEL;
          continue; /* continue with the next param */
     }
  
     
     /* look like a memory mode access */
     if ( c == '[' ) /* first character is a addressing mode */
     {
       k = j;
       while (bs3line->line[++k]);
       l = k - j; /* length of param */
       k--;
       if (bs3line->line[k] != ']') /* in addressing mode last character */
       {
         isok = 0;
         err = BS3_ASM_PASS1_PARSE_ERR_ADDRMODE;
         idxLine = j;
         continue;
       }
       if (l > 8) /* possible mode 6 or 7 */ 
       {
         if (bs3line->line[j+1] == 'S' && 
             bs3line->line[j+2] == 'P' && 
             bs3line->line[j+3] == '+' && 
             bs3line->line[j+4] == 'W' &&
             bs3line->line[j+5] >= '0' &&
             bs3line->line[j+5] <= '3' &&
             bs3line->line[j+6] >= '+')    /* possible mode 6 */
         {
           /* from j+7 on a length of l-8 */
           symboltype = bs3_asm_pass1_symboltype(&bs3line->line[j+6], l-8, &value);
           if ( symboltype == BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M6DECIMAL;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_HEXA_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M6HEXA;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_CHAR )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M6CHAR;
             bs3line->paramValue[i] = value; /* value between 0 and 255 or -128 and + 127*/
             continue; /* go to next param */           
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_SYMBOL )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M6SYMBOL;
             bs3line->paramValue[i] = j+6; /* index of symbol inside addressing mode param */
             continue; /* go to next param */           
           }
           isok = 0;
           err = BS3_ASM_PASS1_PARSE_ERR_ADDRMODE;
           idxLine = j;
           continue;
         } /* end of possible mode 6 */  
            
         if (bs3line->line[j+1] == 'W' && 
             bs3line->line[j+2] >= '0' && 
             bs3line->line[j+2] <= '3' && 
             bs3line->line[j+3] == '+' && 
             bs3line->line[j+4] == 'W' &&
             bs3line->line[j+5] >= '0' &&
             bs3line->line[j+5] <= '3' &&
             bs3line->line[j+6] >= '+')    /* possible mode 7 */
         {
           /* from j+7 on a length of l-8 */
           symboltype = bs3_asm_pass1_symboltype(&bs3line->line[j+6], l-8, &value);
           if ( symboltype == BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M7DECIMAL;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_HEXA_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M7HEXA;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_CHAR )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M7CHAR;
             bs3line->paramValue[i] = value; /* value between 0 and 255 or -128 and +127 */
             continue; /* go to next param */           
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_SYMBOL )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M7SYMBOL;
             bs3line->paramValue[i] = j+6; /* index of symbol inside addressing mode param */
             continue; /* go to next param */           
           }
           isok = 0;
           err = BS3_ASM_PASS1_PARSE_ERR_ADDRMODE;
           idxLine = j;
           continue;
         }  /* end of possible mode 7 */   
       }
       
       if (l == 7) /* possible mode 3 or 5 */
       {
         if (bs3line->line[j+1] == 'S' && 
             bs3line->line[j+2] == 'P' && 
             bs3line->line[j+3] == '+' && 
             bs3line->line[j+4] == 'W' &&
             bs3line->line[j+5] >= '0' &&
             bs3line->line[j+5] <= '3')    /* possible mode 3 */
         {
           bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M3;
           continue; /* go to next param */
         }     
         if (bs3line->line[j+1] == 'W' && 
             bs3line->line[j+2] >= '0' && 
             bs3line->line[j+2] <= '3' && 
             bs3line->line[j+3] == '+' && 
             bs3line->line[j+4] == 'W' &&
             bs3line->line[j+5] >= '0' &&
             bs3line->line[j+5] <= '3')    /* possible mode 5 */
         {
           bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M5;
           continue; /* go to next param */
         }     
       }
       
       if (l > 5) /* possible mode 2 or 4 */
       {
         if (bs3line->line[j+1] == 'S' && 
             bs3line->line[j+2] == 'P' && 
             bs3line->line[j+3] == '+')    /* possible mode 4 */
         {
           /* from j+4 on a length of l-5 */
           symboltype = bs3_asm_pass1_symboltype(&bs3line->line[j+4], l-5, &value);
           if ( symboltype == BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M4DECIMAL;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_HEXA_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M4HEXA;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_CHAR )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M4CHAR;
             bs3line->paramValue[i] = value; /* value between 0 and 255 or -128 and + 127 */
             continue; /* go to next param */           
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_SYMBOL )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M4SYMBOL;
             bs3line->paramValue[i] = j+4; /* index of symbol inside addressing mode param */
             continue; /* go to next param */           
           }
           isok = 0;
           err = BS3_ASM_PASS1_PARSE_ERR_ADDRMODE;
           idxLine = j;
           continue;
         }  /* end of possible mode 4 */
         if (bs3line->line[j+1] == 'W' && 
             bs3line->line[j+2] >= '0' && 
             bs3line->line[j+2] <= '3' && 
             bs3line->line[j+3] == '+')    /* possible mode 2 */
         {
           /* from j+4 on a length of l-5 */
           symboltype = bs3_asm_pass1_symboltype(&bs3line->line[j+4], l-5, &value);
           if ( symboltype == BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M2DECIMAL;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_HEXA_BYTE )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M2HEXA;
             bs3line->paramValue[i] = value; /* value between 0 and 255 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_CHAR )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M2CHAR;
             bs3line->paramValue[i] = value; /* value between 0 and 255 or -128 and +127 */
             continue; /* go to next param */           
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_SYMBOL )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M2SYMBOL;
             bs3line->paramValue[i] = value; /* index of symbol inside addressing mode param */
             continue; /* go to next param */           
           }
           isok = 0;
           err = BS3_ASM_PASS1_PARSE_ERR_ADDRMODE;
           idxLine = j;
           continue;
         } /* end of possible mode 2 */    
       }
       
       if (l == 4) /* possible mode 1 */
       {
         if (bs3line->line[j+1] == 'W' && 
             bs3line->line[j+2] >= '0' && 
             bs3line->line[j+2] <= '3')   
         {
           bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M1;
           continue; /* go to next param */
         }     
       }
       
       if (l > 2) /* possible mode 0 */
       {
         /* from j+1 on a length of l-2 */
          symboltype = bs3_asm_pass1_symboltype(&bs3line->line[j+1], l-2, &value);
           if ( symboltype == BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE ||
                symboltype == BS3_ASM_SYMBOLTYPE_DECIMAL_WORD)
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M0DECIMAL;
             bs3line->paramValue[i] = value; /* value between 0 and 65535 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_HEXA_BYTE ||
                symboltype == BS3_ASM_SYMBOLTYPE_HEXA_WORD)
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M0HEXA;
             bs3line->paramValue[i] = value; /* value between 0 and 65535 */
             continue; /* go to next param */
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_CHAR )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M0CHAR;
             bs3line->paramValue[i] = value; /* value between 0 and 255 or -128 and +127 */
             continue; /* go to next param */           
           }
           if ( symboltype == BS3_ASM_SYMBOLTYPE_SYMBOL )
           {
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M0SYMBOL;
             bs3line->paramValue[i] = value; /* index of symbol inside addressing mode param */
             continue; /* go to next param */           
           }
           isok = 0;
           err = BS3_ASM_PASS1_PARSE_ERR_ADDRMODE;
           idxLine = j;
           continue;
       } /* end of possible mode 0 */
       isok = 0;
       err = BS3_ASM_PASS1_PARSE_ERR_ADDRMODE;
       idxLine = j;
       continue;
     }
     /*  param still unknonwn  */
     isok = 0;
     err = BS3_ASM_PASS1_PARSE_ERR_BADPARAM;
     idxLine = j;
     continue;
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
