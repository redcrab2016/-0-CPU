#include "bs3_asm.h"

int bs3_asm_file( const char * filename)
{
  int err;
  err = 0; /* TODO : assign correct default global success */
  bs3_asm_line_reset();
  err =  bs3_asm_pass1_file(filename, 0 /* adress 0x0000 by default */,-1 /* not macro expansion mode*/);
  if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
  {
    bs3_asm_report(filename, 0 , 0 , BS3_ASM_PASS1_FAILURE) ;
    return BS3_ASM_PASS1_FAILURE;
  }
/* invoke pass 2 */
  err = bs3_asm_pass2();
  if (err != BS3_ASM_PASS1_PARSE_ERR_OK) 
  {
    bs3_asm_report(filename, 0 , 0 , BS3_ASM_PASS2_FAILURE) ;
    return BS3_ASM_PASS2_FAILURE;
  }

/* TODO : if pass 2 ok, then generate binary file */
return err;
}
/*
TODO:
DONE : check if a label is already defined (duplicate label) inside bs3_asm_pass1_oneline
DONE : local label attached to global label
DONE : accept local label fully qualified global.local in parameter 
directive DIST...
missing ALIGN directive
DONE : detect if assembly code at adress overlap an existing generated code
DONE : macro recording
DONE : macro expansion
DONE : pass 2 : complete assembly with label address (abs and relative )


*/
