#include <stdio.h>
//#include <stdlib.h>
#include <string.h>
//#include <errno.h>

#include "bs3_asm.h"

/* check label in provided bs3line for duplicate, then attach local label to last encounter global label */
int bs3_asm_line_checkLabel(struct bs3_asm_line * bs3line)
{
  int err = BS3_ASM_PASS1_PARSE_ERR_OK;
  char * curLabel;
  struct bs3_asm_line bs3_asm_tmp;
  int i;
  if (bs3line->label == -1) return err;
  curLabel = &bs3line->line[bs3line->label];
  for (i = bs3line->asmIndex -1; i >=0 ; i--)
  {
    bs3_asm_line_at(i, &bs3_asm_tmp);
    if (bs3_asm_tmp.label == -1) continue;
    if (strcmp(curLabel, &bs3_asm_tmp.line[bs3_asm_tmp.label]) == 0)
    {
      err =BS3_ASM_PASS1_PARSE_ERR_DUPLABEL;
      break;
    }

    if (curLabel[0] == '.') /* the label to check is local */
    {
      if (bs3_asm_tmp.line[bs3_asm_tmp.label] != '.') /* if found a global label */
      {
        /* attached to global label */
        bs3line->asmIndexToGlobalLabel = i;
        break;
      }
    }
  }
  
  return err;
}

/*
  register label
  if local label, check if not duplicate inside its scope (attache to next above gloabl label)
                  update attachment to global label
  if global label, check if not duplicate

/* return NULL pointer if not found, otherwise the pointer to bs3_asm line  */
struct bs3_asm_line * bs3_asm_getEQU(const char * symbol, struct bs3_asm_line *  curbs3line, struct bs3_asm_line * foundbs3line)
{
  long currIndex = curbs3line->asmIndex; /*(curbs3line - bs3_asm); /* retrieve current index in bs3_asm array */
  long i;
  long value;
  struct bs3_asm_line * bs3_line_cursor;
  char symb[BS3_ASM_LINE_BUFFER];
  i = 0;
  while (symbol[i] &&
         symbol[i] != ']' &&
          (
            (symbol[i] >= 'A' && symbol[i] <= 'Z') ||
            (symbol[i] >= '0' && symbol[i] <= '9') ||
             symbol[i] == '_'
          ) &&
          i < (BS3_ASM_LINE_BUFFER-1)
         ) { symb[i] = symbol[i]; i++; } /* symbol may be embededed inside a larger character string */
  symb[i]  = 0;/* be sure to null terminate the symbol */
  for (i = currIndex -1; i >=0; i--) /* backward search */
  {
    bs3_line_cursor = bs3_asm_line_at(i, foundbs3line);
    if (bs3_line_cursor == 0) return 0;
    if (bs3_line_cursor->label != -1 &&  /* has a label */
        bs3_line_cursor->labelIsAlias && /* label is an alias */
        bs3_line_cursor->opeCode == BS3_INSTR_EQU && /* operation is a EQU alias operation */
        strcmp(symb, &bs3_line_cursor->line[bs3_line_cursor->label]) == 0) /* alias name is what we're looking for */
    {
      if (bs3_line_cursor->nbParam != 1) break; /* we expect one param only */
      switch (bs3_line_cursor->paramType[0])
      {
        case BS3_ASM_PARAM_TYPE_BYTE:
        case BS3_ASM_PARAM_TYPE_BYTE_DECIMAL:
        case BS3_ASM_PARAM_TYPE_BYTE_HEXA:
        case BS3_ASM_PARAM_TYPE_BYTE_CHAR :
        case BS3_ASM_PARAM_TYPE_SBYTE_DECIMAL:
        case BS3_ASM_PARAM_TYPE_WORD:
        case BS3_ASM_PARAM_TYPE_WORD_DECIMAL:
        case BS3_ASM_PARAM_TYPE_WORD_HEXA:
        case BS3_ASM_PARAM_TYPE_SWORD_DECIMAL:
           return bs3_line_cursor;
        case  BS3_ASM_PARAM_TYPE_SYMBOL:
          return bs3_asm_getEQU(&bs3_line_cursor->line[bs3_line_cursor->param[0]], bs3_line_cursor, bs3_line_cursor );
      }
    }
  }
  return 0;
}

void bs3_asm_report(const char * filename, int line, int linecolumn, int message)
{
  if (line > 0)
  {
    fprintf(stderr, "In %s at line %d column %d : Error %d : %s\n", filename, line, linecolumn,message, bs3_asm_message[message]);
  }
  else
  {
    fprintf(stderr, "File %s : Error %d : %s\n", filename, message, bs3_asm_message[message]);
  }
}

int bs3_asm_pass1_param_compatible(int destType, int srcType, long srcValue)
{
  int isok = 1;                /* by default it is compatible.. we search for incompatibility */
  destType = destType & 0xF0;  /* looks on type and not on subtype */ 
  srcType  = srcType  & 0xF0;  /* looks on type and not on subtype */
  if (destType != srcType)     /* different type, then source of incompatibility */
  {
    isok = 0;                  /* by default here , source and destination are not compatible */
    if  ( 
          (destType == BS3_ASM_PARAM_TYPE_WORD && srcType == BS3_ASM_PARAM_TYPE_BYTE)  || /* BYTE is compatible WORD */
          (destType == BS3_ASM_PARAM_TYPE_BYTE && srcValue >=-128 && srcValue <= 255)  || /* WORD is compatible BYTE only if value in range [-128,255] */
          (destType == BS3_ASM_PARAM_TYPE_MISC && srcType == BS3_ASM_PARAM_TYPE_BYTE)  || /* MISC is compatible with BYTE  */
          (destType == BS3_ASM_PARAM_TYPE_MISC && srcType == BS3_ASM_PARAM_TYPE_WORD)  || /* MISC is compatible with WORD  */
          (destType == BS3_ASM_PARAM_TYPE_MISC && srcType == BS3_ASM_PARAM_TYPE_LABEL)    /* MISC is compatible with LABEL */
        )                                          
      isok = 1;    
  }
  return isok;
}

