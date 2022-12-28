#include "bs3_asm.h"

struct bs3_asm_line bs3_asm[BS3_ASM_LINE_MAX]; /* to be managed as a sequential third party resource */
long bs3_asm_nbline = 0; /* current size of bs3_asm usage */
struct bs3_asm_code_map  bs3_asm_map; /* assembling memory map*/
/*
  reset all asm line contents
*/
void bs3_asm_line_reset()
{
  long i;
  bs3_asm_nbline = 0;
  for (i = 0; i < 65536; i++)
  {
    bs3_asm_map.code[i]=0;
    bs3_asm_map.inUse[i] = 0;
  }
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

struct bs3_asm_line * bs3_asm_line_atlabel(const char * label, struct bs3_asm_line * bs3line)
{
  /* TODO : return the bs3_asm_line that is represented by label, in context of parameter bs3line */
}

/*
 get the last asm line 
*/
struct bs3_asm_line * bs3_asm_line_last(struct bs3_asm_line * bs3line)
{
  if (bs3_asm_nbline == 0) return ((void *)0);
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
  int i;
  if (bs3line == 0) return BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED;
  currIndex = bs3line->asmIndex;
  if (currIndex == bs3_asm_nbline) 
  {
    /* check overlap in memory map: generated code overlap another generated code*/
    for (i = bs3line->assemblyAddress; i < bs3line->assemblyAddress +  bs3line->assemblyLength ; i++)
    {
      if (bs3_asm_map.inUse[i]) return BS3_ASM_PASS1_PARSE_ERR_ASMOVERLAP;
      bs3_asm_map.inUse[i] = 1;
      bs3_asm_map.code[i] = bs3line->assembly[i-bs3line->assemblyAddress];
    }
    /* save new line */
    bs3_asm_line_copy(bs3_asm + bs3_asm_nbline, bs3line);
    bs3_asm_nbline ++;
    
    return BS3_ASM_PASS1_PARSE_ERR_OK;
  }
  if (currIndex < bs3_asm_nbline && currIndex >= 0)
  {
    /* Update the memory map */
    for (i = bs3line->assemblyAddress; i < bs3line->assemblyAddress +  bs3line->assemblyLength ; i++)
    {
      /* memory map location must be already used : otherwise ... bug somewhere */
      if (bs3_asm_map.inUse[i] == 0) return BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED;
      bs3_asm_map.inUse[i] = 1;
      bs3_asm_map.code[i] = bs3line->assembly[i-bs3line->assemblyAddress];
    }
    /* update the line: no file then nothing to do .. index in bs3_asm[currIndex].asmIndex */
    bs3_asm_line_copy(bs3_asm + currIndex, bs3line);
    return BS3_ASM_PASS1_PARSE_ERR_OK; 
  } 
  return BS3_ASM_PASS1_PARSE_ERR_UNEXPECTED; /* something wrong happened */
}
