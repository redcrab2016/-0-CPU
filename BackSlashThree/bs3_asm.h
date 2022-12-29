#include "bs3.h"

#ifndef _BS3_ASM_H
#define _BS3_ASM_H 1

/*  ASSEMBLER */
#define BS3_ASM_LINE_SIZE     72
#define BS3_ASM_LINE_BUFFER  (BS3_ASM_LINE_SIZE + 2)
#define BS3_ASM_LINE_MAX      65536

/* param types: claims by grammar or detected in asm code : mask 0xF0 for type, mask 0x0F for sub type */
#define BS3_ASM_PARAM_TYPE_STRING        0x00
#define BS3_ASM_PARAM_TYPE_WORDS         0x01
#define BS3_ASM_PARAM_TYPE_BYTES         0x02
#define BS3_ASM_PARAM_TYPE_BYTE          0x10
#define BS3_ASM_PARAM_TYPE_BYTE_DECIMAL  0x11
#define BS3_ASM_PARAM_TYPE_BYTE_HEXA     0x12
#define BS3_ASM_PARAM_TYPE_BYTE_CHAR     0x13
#define BS3_ASM_PARAM_TYPE_BYTE_SYMBOL   0x14
#define BS3_ASM_PARAM_TYPE_SBYTE_DECIMAL 0x15
#define BS3_ASM_PARAM_TYPE_WORD          0x20
#define BS3_ASM_PARAM_TYPE_WORD_DECIMAL  0x21
#define BS3_ASM_PARAM_TYPE_WORD_HEXA     0x22
#define BS3_ASM_PARAM_TYPE_WORD_SYMBOL   0x23
#define BS3_ASM_PARAM_TYPE_SWORD_DECIMAL 0x24
#define BS3_ASM_PARAM_TYPE_MISC          0x30
#define BS3_ASM_PARAM_TYPE_SYMBOL        0x31
#define BS3_ASM_PARAM_TYPE_UNKNOWN       0x3F
#define BS3_ASM_PARAM_TYPE_ANY           0x3F
#define BS3_ASM_PARAM_TYPE__             0x3F
#define BS3_ASM_PARAM_TYPE_REGISTER_B    0x40
#define BS3_ASM_PARAM_TYPE_REGISTER_W    0x50
#define BS3_ASM_PARAM_TYPE_REGISTER_SP   0x60
#define BS3_ASM_PARAM_TYPE_M0            0x70
#define BS3_ASM_PARAM_TYPE_M0DECIMAL     0x71
#define BS3_ASM_PARAM_TYPE_M0HEXA        0x72
#define BS3_ASM_PARAM_TYPE_M0CHAR        0x73
#define BS3_ASM_PARAM_TYPE_M0SYMBOL      0X74
#define BS3_ASM_PARAM_TYPE_M0LABEL       0X75
#define BS3_ASM_PARAM_TYPE_M1            0x80
#define BS3_ASM_PARAM_TYPE_M2            0x90
#define BS3_ASM_PARAM_TYPE_M2DECIMAL     0x91
#define BS3_ASM_PARAM_TYPE_M2HEXA        0x92
#define BS3_ASM_PARAM_TYPE_M2CHAR        0x93
#define BS3_ASM_PARAM_TYPE_M2SYMBOL      0x94
#define BS3_ASM_PARAM_TYPE_M3            0xA0
#define BS3_ASM_PARAM_TYPE_M4            0xB0
#define BS3_ASM_PARAM_TYPE_M4DECIMAL     0xB1
#define BS3_ASM_PARAM_TYPE_M4HEXA        0xB2
#define BS3_ASM_PARAM_TYPE_M4CHAR        0xB3
#define BS3_ASM_PARAM_TYPE_M4SYMBOL      0xB4
#define BS3_ASM_PARAM_TYPE_M5            0xC0
#define BS3_ASM_PARAM_TYPE_M6            0xD0
#define BS3_ASM_PARAM_TYPE_M6DECIMAL     0xD1
#define BS3_ASM_PARAM_TYPE_M6HEXA        0xD2
#define BS3_ASM_PARAM_TYPE_M6CHAR        0xD3
#define BS3_ASM_PARAM_TYPE_M6SYMBOL      0xD4
#define BS3_ASM_PARAM_TYPE_M7            0xE0
#define BS3_ASM_PARAM_TYPE_M7DECIMAL     0xE1
#define BS3_ASM_PARAM_TYPE_M7HEXA        0xE2
#define BS3_ASM_PARAM_TYPE_M7CHAR        0xE3
#define BS3_ASM_PARAM_TYPE_M7SYMBOL      0xE4
#define BS3_ASM_PARAM_TYPE_LABEL         0xF0

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
#define BS3_ASM_PASS1_PARSE_ERR_OK             0
#define BS3_ASM_PASS1_PARSE_ERR_NOPE           1
#define BS3_ASM_PASS1_PARSE_ERR_FAIL           2
#define BS3_ASM_PASS1_PARSE_ERR_LINETOOLONG    3
#define BS3_ASM_PASS1_PARSE_ERR_EMPTYLABEL     4
#define BS3_ASM_PASS1_PARSE_ERR_BADLABEL       5
#define BS3_ASM_PASS1_PARSE_ERR_BADOPE         6
#define BS3_ASM_PASS1_PARSE_ERR_BADPARAM       7
#define BS3_ASM_PASS1_PARSE_ERR_BADSTRING      8
#define BS3_ASM_PASS1_PARSE_ERR_BADCHAR        9
#define BS3_ASM_PASS1_PARSE_ERR_BADNUMBER      10
#define BS3_ASM_PASS1_PARSE_ERR_BADREGISTER    11
#define BS3_ASM_PASS1_PARSE_ERR_BADSYMBOL      12
#define BS3_ASM_PASS1_PARSE_ERR_KEYWORD        13
#define BS3_ASM_PASS1_PARSE_ERR_ADDRMODE       14
#define BS3_ASM_PASS1_PARSE_ERR_BIGVALUE       15
#define BS3_ASM_PASS1_PARSE_ERR_NOALIAS        16
#define BS3_ASM_PASS1_PARSE_ERR_BADHEXA        17
#define BS3_ASM_PASS1_ERR_BADFILE              18
#define BS3_ASM_PASS1_PARSE_ILLEGALCHAR        19
#define BS3_ASM_PASS1_PARSE_ERR_SYMBOLNOTFOUND 20
#define BS3_ASM_PASS1_PARSE_ERR_SYMBOLTOOBIG   21
#define BS3_ASM_PASS1_PARSE_ERR_BADOPETYPE     22
#define BS3_ASM_PASS1_PARSE_ERR_BADOPESYNTAX   22
#define BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED     23
#define BS3_ASM_PASS1_PARSE_ERR_BADBYTE        24
#define BS3_ASM_PASS1_PARSE_ERR_MEMORY         25
#define BS3_ASM_PASS1_PARSE_ERR_NOLABEL        26
#define BS3_ASM_PASS1_PARSE_ERR_INCLUDE        27
#define BS3_ASM_PASS1_PARSE_ERR_BEYOND         28
#define BS3_ASM_PASS1_FAILURE                  30
#define BS3_ASM_PASS1_PARSE_ERR_DUPLABEL       31
#define BS3_ASM_PASS1_PARSE_ERR_EXPMACRO       32
#define BS3_ASM_PASS1_PARSE_ERR_MACROSYNTAX    33
#define BS3_ASM_PASS1_PARSE_ERR_MACROREFPARAM  34
#define BS3_ASM_PASS1_PARSE_ERR_MACROLINE2BIG  35
#define BS3_ASM_PASS1_PARSE_ERR_ENDMNOTFOUND   36
#define BS3_ASM_PASS1_PARSE_ERR_ASMOVERLAP     37
#define BS3_ASM_PASS2_FAILURE                  38
#define BS3_ASM_PASS2_ERR_UNEXPECTED           39
#define BS3_ASM_PASS2_ERR_LABELNOTFOUND        40
#define BS3_ASM_PASS2_ERR_LABEL2FAR            41

