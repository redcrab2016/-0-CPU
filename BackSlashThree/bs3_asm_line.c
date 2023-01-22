#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <libgen.h>
#include "bs3_asm.h"
#include "bs3_asm_code_map.h"


#define BS3_MAX_MACROFILE 1024
#define BS3_FILENAME_LENGTH 256

struct bs3_asm_pass1_macrofile {
  char filename[BS3_FILENAME_LENGTH];
};
struct bs3_asm_pass1_macrofile bs3macrofiles[BS3_MAX_MACROFILE];
int bs3_asm_pass1_nbmacrofiles = 0;

struct bs3_asm_line bs3_asm[BS3_ASM_LINE_MAX]; /* to be managed as a sequential third party resource */
long bs3_asm_nbline = 0; /* current size of bs3_asm usage */
struct bs3_asm_code_map  bs3_asm_map; /* assembling memory map*/


/*
  search existing macrofile
*/
int  bs3_asm_pass1_getindex(const char * macrofilename)
{
  int result = -1;
  int i = 0;
  if (macrofilename == ((void *)0)) return result;
  if (macrofilename[0] == 0) return result;
  for (i = 0; i < bs3_asm_pass1_nbmacrofiles ; i++)
  {
    if (strcmp(macrofilename,bs3macrofiles[i].filename ) == 0)
    {
      result = i;
      break;
    }
  }
  return result;
}

/* add a macro filename in our list of macro filename*/
/* return index in macro filename list */
/* return -1 if macro filename list is full */
int bs3_asm_pass1_addmacro(const char * macrofilename)
{
  int result;
  result = bs3_asm_pass1_getindex(macrofilename);
  if (result >=0 ) return result; /* already exist */
  if (bs3_asm_pass1_nbmacrofiles >= BS3_MAX_MACROFILE) return -1; /* macro filename list is full */
  strcpy(bs3macrofiles[bs3_asm_pass1_nbmacrofiles++].filename, macrofilename);
  result = bs3_asm_pass1_nbmacrofiles-1;
  return result;
}

/* remove all macro files */
void bs3_asm_pass1_removemacrofiles()
{
  int i;
  struct stat buffer;
  for (i = 0 ; i < bs3_asm_pass1_nbmacrofiles; i++)
  {
    /* if file exist */
    if (stat(bs3macrofiles[i].filename, &buffer) != 0) 
    {
      fprintf(stderr, "Macro file %s does not exist anymore\n", bs3macrofiles[i].filename);
      continue;
    }
    /* file does exist then remove it */
    if (remove(bs3macrofiles[i].filename) != 0)
    {
      fprintf(stderr, "Failed to remove macro file %s\n errno:%d", bs3macrofiles[i].filename, errno);
    }
  }
  bs3_asm_pass1_nbmacrofiles = 0;
}

