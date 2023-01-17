#if defined(BS3RUNTIME)

#include <stdlib.h>
#include <stdio.h>
#include "bs3.h"
int main(int argc, char *argv[]) 
{
  int i, err;
  struct bs3_asm_code_map codemap;
  bs3_asm_code_map_reset(&codemap);
  for (i = 0 ; i < argc; i++)
  {
    if ((err = bs3_asm_code_map_load(argv[i],&codemap,0)) != BS3_ASM_CODE_MAP_ERR_OK)
    {
      if (err == BS3_ASM_CODE_MAP_ERR_LOADBADFILE && (i == (argc -1)))
      {
        fprintf(stderr,"Error on loading %s : %s\n",argv[i],bs3_asm_code_map_message[err]);
        return 1;
      }
    }
  }
  bs3_hyper_main(&codemap,((void *)0));
  return 0;
}
#else

#include "bs3_asm.h"
#include "bs3_debug.h"

int main(int argc, char *argv[]) 
{
  struct bs3_asm_code_map codemap;
  
  if (bs3_asm_file("test.asm", "test.out.2","test.report.txt", BS3_ASM_CODE_MAP_TYPE_HEXA ) == BS3_ASM_PASS1_PARSE_ERR_OK)
  {
    bs3_asm_code_map_reset(&codemap);
    bs3_asm_code_map_load("test.out.2",&codemap,0);
    bs3_debug_prepare(0); /* 0 for default port 35853*/
    bs3_hyper_main(&codemap,&bs3_debug);
    bs3_debug_end();
  }
  return 0;
 
}
#endif