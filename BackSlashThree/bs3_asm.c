#include <stdio.h>
#include <string.h>
#include "bs3_asm.h"

struct bs3_asm_include_paths * bs3_asm_includepaths = 0;

int bs3_asm_file( const char * filename, const char * filenameout, const char * filenamereport, int format, const char * tmppath,  int keepTmpfile)
{
  int err;
  long l;
  int i;
  struct bs3_asm_line bs3line;
  WORD addressout;
  FILE * freport;
  char reportline[BS3_ASM_LINE_BUFFER*3];
  err = BS3_ASM_PASS1_PARSE_ERR_OK;
  if (tmppath != ((void *)0) )  
    strcpy(bs3_asm_tmppath,tmppath);
  else
    strcpy(bs3_asm_tmppath, "/tmp");
  /* Init asm line set */
  bs3_asm_line_reset();
  /* pass 1 */
  err =  bs3_asm_pass1_file(filename, 0 /* adress 0x0000 by default */,&addressout, -1 /* not macro expansion mode*/);
  if (!keepTmpfile) bs3_asm_pass1_removemacrofiles();
  if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
  {
    bs3_asm_report(filename, 0 , 0 , BS3_ASM_PASS1_FAILURE) ;
    return BS3_ASM_PASS1_FAILURE;
  }
/* pass 2 : not true pass 2 : reuse memory model bs3_asm_line*/
  err = bs3_asm_pass2();
  if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
  {
    bs3_asm_report(filename, 0 , 0 , BS3_ASM_PASS2_FAILURE) ;
    return BS3_ASM_PASS2_FAILURE;
  }

/* Generate binary file */
  err = bs3_asm_code_map_save(filenameout ,&bs3_asm_map, format );
  if (err == BS3_ASM_CODE_MAP_ERR_OK)
  {
    for (i= 0; i < bs3_asm_embedlist.size; i++) /* add embed file */
    {
      err = bs3_asm_code_map_embed( filenameout, 
                                    bs3_asm_embedlist.embed[i].embedfile,
                                    bs3_asm_embedlist.embed[i].rombank,
                                    bs3_asm_embedlist.embed[i].rambank,
                                    format);
      if (err != BS3_ASM_CODE_MAP_ERR_OK)
      {
        fprintf(stderr, "Failed to embed file %s for rom bank #%d ram bank#%d : error %s\n",
                bs3_asm_embedlist.embed[i].embedfile, 
                (int)bs3_asm_embedlist.embed[i].rombank, 
                (int)bs3_asm_embedlist.embed[i].rambank,
                bs3_asm_code_map_message[err]);
        break;
      }
    }
  }
  if (err == BS3_ASM_CODE_MAP_ERR_OK) /* if ok then  generate the report */
  {
    freport = fopen(filenamereport, "wt");
    l = bs3_asm_line_size();
    for (i = 0 ; i < l ; i++)
    {
      if (bs3_asm_line_at(i, &bs3line))
      {
          bs3_asm_line_tostring(&bs3line, reportline);
          fprintf(freport, "%s\n", reportline);
      }
    }
    fclose(freport);
  }
return err;
}