int bs3_asm_pass1_instructionCheck(struct bs3_asm_line * bs3line)
{
  int err = BS3_ASM_PASS1_PARSE_ERR_OK; /* by default it is ok */
  int i;
  int j;
  int found;
  switch (bs3line->opeType)
  {
      case BS3_ASM_OPETYPE_HUMAN: /* need to precise the FULL instruction: search instruction */
        found = 0; /* by default we did not found the FULL instruction */
        for (i = 0 ; i <= BS3_INSTR_LAST; i++) /* search the FULL instruction */
        {
          if (bs3Instr[i].opeType != BS3_ASM_OPETYPE_CPU) continue; /* Human instruction concern only CPU instruction (not meta, directive or alias ) */
          if (strcmp (bs3Instr[i].name, &bs3line->line[bs3line->ope]) == 0) /* found a potential */
          {
            if ( bs3Instr[i].nbParam == (int)bs3line->nbParam )  /* found same number of parameter, good start */ 
            {
              found = 1; /* by default consider as found full */
              for (j = 0 ; j < bs3Instr[i].nbParam && j < 3; j++) /* search if each param match in type (param type category) */ 
              {
              
                if (! bs3_asm_pass1_param_compatible(bs3Instr[i].ptype[j], bs3line->paramType[j], bs3line->paramValue[j] ) )
                {
                  found = 0; /* parameter does not match */
                  break;
                }
              }
            }
          }
          if (found) break;
        }
        if (!found) 
        {
          err = BS3_ASM_PASS1_PARSE_ERR_BADOPESYNTAX;
          break;
        }
        /* here 'i' is the full instruction to use */
        bs3line->opeType = BS3_ASM_OPETYPE_FULL;
        bs3line->opeCode = (WORD)i;
        break;
      case BS3_ASM_OPETYPE_DIRECTIVE: /*   MACRO/ENDM/INCLUDE/ORF ...*/
      case BS3_ASM_OPETYPE_ALIAS: /*   EQU/DIST ...*/
      case BS3_ASM_OPETYPE_FULL: /* CPU instruction */
        i =  bs3line->opeCode;
        found = 0;
        if (bs3Instr[i].nbParam == (int)bs3line->nbParam)  /* found same number of parameter */ 
        {
          found = 1; /* by default consider as found full */
          for (j = 0 ; j < bs3Instr[i].nbParam && j < 3; j++) /* search if each param match in type (param type category) */ 
          {
            if (! bs3_asm_pass1_param_compatible(bs3Instr[i].ptype[j], bs3line->paramType[j], bs3line->paramValue[j] ) )
            {
              found = 0;
              break;
            }
          }
        }
        if (!found)  err = BS3_ASM_PASS1_PARSE_ERR_BADOPESYNTAX;
        break;        
      case BS3_ASM_OPETYPE_META: /* DB or DW, special case to check: variable number of param, param type multiple */
        i = bs3line->opeCode;
        found = 0;
        if (bs3line->nbParam >= 1) { /* one or more parameter */
          found = 1; /* by default it is ok, we search for the first anomaly : unexpected param type */
          switch (i)
          {
            case BS3_INSTR_DB:
              for (j = 0 ; j < bs3line->nbParam; j++)
              { 
                switch ( bs3line->paramType[j] & 0xF0)
                {
                  case BS3_ASM_PARAM_TYPE_WORD:
                    if (!bs3_asm_pass1_param_compatible( BS3_ASM_PARAM_TYPE_BYTE, BS3_ASM_PARAM_TYPE_WORD, bs3line->paramValue[j]))
                    {
                      found = 0;
                      err = BS3_ASM_PASS1_PARSE_ERR_BADBYTE;
                    }
                    break;
                  case BS3_ASM_PARAM_TYPE_STRING:
                  case BS3_ASM_PARAM_TYPE_BYTE:
                    break;
                  default:
                    found = 0;
                    err = BS3_ASM_PASS1_PARSE_ERR_BADPARAM;
                }
                if (!found) break;
              }
              break;
            case BS3_INSTR_DW:
              for (j = 0 ; j < bs3line->nbParam; j++)
              { 
                switch ( bs3line->paramType[j] & 0xF0)
                {
                  case BS3_ASM_PARAM_TYPE_WORD:
                  case BS3_ASM_PARAM_TYPE_BYTE: /* BYTE is compatible WORD */
                    break;
                  default:
                    found = 0;
                    err = BS3_ASM_PASS1_PARSE_ERR_BADPARAM;
                }
                if (!found) break;
              }
              break;
            default:
              err = BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED; /* should never occurs */
          }
        }
        if (!found && err == BS3_ASM_PASS1_PARSE_ERR_OK) err = BS3_ASM_PASS1_PARSE_ERR_BADOPESYNTAX;
  }
  return err;  
}




