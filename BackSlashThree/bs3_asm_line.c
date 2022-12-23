#include "bs3_asm.h"

struct bs3_asm_line bs3_asm[BS3_ASM_LINE_MAX]; /* to be managed as a sequential third party resource */
long bs3_asm_nbline = 0; /* current size of bs3_asm usage */

/*
  reset all asm line contents
*/
void bs3_asm_line_reset()
{
  bs3_asm_nbline = 0;
}

struct bs3_asm_line *  bs3_asm_line_copy(struct bs3_asm_line * dest, struct bs3_asm_line * src)
{
  BYTE * pSrc;
  BYTE * pDest;
  int i;
  if (dest == 0 || src == 0 ) return ((void *)0);
  pSrc = (BYTE *) src;
  pDest = (BYTE *) dest;
  for (i = 0 ; i < sizeof(struct bs3_asm_line); i++)
  {
    pDest[i] = pSrc[i];
  }
  
}

/*
  return first asm line 
*/
struct bs3_asm_line * bs3_asm_line_first(struct bs3_asm_line * bs3line)
{
  if (bs3_asm_nbline == 0) return ((void *)0);  
  return bs3_asm_line_copy(bs3line, bs3_asm);
}

/*
 return how many asm line
*/
long bs3_asm_line_size() 
{
  return bs3_asm_nbline;
}

/*
  get thr asm line zeo based index asm line 
*/
struct bs3_asm_line * bs3_asm_line_at(long index, struct bs3_asm_line * bs3line)
{
  if (index > bs3_asm_nbline-1 || index < 0 ) return ((void *)0);
  return bs3_asm_line_copy(bs3line, bs3_asm + index);
}

/*
 get the last asm line 
*/
struct bs3_asm_line * bs3_asm_line_last(struct bs3_asm_line * bs3line)
{
  if (bs3_asm_nbline ==0) return ((void *)0);
  return bs3_asm_line_copy(bs3line, bs3_asm + (bs3_asm_nbline-1));
}

/*
  provide a free asm line to construct
*/
struct bs3_asm_line * bs3_asm_line_nextfree(struct bs3_asm_line * bs3line)
{
  if (bs3_asm_nbline == BS3_ASM_LINE_MAX) return ((void *)0);
  bs3_asm[bs3_asm_nbline].asmIndex = bs3_asm_nbline;
  return bs3_asm_line_copy(bs3line, bs3_asm + bs3_asm_nbline);
}

/*
  add a constructed asm line (line retrieved by a .. nextfree()
*/
int bs3_asm_line_commit(struct bs3_asm_line *  bs3line) 
{
  int currIndex; /* = (bs3line - bs3_asm);*/
  if (bs3line == 0) return 0;
  currIndex = bs3line->asmIndex;
  if (currIndex = bs3_asm_nbline) 
  {
    /* save new line */
    bs3_asm_line_copy(bs3_asm + bs3_asm_nbline, bs3line);
    bs3_asm_nbline ++;
    return 1;
  }
  if (currIndex < bs3_asm_nbline && currIndex >= 0)
  {
    /* update the line: no file then nothing to do .. index in bs3_asm[currIndex].asmIndex */
    bs3_asm_line_copy(bs3_asm + currIndex, bs3line);
    return 1; 
  } 
  return 0; /* something wrong happened */
}
