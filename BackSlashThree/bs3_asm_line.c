#include <string.h>
#include "bs3_asm.h"
#include "bs3_asm_code_map.h"

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
  bs3_asm_code_map_reset(&bs3_asm_map);
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


char * bs3_asm_line_getFilename(struct bs3_asm_line * bs3line, char * filename)
{
  struct bs3_asm_line asmcursor;
  struct bs3_asm_line * pasmcursor;
  pasmcursor = &asmcursor;
  int i;
  int j;
  int k;
  int k2;
  i = bs3line->asmIndex;
  strcpy(filename, "<unknown file name>");
  for (j = i ; j >= 0 ; j--) 
  {
    if (bs3_asm_line_at(j, pasmcursor))
    {
      if (asmcursor.linenum      != 0) continue;
      if (asmcursor.label        >= 0 &&
          asmcursor.labelIsAlias == 0 &&
          asmcursor.ope          == -1)
      {
        for (i = strlen(asmcursor.line)+1; i < BS3_ASM_LINE_SIZE ; i++)
        {
          if (asmcursor.line[i] = ';' ) {
            strcpy(filename, &asmcursor.line[i + 1]);
            for (k = 0; filename[k]; k++)
            {
              if (filename[k] < ' ') 
              {
                filename[k] = 0;
                break;
              }
            }
            return filename;
          }
        }
      }
      return filename;
    }
  }
  return filename;
}

struct bs3_asm_line * bs3_asm_line_atgloballabel(const char * label, struct bs3_asm_line * bs3lineLabel)
{
  long i;
  long l;
  l = bs3_asm_line_size();
  for (i = 0; i < l; i++)
  {
    if (bs3_asm_line_at(i, bs3lineLabel))
    {
      if (bs3lineLabel->label                                     >= 0   && 
          bs3lineLabel->labelIsAlias                              == 0   && 
          bs3lineLabel->line[bs3lineLabel->label]                 != '.' &&
          strcmp(label, &bs3lineLabel->line[bs3lineLabel->label]) == 0)
        return bs3lineLabel;
    }
  }
  return ((void *)0);
}

struct bs3_asm_line * bs3_asm_line_atlabel(const char * label, struct bs3_asm_line * bs3line, struct bs3_asm_line * bs3lineLabel)
{
  /* determine if it is local(".xxx") / global ("yyyy") or local by global ("yyyy.xxxx")*/
  char global_label[BS3_ASM_LINE_BUFFER];
  char local_label[BS3_ASM_LINE_BUFFER];
  int i;
  int j;
  int k;
  long l;
  l = bs3_asm_line_size();
  if (label[0] == '.') 
  {
    for (i=0; label[i]; i++) local_label[i] = label[i];
    local_label[i] = 0;
    /* search global label at or above bs3line */
    for (i = bs3line->asmIndex; i >= 0 ; i--)
    {
      if (bs3_asm_line_at(i, bs3lineLabel))
      {
        if (bs3lineLabel->label                      >= 0 &&
            bs3lineLabel->labelIsAlias               == 0 &&
            bs3lineLabel->line[bs3lineLabel->label]  != '.' )
        {
          /* then search local label attached to the global label and equals to local_label*/
          j = bs3lineLabel->asmIndex;
          for (k = j+1; k < l; k++)
          {
            if (bs3_asm_line_at(k, bs3lineLabel))
            {
              if (bs3lineLabel->label               >= 0 &&
                  bs3lineLabel->labelIsAlias        == 0)
              {    
                  if (bs3lineLabel->line[bs3lineLabel->label]  != '.') return ((void *)0);
                  if (strcmp( &bs3lineLabel->line[bs3lineLabel->label], local_label )) return bs3lineLabel;
              }
            }
            else return ((void *)0);
          }
        }
      }
    }
    return ((void *)0);
  } 
  else /* can be a global label or a local in context of a global */
  {
    /* capture the global label */
    for (i=0; label[i] && label[i] != '.'; i++) global_label[i] = label[i];
    global_label[i] = 0;
    if (label[i]) /* there is a local label designation*/
    {
      /* capture the local label designation */
      for (j=0; label[i]; i++, j++) local_label[j] = label[i];
      local_label[j]=0;
      /* search global label by global_label */
      if (bs3_asm_line_atgloballabel(global_label, bs3lineLabel))
      {
        /* then search local label attached to the global label and equals to local,_label */
          /* then search local label attached to the global label and equals to local_label*/
          j = bs3lineLabel->asmIndex;
          for (k = j+1; k < l; k++)
          {
            if (bs3_asm_line_at(k, bs3lineLabel))
            {
              if (bs3lineLabel->label               >= 0 &&
                  bs3lineLabel->labelIsAlias        == 0)
              {    
                  if (bs3lineLabel->line[bs3lineLabel->label]  != '.') return ((void *)0);
                  if (strcmp( &bs3lineLabel->line[bs3lineLabel->label], local_label )) return bs3lineLabel;
              }
            }
            else return ((void *)0);
          }

      }
      return ((void *)0);
    }
    else /* simply global label */
    {
      /* search global label by global label  */
      return bs3_asm_line_atgloballabel(global_label, bs3lineLabel);
    }
  } 
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