/* Symbol type */
#define BS3_ASM_SYMBOLTYPE_UNKNOWN             0x00
#define BS3_ASM_SYMBOLTYPE_DECIMAL             0x10
#define BS3_ASM_SYMBOLTYPE_DECIMAL_BYTE        0x10
#define BS3_ASM_SYMBOLTYPE_DECIMAL_WORD        0x11
#define BS3_ASM_SYMBOLTYPE_DECIMAL_SBYTE       0x18
#define BS3_ASM_SYMBOLTYPE_DECIMAL_SWORD       0x19
#define BS3_ASM_SYMBOLTYPE_DECIMAL_BIG         0x1F
#define BS3_ASM_SYMBOLTYPE_HEXA                0x20
#define BS3_ASM_SYMBOLTYPE_HEXA_BYTE           0x20
#define BS3_ASM_SYMBOLTYPE_HEXA_WORD           0x21
#define BS3_ASM_SYMBOLTYPE_HEXA_BIG            0x2F
#define BS3_ASM_SYMBOLTYPE_CHAR                0x30
#define BS3_ASM_SYMBOLTYPE_REGISTER            0x40
#define BS3_ASM_SYMBOLTYPE_REGISTER_BYTE       0x40
#define BS3_ASM_SYMBOLTYPE_REGISTER_WORD       0x41
#define BS3_ASM_SYMBOLTYPE_REGISTER_SP         0x42
#define BS3_ASM_SYMBOLTYPE_KEYWORD             0x50
#define BS3_ASM_SYMBOLTYPE_SYMBOL              0x60
#define BS3_ASM_SYMBOLTYPE_LABEL               0x70

