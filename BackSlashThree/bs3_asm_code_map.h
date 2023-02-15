
#include "bs3_type.h"

#ifndef _BS3_ASM_CODE_MAP_H
#define _BS3_ASM_CODE_MAP_H 1

#define BS3_ASM_CODE_MAP_TYPE_BINARY            0x00
#define BS3_ASM_CODE_MAP_TYPE_HEXA              0x01

#define BS3_ASM_CODE_MAP_SAVE_STATE_INIT        0x00
#define BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHSTART 0x01
#define BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHEND   0x02
#define BS3_ASM_CODE_MAP_SAVE_STATE_TRANSFER    0x03
#define BS3_ASM_CODE_MAP_SAVE_STATE_END         0x0F

#define BS3_ASM_CODE_MAP_LOAD_STATE_INIT              0x00
#define BS3_ASM_CODE_MAP_LOAD_STATE_GETADDRESS        0x01
#define BS3_ASM_CODE_MAP_LOAD_STATE_GETLENGTH         0x02
#define BS3_ASM_CODE_MAP_LOAD_STATE_GETBLOCK          0x03
#define BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK         0x04
#define BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK_ROMBANK 0x05
#define BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK_RAMBANK 0x06
#define BS3_ASM_CODE_MAP_LOAD_STATE_END               0x07

#define BS3_ASM_CODE_MAP_ERR_OK                 0x00
#define BS3_ASM_CODE_MAP_ERR_UNEXPECTED         0x01
#define BS3_ASM_CODE_MAP_ERR_FILECREATE         0x02
#define BS3_ASM_CODE_MAP_ERR_BADSAVECALL        0x03
#define BS3_ASM_CODE_MAP_ERR_BADLOADCALL        0x04
#define BS3_ASM_CODE_MAP_ERR_LOADFILE           0x05
#define BS3_ASM_CODE_MAP_ERR_LOADBADFILE        0x06
#define BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF      0x07
#define BS3_ASM_CODE_MAP_ERR_LOADOVERLAP        0x08
#define BS3_ASM_CODE_MAP_ERR_EMBEDBADTARGET     0x09
#define BS3_ASM_CODE_MAP_ERR_BADEMBEDFILE       0x0A
#define BS3_ASM_CODE_MAP_ERR_BADMETABLOCK       0x0B

struct bs3_asm_code_map
{
  BYTE code[65536];
  BYTE inUse[65536];
  BYTE rombank;
  BYTE rambank;
  struct bs3_asm_code_map * next; /* reference to the next code map */
  int dynamic; /* != 0 means structure created by bs3_asm_code_map_new() function */
};

extern const char * bs3_asm_code_map_message[];
int bs3_asm_code_map_load(const char * filename ,struct bs3_asm_code_map * bs3codemap, int acceptOverlap);
int bs3_asm_code_map_save(const char * filename ,struct bs3_asm_code_map * bs3codemap, int codemaptype );
int bs3_asm_code_map_embed(const char * filename, const char * embedfile, BYTE rombank, BYTE rambank, int codemaptype);
void bs3_asm_code_map_reset(struct bs3_asm_code_map * bs3codemap);
int bs3_asm_code_map_isvalid(const char * filename);
struct bs3_asm_code_map * bs3_asm_code_map_new_(struct bs3_asm_code_map * current );
struct bs3_asm_code_map * bs3_asm_code_map_new();
void bs3_asm_code_map_free(struct bs3_asm_code_map * bs3codemap);
#endif