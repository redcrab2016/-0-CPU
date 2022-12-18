#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "bs3.h"

const char * bs3_asm_message[]=
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
  [BS3_ASM_PASS1_PARSE_ERR_ADDRMODE]     = "Addressing mode error",
  [BS3_ASM_PASS1_PARSE_ERR_BIGVALUE]     = "Value too big (> 65535) or too small (< -32768)",
  [BS3_ASM_PASS1_PARSE_ERR_NOALIAS]      = "Missing alias identification"
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
    i= 1;
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
            ( symbol[0] == '6' && symbol[1] == '5' && symbol[2] == '5' && symbol[3] == '3' && symbol[4] < '6')
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
            ( symbol[0] == '3' && symbol[1] == '2' && symbol[2] == '7' && symbol[3] == '6' && symbol[4] < '8' && symbol[0] == '+') ||
            ( symbol[0] == '3' && symbol[1] == '2' && symbol[2] == '7' && symbol[3] == '6' && symbol[4] < '9' && symbol[0] == '-')
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
    for (l = 0 ; l <= BS3_INSTR_LAST; l++)
    {
      if ( strcmp( bs3Instr[l].fullName,temp ) == 0 )
      {
        if (pvalue) *pvalue = l;
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


int bs3_asm_pass1_oneline(struct bs3_asm_line * bs3line, WORD linenum, const char * oneLine)
{
  int idxLine = 0;
  int state = BS3_ASM_PASS1_PARSE_STATE_START; 
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
              if (bs3line->line[bs3line->label] == '.' && bs3line->label == idxLine-1) 
              {
                isok = 0;
                err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;
                break;
              }
              /* correct end of label */
              bs3line->line[idxLine] = 0;
              bs3line->labelIsAlias = 0 ; /* by default the label is not an alias */
              /* checks */
              symboltype = bs3_asm_pass1_symboltype(&bs3line->line[bs3line->label], 0, &value);
              switch (symboltype)
              {
                case BS3_ASM_SYMBOLTYPE_SYMBOL:
                case BS3_ASM_SYMBOLTYPE_LABEL:
                  break;
                default:
                  isok = 0;
                  idxLine = bs3line->label;
                  err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;                
              }
              if (!isok) break;
              /* checks done and ok then carry on operation */
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
              /* checks */
              symboltype = bs3_asm_pass1_symboltype(&bs3line->line[bs3line->ope], 0, &value);
              switch (symboltype)
              {
                case BS3_ASM_SYMBOLTYPE_KEYWORD:
                  /* TODO , human or full ope name, indicate ope quality  */
                  if (value == -1) 
                  {
                    bs3line->opeType = BS3_ASM_OPETYPE_HUMAN;
                    bs3line->opeCode = 0;
                  }
                  else
                  {
                    if (value < 256 && value >= 0)
                    {
                      bs3line->opeType = BS3_ASM_OPETYPE_FULL;
                      bs3line->opeCode = (BYTE)value;
                    }
                    else
                    {
                      bs3line->opeType = bs3Instr[value].opeType;
                      bs3line->opeCode = value; 
                      if (bs3line->opeType == BS3_ASM_OPETYPE_ALIAS )
                      {
                        if (bs3line->label == -1) 
                        {
                          isok = 0;
                          idxLine = 0;
                          err = BS3_ASM_PASS1_PARSE_ERR_NOALIAS;
                        }
                        else
                        {
                          bs3line->labelIsAlias = 1;
                        }
                      }
                    } 
                  }
                  break;
                case BS3_ASM_SYMBOLTYPE_SYMBOL:
                  bs3line->opeType = BS3_ASM_OPETYPE_SYMBOL;
                  bs3line->opeCode = 0;
                  break;
                default:
                  isok = 0;
                  idxLine = bs3line->ope;
                  err = BS3_ASM_PASS1_PARSE_ERR_BADOPE;
              }
              if (!isok) break;
              /* checks are ok then carry on possible parameter */
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
                case ';':
                  bs3line->line[idxLine] = 0;
                  state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
                  if (c != ';') idxLine++;
                  break;
                default:
                  isok = 0;
                  err = BS3_ASM_PASS1_PARSE_ERR_BADSTRING;
              }
              break;
            case 0 ... 9:
            case 11 ... 12:
            case 14 ... 31:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADSTRING;
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
              value = (BYTE)c;
              idxLine++;
              c = oneLine[idxLine];
              switch (c)
              {
                case '\'':
                  idxLine++;
                  c = oneLine[idxLine];
                  switch (c)
                  {
                    case '\n':
                    case '\r':
                    case ' ':
                    case ',':
                    case '\t':
                    case ';':
                      bs3line->line[idxLine] = 0;
                      state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
                      bs3line->paramValue[bs3line->nbParam-1] = value;
                      if (c != ';' ) idxLine++;
                      break;
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
              eol = 1;
            case ' ':
            case '\t':
            case ',':
              bs3line->line[idxLine] = 0;
              value = (bs3line->line[  bs3line->param[bs3line->nbParam-1]  ] == '-') ? -value : value;
              if (value >= 0 && value < 256) bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_BYTE_DECIMAL;
              if (value >= 256 && value < 65536 ) bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_WORD_DECIMAL;
              if (value < 0 && value >= -128)  bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_SBYTE_DECIMAL;
              if (value < -128 && value >= -32768) bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_SWORD_DECIMAL;
              bs3line->paramValue[bs3line->nbParam-1] = value;
              if (eol) break;
              idxLine++;
              state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
              break;
            case '0' ... '9':
              value = value * 10 + ((BYTE)c) - '0';
              if ( value < -32768 || value > 65535) 
              {
                isok = 0;
                err = BS3_ASM_PASS1_PARSE_ERR_BIGVALUE;
                break;
              }
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
              eol = 1;
            case ' ':
            case '\t':
            case ',':
              bs3line->line[idxLine] = 0;
              bs3line->paramValue[bs3line->nbParam-1] = value;
              j = strlen(&bs3line->line[  bs3line->param[bs3line->nbParam-1]  ]);
              bs3line->paramType[bs3line->nbParam-1] = (j<=3) ? BS3_ASM_PARAM_TYPE_BYTE_HEXA : BS3_ASM_PARAM_TYPE_WORD_HEXA;
              if (eol) break;
              idxLine++;
              state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
              break;
            case 'a' ... 'f':
              bs3line->line[idxLine] -= 32;
              c -= 32;
            case 'A' ... 'F':
            case '0' ... '9':
              value = (c <= '9') ? (value * 10 + (((BYTE)c) -'0')) :  (value * 10 + (((BYTE)c) -'A' + 10 ));
              if ( value > 65535) 
              {
                isok = 0;
                err = BS3_ASM_PASS1_PARSE_ERR_BIGVALUE;
                break;
              }
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
                  bs3line->paramType[bs3line->nbParam-1] = BS3_ASM_PARAM_TYPE_BYTE_CHAR;
                  value = 0;
                  break;
                case '+':
                case '-':
                case '0' ... '9':
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM_NUMBER;
                  value = (c >= '0')?((BYTE)c) - '0':0;
                  break;
                case '$':
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM_HEXA;
                  value = 0;
                default:
                  state = BS3_ASM_PASS1_PARSE_STATE_PARAM;
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

 
     symboltype =  bs3_asm_pass1_symboltype(&bs3line->line[j], 0, &value);
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
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M6SYMBOL; /* only alias */
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
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M7SYMBOL; /* only alias */
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
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M4SYMBOL; /* only alias */
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
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M2SYMBOL; /* only alias */
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
             bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M0SYMBOL; /* can be alias or label */
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
  
  /* validate instruction*/
  if (isok && bs3line->ope != -1)
  {
    /* TODO : need more data in 'struct bs3_cpu_instr' */
  }
  
  if (!isok) /* something wrong ... */
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
  /* parsing done and ok */
  return BS3_ASM_PASS1_PARSE_ERR_OK;
  
} 