/* Operation type */
#define BS3_ASM_OPETYPE_SYMBOL                 0x00
#define BS3_ASM_OPETYPE_HUMAN                  0x01
#define BS3_ASM_OPETYPE_FULL                   0x02
#define BS3_ASM_OPETYPE_META                   0x03
#define BS3_ASM_OPETYPE_DIRECTIVE              0x04
#define BS3_ASM_OPETYPE_CPU                    0x05
#define BS3_ASM_OPETYPE_ALIAS                  0x06

struct bs3_asm_line
{
  /* asm_line_index */
  long asmIndex;
  /* source file info */
  WORD linenum;
  char line[BS3_ASM_LINE_BUFFER];
  /* label info + column place of error info */
  union {
    SBYTE label; /* -1 = no label, 0= index at line[] */
    SBYTE column; /* if parsing error, then it indicates the character index in the line where the error has been detected */
  };
  long asmIndexToGlobalLabel; /* asmIndex of global label, when label is local */
  BYTE labelIsAlias; /* =0 if label repesent a place in code, =1 if label represent an alias of a value (like EQU, DIST ...) */
  /* operation info */
  SBYTE ope; /* operation , index inside line[] */
  BYTE opeType; /* one amongst SYMBOL(macro invocation), HUMAN, FULL,META, DIRECTIVE,CPU and ALIAS */
  WORD opeCode; /* CPU operation indentifier: 0 to 0xFF for CPU, 0x100 and above for META DIRECTIVE and ALIAS */
  /* operation parameter info */
  BYTE nbParam;
  SBYTE param[40]; /* store the index inside line[] */
  BYTE paramType[40];
  long paramValue[40]; /* meaning depends of paramType, BS3_ASM_PARAM_TYPE...DECIMAL/HEXA/CHAR the value , SYMBOL/LABEL the index in the param... */
  /* assembly info */
  WORD assemblyAddress; /* memory address in the code */
  BYTE assemblyLength;
  BYTE assembly[BS3_ASM_LINE_BUFFER];
};

struct bs3_asm_code_map
{
  BYTE code[65536];
  BYTE inUse[65536];
};

extern struct bs3_asm_code_map  bs3_asm_map; 

extern const char * bs3_asm_message[];

extern struct bs3_asm_line bs3_asm[]; /* to be managed as a sequential third party resource ( as a file) */
extern long bs3_asm_nbline = 0; /* current size of bs3_asm usage */

void bs3_asm_line_reset();
struct bs3_asm_line *  bs3_asm_line_copy(struct bs3_asm_line * dest, struct bs3_asm_line * src);
struct bs3_asm_line * bs3_asm_line_first(struct bs3_asm_line * bs3line);
long bs3_asm_line_size();
struct bs3_asm_line * bs3_asm_line_at(long index, struct bs3_asm_line * bs3line);
struct bs3_asm_line * bs3_asm_line_atlabel(const char * label, struct bs3_asm_line * bs3line,  struct bs3_asm_line * bs3lineLabel);
struct bs3_asm_line * bs3_asm_line_last(struct bs3_asm_line * bs3line);
struct bs3_asm_line * bs3_asm_line_nextfree(struct bs3_asm_line * bs3line);
int bs3_asm_line_commit(struct bs3_asm_line *  bs3line);
int bs3_asm_line_checkLabel(struct bs3_asm_line * bs3line);
struct bs3_asm_line * bs3_asm_getEQU(const char * symbol, struct bs3_asm_line *  curbs3line, struct bs3_asm_line * foundbs3line);

void bs3_asm_report(const char * filename, int line, int linecolumn, int message);
int bs3_asm_pass1_param_compatible(int destType, int srcType, long srcValue);
int bs3_asm_pass1_instructionCheck(struct bs3_asm_line * bs3line);
int bs3_asm_pass1_assemble(struct bs3_asm_line * bs3line);
int bs3_asm_pass1_symboltype(const char * symbol, int length, long * pvalue);
int bs3_asm_pass1_oneline(struct bs3_asm_line * bs3line, WORD linenum, WORD address, const char * oneLine);
int bs3_asm_pass1_file( const char * filename, WORD address, long asmIndexMacro);
int bs3_asm_pass2();
int bs3_asm_file( const char * filename);

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


#endif