/*
  reset all asm line contents
*/
void bs3_asm_line_reset()
{
  long i;
  bs3_asm_nbline = 0;
  bs3_asm_pass1_removemacrofiles();
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
  return dest;
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
  get the asm line zeo based index asm line 
*/
struct bs3_asm_line * bs3_asm_line_at(long index, struct bs3_asm_line * bs3line)
{
  if (index > bs3_asm_nbline-1 || index < 0 ) return ((void *)0);
  return bs3_asm_line_copy(bs3line, bs3_asm + index);
}

/*
 Stringify the parsed asm line
 filename linenumber [label] ope param,... ; 0000 00 00 00 
*/
static const char digit_hexa[]="0123456789ABCDEF";

char * bs3_asm_line_tostring(struct bs3_asm_line * bs3line, char * stringasmline)
{
  int i;
  BYTE assembly;
  char filename[255];
  if (stringasmline == ((void *)0)) return ((void *)0);
  if (bs3line == ((void *)0)) 
  {
    *stringasmline = 0;
    return stringasmline;
  }
  /* generate '[filename:linenum:addrhexa] label ope param1,param2 ... ; hexa generated'*/
  /*      [filename:linenum:addrhexa] label ope ... */
  sprintf(stringasmline,"[%-32s:%0.5hd:%0.4hX] %s\t%s",
          basename(bs3_asm_line_getFilename (bs3line, filename)),
          (int)(bs3line->linenum),
          bs3line->assemblyAddress,
          ((bs3line->label == -1)?"":&bs3line->line[bs3line->label]),
          ((bs3line->ope == -1)?"":&bs3line->line[bs3line->ope])
          );
  /*      param1,param2 ... */
  for (i = 0 ; i < bs3line->nbParam ; i++)
  {
    if (i == 0)
    {
      strcat(stringasmline, "\t");
    }
    else
    {
      strcat(stringasmline, ", ");
    }
    strcat(stringasmline, &bs3line->line[bs3line->param[i]]);
  } 
  /* if there is some assembly */
  for (i = 0 ; i < bs3line->assemblyLength ; i++)
  {
    if (i == 0)
    {
      strcat(stringasmline, "\t; ");
    }
    else
    {
      strcat(stringasmline, " ");
    }
    strncat(stringasmline,&digit_hexa[(bs3line->assembly[i] >> 4) & 0x0F], 1); 
    strncat(stringasmline,&digit_hexa[bs3line->assembly[i] & 0x0F], 1); 
  }

  return stringasmline; 
}

char * bs3_asm_line_getFilename(struct bs3_asm_line * bs3line, char * filename)
{
  struct bs3_asm_line asmcursor;
  struct bs3_asm_line * pasmcursor;
  pasmcursor = &asmcursor;
  int i;
  int k;
  i = bs3line->asmIndex;
  strcpy(filename, "<unknown file name>");

  if (bs3_asm_line_at(bs3line->fileasmindex, pasmcursor))
  {
    if (asmcursor.label        >= 0 &&
        asmcursor.labelIsAlias == 0 &&
        asmcursor.ope          == -1)
    {
      for (i = strlen(asmcursor.line)+1; i < BS3_ASM_LINE_SIZE ; i++)
      {
        if (asmcursor.line[i] == ';' ) {
          strcpy(filename, &asmcursor.line[i + 1]);
          for (k = 0; filename[k]; k++)
          {
            if (filename[k] < ' ') 
            {
              filename[k] = 0;
              break;
            }
          }
        }
      }
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
  char currFilename[BS3_ASM_LINE_SIZE];
  char parentFilename[BS3_ASM_LINE_SIZE];

  l = bs3_asm_line_size();
  if (label[0] == '.') /* local label case */
  {
    for (i=0; label[i]; i++) local_label[i] = label[i];
    local_label[i] = 0;
    /* search global label at or above bs3line */
    for (i = bs3line->asmIndex; i >= 0 ; i--)
    {
      if (bs3_asm_line_at(i, bs3lineLabel))
      {
        bs3_asm_line_getFilename(bs3line, currFilename);
        bs3_asm_line_getFilename(bs3lineLabel, parentFilename); 
        if (bs3lineLabel->label                      >= 0   &&
            bs3lineLabel->labelIsAlias               == 0   &&
            bs3lineLabel->line[bs3lineLabel->label]  != '.' &&
            strcmp(currFilename, parentFilename) == 0 )
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
                  //if (bs3lineLabel->line[bs3lineLabel->label]  != '.') return ((void *)0);
                  bs3_asm_line_getFilename(bs3lineLabel, parentFilename); 
                  if (strcmp( &bs3lineLabel->line[bs3lineLabel->label], local_label ) == 0 && /* same label name */
                      strcmp(currFilename, parentFilename)                            == 0 ) //&& /* same file(prog/include/macro) name */
                      //bs3lineLabel->asmIndexToGlobalLabel == j ) 
                  {
                    return bs3lineLabel;
                  }
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
      bs3_asm_line_getFilename(bs3line, parentFilename);
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
              bs3_asm_line_getFilename(bs3lineLabel, currFilename);
              if (bs3lineLabel->label               >= 0 &&
                  bs3lineLabel->labelIsAlias        == 0)
              {    
                  if (bs3lineLabel->line[bs3lineLabel->label]  != '.'                         &&
                      strcmp( currFilename,                             parentFilename) == 0) 
                      return ((void *)0);
                  if (strcmp( &bs3lineLabel->line[bs3lineLabel->label], local_label   ) == 0  &&
                      strcmp( currFilename,                             parentFilename) == 0 ) 
                      return bs3lineLabel;
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
