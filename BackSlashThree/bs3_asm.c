#include <stdio.h>
#include "bs3_asm.h"

int bs3_asm_file( const char * filename, const char * filenameout, const char * filenamereport, int format)
{
  int err;
  long l;
  int i;
  struct bs3_asm_line bs3line;
  WORD addressout;
  FILE * freport;
  char reportline[BS3_ASM_LINE_BUFFER*3];
  err = BS3_ASM_PASS1_PARSE_ERR_OK; 
  /* Init asm line set */
  bs3_asm_line_reset();
  /* pass 1 */
  err =  bs3_asm_pass1_file(filename, 0 /* adress 0x0000 by default */,&addressout, -1 /* not macro expansion mode*/);
  bs3_asm_pass1_removemacrofiles();
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