/* assemble an asm line  : assume parsing is ok (parse + check )*/
/* modify the bs3line->assembly[] and bs3line->assemblyLength */
/* if ok return BS3_ASM_PASS1_PARSE_ERR_OK */
/* if nothing to do return BS3_ASM_PASS1_PARSE_ERR_NOPE */
/* otherwise return error */ 
int bs3_asm_pass1_assemble(struct bs3_asm_line * bs3line)
{
  int err = BS3_ASM_PASS1_PARSE_ERR_OK; /* by default all is ok */
  int ope;
  union opParam opeParam;
  int i;
  int j;
  int k;
  BYTE rIdx;
  bs3line->assemblyLength = 0;
  ope = bs3line->opeCode;
  if (ope < 0) return BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED;
  if (bs3Instr[ope].opeType != BS3_ASM_OPETYPE_CPU &&  bs3Instr[ope].opeType != BS3_ASM_OPETYPE_META)
    return BS3_ASM_PASS1_PARSE_ERR_NOPE; /* assemble CPU or META. there is nothing to do with other */
 
  switch (bs3Instr[ope].opeType)
  {
    case BS3_ASM_OPETYPE_CPU:
      k = 0;
      while (bs3Instr[ope].asmpattern[k])
      {
        switch (bs3Instr[ope].asmpattern[k]) /* go through the pattern to generate the assembly */
        {
          case 'o': /* operation code */
            bs3line->assembly[bs3line->assemblyLength++] = (BYTE) ope;
            break;
          case 'p': /* operation code parameter */
            j = 0; /* operation code parameter register reference index */
            opeParam.param = 0;
            for (i = 0; i < bs3line->nbParam; i++) /* explore each param to extract and transform register index */
            {
              if (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_REGISTER_B)
              {
                rIdx = (BYTE)bs3line->paramValue[i] & 0x07 ; // bs3line->line[bs3line->param[i] + 1] - '0';
                j++;
                switch (j)
                {
                  case 1: 
                    opeParam.x3 = rIdx;
                    break;
                  case 2:
                  case 3:
                    opeParam.y3 = rIdx;
                } 
              } /* end of if param type register B */
              if (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_REGISTER_W)
              {
                rIdx = (BYTE)bs3line->paramValue[i] & 0x03; // bs3line->line[bs3line->param[i] + 1] - '0';
                j++;
                switch (j)
                {
                  case 1: 
                    opeParam.x2 = rIdx;
                    break;
                  case 2:
                    opeParam.y2 = rIdx;
                    break;
                  case 3:
                    opeParam.z2 = rIdx;
                } 
              } /* end of if param type register W */
              if ( 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M1) || 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M2) 
                 ) 
              {
                rIdx = bs3line->line[bs3line->param[i] + 2] - '0';
                j++;
                switch (j)
                {
                  case 1: 
                    opeParam.x2 = rIdx;
                    break;
                  case 2:
                    opeParam.y2 = rIdx;
                    break;
                  case 3:
                    opeParam.z2 = rIdx;
                } 
              } /* end of if param type M1 or M2 */
              if ( 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M3) || 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M6) 
                 ) 
              {
                rIdx = bs3line->line[bs3line->param[i] + 5] - '0';
                j++;
                switch (j)
                {
                  case 1: 
                    opeParam.x2 = rIdx;
                    break;
                  case 2:
                    opeParam.y2 = rIdx;
                    break;
                  case 3:
                    opeParam.z2 = rIdx;
                } 
              } /* end of if param type M3 or M6 */
              if ( 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M5) || 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M7) 
                 ) 
              {
                rIdx = bs3line->line[bs3line->param[i] + 2] - '0';
                j++;
                switch (j)
                {
                  case 1: 
                    opeParam.x2 = rIdx;
                    break;
                  case 2:
                    opeParam.y2 = rIdx;
                    break;
                  case 3:
                    opeParam.z2 = rIdx;
                } 
                rIdx = bs3line->line[bs3line->param[i] + 5] - '0';
                j++;
                switch (j)
                {
                  case 1: 
                    opeParam.x2 = rIdx;
                    break;
                  case 2:
                    opeParam.y2 = rIdx;
                    break;
                  case 3:
                    opeParam.z2 = rIdx;
                } 
              } /* end if param type M5 or M7 */
            } /* end of for each parameter to explore */
            bs3line->assembly[bs3line->assemblyLength++] = (BYTE) opeParam.param;
            break;
          case 'b': /* immediate BYTE */
            for (i = 0; i < bs3line->nbParam; i++)
            {
              if ( 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_BYTE) ||
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_WORD) ||
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M2)   ||
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M4)   ||
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M6)   ||
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M7)   
                 )
              {
                bs3line->assembly[bs3line->assemblyLength++] = (BYTE)(bs3line->paramValue[i] & 0x00FF);
                break;
              }
            }
            return BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED; /* should never occur ... meaning bad operation design */
          case 'r': /* immediate SBYTE representing the offset to a label */ 
            bs3line->assembly[bs3line->assemblyLength++] = 0; /* temp relative 8 bits address : real value in pass 2 */
            break;
          case 'w': /* immediate WORD or label address (address real value in pass 2) */
            for (i = 0; i < bs3line->nbParam; i++)
            {
              if ( 
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_BYTE) ||
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_WORD) ||
                   (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_M0 /* including M0LABEL case ! */)   
                )
              {
                bs3line->assembly[bs3line->assemblyLength++] = (BYTE)(bs3line->paramValue[i] & 0x00FF);
                bs3line->assembly[bs3line->assemblyLength++] = (BYTE)((bs3line->paramValue[i] >> 8) & 0x00FF);
                break;
              }
              if (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_LABEL)
              {
                bs3line->assembly[bs3line->assemblyLength++] = 0; /* temp low 8 bits of a 16 bits relative address : real value in pass 2*/
                bs3line->assembly[bs3line->assemblyLength++] = 0; /* temp high 8 bits of a 16 bits relative address : real value in pass 2 */
              }
            }
            return BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED; /* should never occur ... meaning bad operation design */
          case 'R': /* immediate SWORD of a label offset */
            bs3line->assembly[bs3line->assemblyLength++] = 0; /* temp low 8 bits of a 16 bits relative address : real value in pass 2*/
            bs3line->assembly[bs3line->assemblyLength++] = 0; /* temp high 8 bits of a 16 bits relative address : real value in pass 2 */
            break;
          default:
            return BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED; /* should never occur ... meaning bad operation design */
        }
        k++;
      } /* end of for each asmpattern code */
      break;
    case BS3_ASM_OPETYPE_META: /* special case not possible to be expressed by bs3Instr[] */
      switch (ope)
      {
        case BS3_INSTR_DB:
          for (i = 0; i < bs3line->nbParam; i++)
          {
            if (
                 (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_BYTE) ||
                 (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_WORD) 
               )
            {
              bs3line->assembly[bs3line->assemblyLength++] = (BYTE)(bs3line->paramValue[i] & 0x00FF);
            }
            if (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_STRING)
            {
              for (j = 1; bs3line->line[bs3line->param[i] + j] != '"'; j++)
                bs3line->assembly[bs3line->assemblyLength++] = bs3line->line[bs3line->param[i] + j];
            }
          }
          break;
        case BS3_INSTR_DW:
          for (i = 0; i < bs3line->nbParam; i++)
          {
            if (
                 (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_BYTE) ||
                 (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_WORD) ||
                 (bs3line->paramType[i] & 0xF0 == BS3_ASM_PARAM_TYPE_LABEL) 
               )
            {
              bs3line->assembly[bs3line->assemblyLength++] = (BYTE)(bs3line->paramValue[i] & 0x00FF);
              bs3line->assembly[bs3line->assemblyLength++] = (BYTE)((bs3line->paramValue[i] >> 8) & 0x00FF);
            }
          }
          break;
      }
      break;
  }
  return err;
}

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
    l=0; /* =1 then LABEL =0 otherwise */
    for (i = 0 ; i < length; i++) /* for each character in Symbol, verify it does comply */
    {
      switch (symbol[i])
      {
        case '.':
          l = 1;
        case 'A' ... 'Z':
        case '0' ... '9':
        case '_':
          break;
        default:
          return BS3_ASM_SYMBOLTYPE_UNKNOWN;
      }
    } /* end for each character in Symbol*/
    
    if (l) return BS3_ASM_SYMBOLTYPE_LABEL;
    /* from here 'l' does not represent 'is label' anymore */
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


