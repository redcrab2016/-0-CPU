#include "bs3_asm.h"
#include "bs3_debug.h"

char linebuffer[1024];

void main() {
  struct bs3_registers reg;
  char line[120];
  WORD start = 0x1FFE;
  int i =0;
  struct bs3_asm_code_map codemap;
  
  if (bs3_asm_file("test.asm", "test.out.2","test.report.txt",1) == BS3_ASM_PASS1_PARSE_ERR_OK)
  {
    bs3_asm_code_map_reset(&codemap);
    bs3_asm_code_map_load("test.out.2",&codemap,0);
    bs3_debug_prepare(0); /* 0 for default port 35853*/
    bs3_hyper_main(&codemap,&bs3_debug);
    bs3_debug_end();

  }
  return;
 
}
