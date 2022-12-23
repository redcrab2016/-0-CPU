#include "bs3.h"

#ifndef _BS3_ASM_H
#define _BS3_ASM_H 1

extern const char * bs3_asm_message[];

void bs3_asm_line_reset();
struct bs3_asm_line *  bs3_asm_line_copy(struct bs3_asm_line * dest, struct bs3_asm_line * src);
struct bs3_asm_line * bs3_asm_line_first(struct bs3_asm_line * bs3line);
long bs3_asm_line_size();
struct bs3_asm_line * bs3_asm_line_at(long index, struct bs3_asm_line * bs3line);
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
int bs3_asm_file( const char * filename);



#endif