int bs3_asm_pass1_oneline(struct bs3_asm_line * bs3line, WORD linenum, WORD address, const char * oneLine)
{
  int idxLine = 0;
  struct bs3_asm_line * pbs3lineFound;
  struct bs3_asm_line bs3lineFound;
  int state = BS3_ASM_PASS1_PARSE_STATE_START; 
  int symboltype;
  long value;                          /* numerical value associated to a operand           */
  int i;                               /* general index for iteration                       */
  int j;                               /* general index for iteration                       */
  int k;                               /* general index for iteration                       */
  int l;                               /* general index for iteration or length of string   */
  char c;                              /* a character in the asm line                       */
  char isok= 1;                        /* 1 parsing is ok, 0 there is an error of parsing   */
  char err=BS3_ASM_PASS1_PARSE_ERR_OK; /* by default there is no error                      */
  /* initialise the asm line result structure                                               */
  bs3line->linenum         = linenum;  /* init: set the current line in source code         */
  bs3line->line[0]         = 0;        /* init: the asm line in structure is empty          */
  bs3line->label           = -1;       /* init: no label defined in the parsed asm line     */
  bs3line->ope             = -1;       /* init: no operation defined in the parsed asm line */
  bs3line->nbParam         = 0;        /* init: no parameter in the parsed asm line         */
  bs3line->assemblyLength  = 0;        /* init: no assembly result                          */
  bs3line->assemblyAddress = address;  /* init: asm line at address                         */
  int eol;                             /* 1 end of line , 0 not end of line                 */
  /* get the asm line */
  while (oneLine[idxLine] && idxLine < BS3_ASM_LINE_SIZE)
  {
    bs3line->line[idxLine] = oneLine[idxLine];
    idxLine++;
  }
  /* check asm line is not too long */
  if (idxLine >= BS3_ASM_LINE_SIZE && oneLine[idxLine]) 
  {
    bs3line->column = BS3_ASM_LINE_SIZE;
    return BS3_ASM_PASS1_PARSE_ERR_LINETOOLONG;
  }
  bs3line->line[idxLine++] = '\n';     /* add an end of line sentinel                       */
  bs3line->line[idxLine] = '0';        /* ASCIIZ null terminate string                      */
 
  /* parse the asm line */
  idxLine = 0;
  eol = 0;
  while ((c = oneLine[idxLine]) && isok)
  {
     /* if (c == '\n' || c== '\r')  
      {
        bs3line->line[idxLine] = 0;
        break;
      }
      */
      switch(state)
      {
        /* start parsing */
        case BS3_ASM_PASS1_PARSE_STATE_START: 
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
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;
              break;
          }
          break;
          
        /* label parsing */
        case BS3_ASM_PASS1_PARSE_STATE_LABEL: 
          switch (c)
          {
            case '\n':
            case '\r':
            case ';':
              eol = 1; /* end of label by end of line detection */
            case ' ':
            case '\t':
            case ':':
              /* check if we have at least one character in case of local label */
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
              /* check duplicate label, and attach local label to first previous global label */
              err = bs3_asm_line_checkLabel(bs3line);
              if (err != BS3_ASM_PASS1_PARSE_ERR_OK)
              {
                isok = 0;
                idxLine = bs3line->label;
                break;
              }
              /* checks done and ok then carry on operation */
              state = BS3_ASM_PASS1_PARSE_STATE_ALABEL;
              idxLine++;
              break;
            case 'a' ... 'z':
               bs3line->line[idxLine] -= 32; /* to upper case label */ 
            case 'A' ... 'Z':
            case '0' ... '9':
            case '.':
            case '_':
              idxLine++;
              break;
            default:
              isok = 0;
              err = BS3_ASM_PASS1_PARSE_ERR_BADLABEL;
              break;
          }
          break;

        /* search start of instruction parsing */
        case BS3_ASM_PASS1_PARSE_STATE_ALABEL: 
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

        /* parse operator */  
        case BS3_ASM_PASS1_PARSE_STATE_OPE: 
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
                  /* human or full ope name, indicate ope quality  */
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
                case BS3_ASM_SYMBOLTYPE_SYMBOL: /* potential macro expansion */
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

        /* searching for parameter */
        case BS3_ASM_PASS1_PARSE_STATE_AOPE: 
          state = BS3_ASM_PASS1_PARSE_STATE_APARAM;
          break;

        /* searching complex parameter */  
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
            case ' ': /* legal in [...] (addressing mode) parameter */
            case '\t': /* legal in [...] (addressing mode) parameter */
            case '.': /* legal in label */
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

        /* parsing string parameter*/
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

        /* parsing a character (between quote) */
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

        /* Parsing a decimal*/
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

        /* parsing an hexadeciaml value */  
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

        /* After parameter, searching for another parameter */      
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
     if (c == '.')
     {  
       bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_LABEL;
     }
     else
     {
       pbs3lineFound = bs3_asm_line_copy( &bs3lineFound, bs3line);
       pbs3lineFound = bs3_asm_getEQU(&bs3line->line[ bs3line->paramType[i]], pbs3lineFound, pbs3lineFound);
       if (pbs3lineFound)
       {
         bs3line->paramType[i] = pbs3lineFound->paramType[0];
         bs3line->paramValue[i] =  pbs3lineFound->paramValue[0];
       }
       else
       {
          bs3line->paramType[i] == BS3_ASM_PARAM_TYPE_LABEL;
       }
     }    
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
          bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_SYMBOL; /* can be an alias or a label */
          /* Search for a value, then replace the parameter type by the corresponding type and value */
             pbs3lineFound = bs3_asm_line_copy( &bs3lineFound, bs3line);
             pbs3lineFound = bs3_asm_getEQU(&bs3line->line[ bs3line->paramType[i]], pbs3lineFound, pbs3lineFound);
             if (pbs3lineFound)
             {
               bs3line->paramType[i] = pbs3lineFound->paramType[0];
               bs3line->paramValue[i] =  pbs3lineFound->paramValue[0];
             }
             else
             {
               bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_LABEL; /* it is not an EQU then it must be a LABEL */
             }             
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
             pbs3lineFound = bs3_asm_line_copy( &bs3lineFound, bs3line);
             pbs3lineFound = bs3_asm_getEQU(&bs3line->line[ bs3line->paramType[i]], pbs3lineFound, pbs3lineFound);
             if (pbs3lineFound)
             { 
               if ( (pbs3lineFound->paramType[0] & 0xF0) == BS3_ASM_PARAM_TYPE_BYTE)
               {
                 bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M6DECIMAL; /* only alias */
                 bs3line->paramValue[i] = pbs3lineFound->paramValue[0]; /* value of the symbol */
                 continue; /* go to next param */
               }
               else
               {
                 isok = 0;
                 err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLTOOBIG;
                 idxLine = j+6;
                 continue;                 
               }
             }
             else 
             {
               isok = 0;
               idxLine = j+6;
               err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLNOTFOUND;
               continue;
             }             
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
             pbs3lineFound = bs3_asm_line_copy( &bs3lineFound, bs3line);
             pbs3lineFound = bs3_asm_getEQU(&bs3line->line[ bs3line->paramType[i]], pbs3lineFound, pbs3lineFound);
             if (pbs3lineFound)
             { 
               if ( (pbs3lineFound->paramType[0] & 0xF0) == BS3_ASM_PARAM_TYPE_BYTE)
               {
                 bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M7DECIMAL; /* only alias */
                 bs3line->paramValue[i] = pbs3lineFound->paramValue[0]; /* value of the symbol */
                 continue; /* go to next param */
               }
               else
               {
                 isok = 0;
                 err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLTOOBIG;
                 idxLine = j+6; 
                 continue;                
               }
             }
             else 
             {
               isok = 0;
               idxLine = j+6;
               err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLNOTFOUND;
               continue;
             }             
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
             pbs3lineFound = bs3_asm_line_copy( &bs3lineFound, bs3line);
             pbs3lineFound = bs3_asm_getEQU(&bs3line->line[ bs3line->paramType[i]], pbs3lineFound, pbs3lineFound);
             if (pbs3lineFound)
             { 
               if ( (pbs3lineFound->paramType[0] & 0xF0) == BS3_ASM_PARAM_TYPE_BYTE)
               {
                 bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M4DECIMAL; /* only alias */
                 bs3line->paramValue[i] = pbs3lineFound->paramValue[0]; /* value of the symbol */
                 continue; /* go to next param */
               }
               else
               {
                 isok = 0;
                 err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLTOOBIG;
                 idxLine = j+4;
                 continue;                 
               }
             }
             else 
             {
               isok = 0;
               idxLine = j+4;
               err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLNOTFOUND;
               continue;
             }             
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
             pbs3lineFound = bs3_asm_line_copy( &bs3lineFound, bs3line);
             pbs3lineFound = bs3_asm_getEQU(&bs3line->line[ bs3line->paramType[i]], pbs3lineFound, pbs3lineFound);
             if (pbs3lineFound)
             { 
               if ( (pbs3lineFound->paramType[0] & 0xF0) == BS3_ASM_PARAM_TYPE_BYTE)
               {
                 bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M2DECIMAL; /* only alias */
                 bs3line->paramValue[i] = pbs3lineFound->paramValue[0]; /* value of the symbol */
                 continue; /* go to next param */
               }
               else
               {
                 isok = 0;
                 err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLTOOBIG;
                 idxLine = j+4;
                 continue;                 
               }
             }
             else 
             {
               isok = 0;
               idxLine = j+4;
               err = BS3_ASM_PASS1_PARSE_ERR_SYMBOLNOTFOUND;
               continue;
             }             
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
             pbs3lineFound = bs3_asm_line_copy( &bs3lineFound, bs3line);
             pbs3lineFound = bs3_asm_getEQU(&bs3line->line[ bs3line->paramType[i]], pbs3lineFound, pbs3lineFound);
             if (pbs3lineFound)
             { 
               bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M0DECIMAL; /* only alias */
               bs3line->paramValue[i] = pbs3lineFound->paramValue[0]; /* value of the symbol */
             }
             else 
             {
               bs3line->paramType[i] = BS3_ASM_PARAM_TYPE_M0LABEL; /* only alias */
               bs3line->paramValue[i] = 0; /* default value */
             }
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
  
  /* validate instruction + assemble the code*/
  if (isok && bs3line->ope != -1)
  { 
    bs3line->assemblyLength = 0; /* by default nothing to assemble */
    bs3line->assemblyAddress = address; /* memory address of the result assembly */
    switch (bs3line->opeType)
    {
      case BS3_ASM_OPETYPE_SYMBOL: /* macro invocation : managed by caller of this function */
        break;
      case BS3_ASM_OPETYPE_HUMAN: /* need to precise the FULL instruction */
      case BS3_ASM_OPETYPE_FULL: /* check if the parameter is ok , then assemble the code */
        if ((err = bs3_asm_pass1_instructionCheck(bs3line)) != BS3_ASM_PASS1_PARSE_ERR_OK)
        {
          isok = 0;
          idxLine = bs3line->ope;
          break;
        }
        /* Assemble the code */
        err =  bs3_asm_pass1_assemble(bs3line);
        if (err == BS3_ASM_PASS1_PARSE_ERR_NOPE) err = BS3_ASM_PASS1_PARSE_ERR_OK;
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK)
        {
          isok = 0;
          idxLine = bs3line->ope;
        }
        break;
      case BS3_ASM_OPETYPE_META: /* DB or DW */
        if ((err = bs3_asm_pass1_instructionCheck(bs3line)) != BS3_ASM_PASS1_PARSE_ERR_OK)
        {
          isok = 0;
          idxLine = bs3line->ope;
          break;
        }
        /* Assemble the code */
        err =  bs3_asm_pass1_assemble(bs3line);
        if (err == BS3_ASM_PASS1_PARSE_ERR_NOPE) err = BS3_ASM_PASS1_PARSE_ERR_OK;
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK)
        {
          isok = 0;
          idxLine = bs3line->ope;
        }
        break;
      case BS3_ASM_OPETYPE_ALIAS:
      case BS3_ASM_OPETYPE_DIRECTIVE: /*  managed by caller of this function : MACRO/ENDM/INCLUDE.. ORG have to fix the assembly address, if a label is defined */
        if ((err = bs3_asm_pass1_instructionCheck(bs3line)) != BS3_ASM_PASS1_PARSE_ERR_OK)
        {
          isok = 0;
          idxLine = bs3line->ope;
          break;
        }
        break;
      default:
        isok = 0;
        idxLine = bs3line->ope;
        err = BS3_ASM_PASS1_PARSE_ERR_BADOPETYPE;
    }
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

int fileId = 0;

int bs3_asm_pass1_file( const char * filename, WORD address, long asmIndexMacro)
{
  int linenum = 0;                             /* line number in file        */
  int achar   = 0;                             /* a character in a file line */
  int lineidx = 0;                             /* index in file line         */
  int lineerr = BS3_ASM_PASS1_PARSE_ERR_OK;
  int err     = BS3_ASM_PASS1_PARSE_ERR_OK;
  int i;
  int j;
  int k;
  int m;
  int n;
  BYTE isMacroExpansion = (asmIndexMacro > -1);
  struct bs3_asm_line * pbs3_asm;
  struct bs3_asm_line abs3_asm;
  struct bs3_asm_line macrobs3_asm;
  pbs3_asm = &abs3_asm;
  FILE * fp;
  FILE * macroFile;
  BYTE isMacroRecording=0;
  char fileline[BS3_ASM_LINE_BUFFER];
  char macroline[BS3_ASM_LINE_BUFFER];
  char includefilename[BS3_ASM_LINE_BUFFER+10];
  /* check file : not null and not empty */
  if (filename == 0 || (filename[0] >= 0 && filename[0] <= ' ') ) 
  {
     bs3_asm_report( "<BAD FILE NAME>",0 ,0 ,BS3_ASM_PASS1_ERR_BADFILE ) ;
    return BS3_ASM_PASS1_ERR_BADFILE;
  } 
  
  /* add a global label of the file*/
  fileId++;
  if (bs3_asm_line_nextfree( pbs3_asm ) == 0) 
  {
    err == BS3_ASM_PASS1_PARSE_ERR_MEMORY;
    bs3_asm_report(filename, linenum, 0, err);
    return err;
  }
  /* add a global label of the file : compute the label name */  
  i = 0;
  j = -1;
  while (filename[i]) /* search last '/' or '\' */
  {
    if (filename[i] == '/' || filename[i] == '\\') j = i;
    i++;
  }
  j++; /* fist character if there is no '/' found, otherwise first char after last '/' */
  while (filename[j] && filename[j] == '.') j++; /* do not take '.' in front of the file name: avoid confusion with local label */
    sprintf(fileline, "_%s_%d:  ;%s\n",&filename[j], fileId, &filename[j]);
  /* sanitize the label name */
  i = 0;
  while (fileline[i])
  {
    if (fileline[i] == ':') break;
    switch (fileline[i])
    {
      case 'a' ... 'z':
        fileline[i] -= 32; /* upper case the label */
      case 'A' ... 'Z':
      case '0' ... '9':
      case '_':
        break;
      default:      
        fileline[i] = '_';
    }
    i++;
  }
  /* add a global label of the file : parse the label */
  lineerr = bs3_asm_pass1_oneline(pbs3_asm, (WORD)linenum, (WORD) address, fileline);
  if (lineerr != BS3_ASM_PASS1_PARSE_ERR_OK && lineerr != BS3_ASM_PASS1_PARSE_ERR_NOPE) 
  {
    err = lineerr;
    bs3_asm_report(filename, linenum , pbs3_asm->column , err) ;
    return err; 
  } 
  lineerr = bs3_asm_line_commit(pbs3_asm); /* validate the addition of the label */
  if (lineerr != BS3_ASM_PASS1_PARSE_ERR_OK && lineerr != BS3_ASM_PASS1_PARSE_ERR_NOPE) 
  {
    err = lineerr;
    bs3_asm_report(filename, linenum , pbs3_asm->column , err) ;
    return err; 
  } 
 
  /* open file */
  fp = fopen(filename, "rt");
  if (fp == NULL)
  {
    bs3_asm_report(filename, 0,0, BS3_ASM_PASS1_ERR_BADFILE) ;
    return BS3_ASM_PASS1_ERR_BADFILE;
  }
 
  /* explore file line per line */
  while (! feof(fp))
  {
    /* read a line in file */
    lineidx = 0;    
    while ((! feof(fp)) && lineidx < (BS3_ASM_LINE_BUFFER - 1))
    {
      achar = fgetc(fp);
      if (achar == -1) break;
      if (achar != '\r') /* take all character except 'carrier return' (CR, \r) */ 
      {
        fileline[lineidx] = (BYTE)achar;
        lineidx++;
        if (achar == '\n') break; /* 'line feed' means 'end of line' */
        if (achar < ' ' && achar >= 0 && achar != '\t')  /* all character below ' ' except \r \n \t are considered as illegal */
        {
          err = BS3_ASM_PASS1_PARSE_ILLEGALCHAR;
          bs3_asm_report(filename, linenum + 1 ,lineidx, err);
          break;
        }
      } else continue;
    } /* end while 'character in line' . exit when end of line, line length exceeded or end of file */
    if (err != BS3_ASM_PASS1_PARSE_ERR_OK) break; // error during file line reading.
    if (lineidx > 0 && !feof(fp) && fileline[lineidx-1] != '\n') /* if line length exceeded */
    {
      err = BS3_ASM_PASS1_PARSE_ERR_LINETOOLONG;
      bs3_asm_report(filename, linenum + 1 ,1, err) ;
      break;
    }
    fileline[lineidx] = 0;/* add  ASCIIZ null terminated string character */
    linenum++; /* line is read, increment the line number */

    if (isMacroRecording) { /* if recording then save line to macro file */
        lineerr = bs3_asm_pass1_oneline(pbs3_asm, (WORD)linenum, (WORD) address, fileline);
        if (lineerr != BS3_ASM_PASS1_PARSE_ERR_OK || pbs3_asm->opeCode != BS3_INSTR_ENDM)
        {
          fprintf(macroFile, "%s", fileline);
          continue;
        } 
    }
    
    
    /* Parse file line */
    if (isMacroExpansion) 
    {
      /* find '{x}' (x is a number from 0 to n)in line and replace it by the parameter of asm line at index asmIndexMacro 
         if 'x' does not exist in asm line at index asmIndexMacro, then error
         if expansion make the line too large , then error
         fileline & macroline
      */
      bs3_asm_line_at(asmIndexMacro,&macrobs3_asm);
      j = 0;
      k = 0; /* =0 in line, =1 in {} */
      for (i= 0; fileline[i]; i++)
      {
        switch (k)
        {
          case 0: /* in text */
            switch (fileline[i])
            {
              case '{':
                k = 1;
                m = i;
                break;
              default:
                if (j >= (BS3_ASM_LINE_BUFFER - 1) )
                {
                  err = BS3_ASM_PASS1_PARSE_ERR_MACROLINE2BIG;
                  bs3_asm_report(filename, linenum ,i, err) ;
                  break;
                }
                macroline[j++] = fileline[i];
                
            }
            break;
          case 1: /* in {xxx} */
            switch (fileline[i])
            {
              case '}':
                k = 0; /* return to 'in text ' */
                if (i== (m+1)) /* error if we have no index '{}' */
                {
                  err = BS3_ASM_PASS1_PARSE_ERR_MACROSYNTAX;
                  bs3_asm_report(filename, linenum ,i, err) ;
                  break;
                }
                /* get the param index */
                fileline[i] = 0;
                m = sscanf(&fileline[m+1],"%d",&m);
                fileline[i] = '}';
                /* if param index is too large */
                if (m > macrobs3_asm.nbParam || m == 0) /* one based index */
                {
                  err = BS3_ASM_PASS1_PARSE_ERR_MACROREFPARAM;
                  bs3_asm_report(filename, linenum ,i, err) ;
                  
                  break;
                }
                /* get the param content then append it to macroline */
                n = 0;
                while (macrobs3_asm.line[macrobs3_asm.param[m-1]+n])
                {
                  if (j >= (BS3_ASM_LINE_BUFFER - 1) )
                  {
                    err = BS3_ASM_PASS1_PARSE_ERR_MACROLINE2BIG;
                    bs3_asm_report(filename, linenum ,i, err);
                    break;
                  }
                  macroline[j++] = macrobs3_asm.line[macrobs3_asm.param[m-1]+n];
                  n++;
                }
                break;
              case '0' ... '9': /* param index is composed by digit only  */
                break;
              default: /* if param index is not a digit then syntax error */
                err = BS3_ASM_PASS1_PARSE_ERR_MACROSYNTAX;
                bs3_asm_report(filename, linenum ,i, err) ;
            }
            break;
        } 
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK) break;
      } /* end of for each character in filename explored for macro parameter substitution */
      if (k == 1) /* we are still inside a {} of a macroline  */
      {
        err = BS3_ASM_PASS1_PARSE_ERR_MACROSYNTAX;
        bs3_asm_report(filename, linenum ,i, err) ;
        break;
      }
      macroline[j] = 0;
      strcpy(fileline, macroline);
    } /* end of macro line substitution */
    if (err != BS3_ASM_PASS1_PARSE_ERR_OK) break;
    if (bs3_asm_line_nextfree(pbs3_asm) == 0) 
    {
      err == BS3_ASM_PASS1_PARSE_ERR_MEMORY;
      bs3_asm_report(filename, linenum, 0, err);
      break;
    }
    lineerr = bs3_asm_pass1_oneline(pbs3_asm, (WORD)linenum, (WORD) address, fileline);
    if (lineerr == BS3_ASM_PASS1_PARSE_ERR_NOPE) continue; /* nothing to parse then carry on on the next file line, but keep current  */
    
    if (lineerr != BS3_ASM_PASS1_PARSE_ERR_OK) /* error during parsing of the line then report the error and stop parsing current file*/
    {
      err = lineerr;
      bs3_asm_report(filename, linenum , pbs3_asm->column , err) ;
      break;
    }
    switch (pbs3_asm->opeType)
    {
      case BS3_ASM_OPETYPE_SYMBOL: /* possible macro expansion */
        err = bs3_asm_line_commit(pbs3_asm);
        if (err == BS3_ASM_PASS1_PARSE_ERR_OK)
        {
            sprintf(includefilename, "%s.macro", &pbs3_asm->line[pbs3_asm->ope]);
            err = bs3_asm_pass1_file(includefilename ,  address, pbs3_asm->asmIndex);
        }
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK)
        {
          err = BS3_ASM_PASS1_PARSE_ERR_EXPMACRO;
          bs3_asm_report(filename, linenum , pbs3_asm->column , err);
          break;
        }
        
        /* include the macro with parameters */
        break;
      case BS3_ASM_OPETYPE_FULL: /* fully qualified CPU instruction */
      case BS3_ASM_OPETYPE_META: /* DB or DW meta instruction */
        if (  (WORD)(address + pbs3_asm->assemblyLength) < address) /* check that assembly result to not go after adress 64k */
        {
          err = BS3_ASM_PASS1_PARSE_ERR_BEYOND;
          bs3_asm_report(filename, linenum , pbs3_asm->column , err);
          break;
        }
        address += pbs3_asm->assemblyLength;
        err = bs3_asm_line_commit(pbs3_asm);
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
        {
            bs3_asm_report(filename, linenum, pbs3_asm->column, err);
        }
        break;
      case BS3_ASM_OPETYPE_DIRECTIVE: /* INCLUDE/ MACRO/ENDM/ORG */
        switch (pbs3_asm->opeCode)
        {
          case BS3_INSTR_ORG:
            address = pbs3_asm->paramValue[0];
            pbs3_asm->assemblyAddress = address; /* adjust the address , needed if there is a label */
            err = bs3_asm_line_commit(pbs3_asm);
            if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
            {
                bs3_asm_report(filename, linenum, pbs3_asm->column, err);
            }
            break;
          case BS3_INSTR_MACRO:
            /* switch to macro recording mode */
            isMacroRecording = 1;
            sprintf(includefilename, "%s.macro", &pbs3_asm->line[pbs3_asm->label]);
            macroFile = fopen(includefilename,"wt");
            break;
          case BS3_INSTR_ENDM:
            /* end of macro recording mode */
            isMacroRecording = 0;  
            fclose(macroFile);  
            macroFile = 0;        
            break;
          case BS3_INSTR_INCLUDE:
            err = bs3_asm_line_commit(pbs3_asm);
            if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
            {
                bs3_asm_report(filename, linenum, pbs3_asm->column, err);
                break;
            }
            i=0;
            while (pbs3_asm->line[pbs3_asm->param[0]+i+1] != '"' && i <BS3_ASM_LINE_BUFFER-1)
            {
              includefilename[i] = pbs3_asm->line[pbs3_asm->param[0]+i+1];
            }
            includefilename[i] = 0;
            err = bs3_asm_pass1_file(includefilename ,  address, -1);
            if (err != BS3_ASM_PASS1_PARSE_ERR_OK)
            {
              err = BS3_ASM_PASS1_PARSE_ERR_INCLUDE;
              bs3_asm_report(filename, linenum , pbs3_asm->column , err);
              break;
            }
            break;
        }
        break;
      case BS3_ASM_OPETYPE_ALIAS: /* EQU/( TODO: DIST) */
        if (pbs3_asm->label == -1) {
          err = BS3_ASM_PASS1_PARSE_ERR_NOLABEL;
          bs3_asm_report(filename, linenum , 0 , err) ;
          break;
        }
        err = bs3_asm_line_commit(pbs3_asm);
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
        {
            bs3_asm_report(filename, linenum, pbs3_asm->column, err);
        }
        break;
      default:
        break;
    }
    if (err != BS3_ASM_PASS1_PARSE_ERR_OK) break;
    
  } /* end while : exiting means end of file, or error during parsing */
  if ( err == BS3_ASM_PASS1_PARSE_ERR_OK && isMacroRecording) /* End of source file , but unfinished macro recording */ 
  {
    isMacroRecording = 0;
    if (macroFile)
    {
        fclose(macroFile);  
        macroFile = 0;  
    }
    err = BS3_ASM_PASS1_PARSE_ERR_ENDMNOTFOUND;
    bs3_asm_report(filename, linenum ,1, err) ;
  }
  fclose(fp);
  return err;
}
