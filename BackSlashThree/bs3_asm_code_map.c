#include <stdio.h>
#include "bs3_type.h"
#include "bs3_asm_code_map.h"


static const char bs3_asm_code_map_binsignature[] = "BS3BIN01"; 
static const char bs3_asm_code_map_hexsignature[] = "BS3HEX01"; 
static const char * hexa_digit="0123456789ABCDEF";

const char * bs3_asm_code_map_message[]=
{
  [BS3_ASM_CODE_MAP_ERR_OK]             = "Ok",
  [BS3_ASM_CODE_MAP_ERR_UNEXPECTED]     = "Unhandle error in code map",
  [BS3_ASM_CODE_MAP_ERR_FILECREATE]     = "Failed to create code map file",
  [BS3_ASM_CODE_MAP_ERR_BADSAVECALL]    = "Incorrect call to code map save function",
  [BS3_ASM_CODE_MAP_ERR_BADLOADCALL]    = "Incorrect call to code map load function",
  [BS3_ASM_CODE_MAP_ERR_LOADFILE]       = "Fail to open file for code loading",
  [BS3_ASM_CODE_MAP_ERR_LOADBADFILE]    = "Not a BS3 loadable file",
  [BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF]  = "Unexpected end of file during BS3 file loading",
  [BS3_ASM_CODE_MAP_ERR_LOADOVERLAP]    = "Loading of BS3 files with overlap" 
};


void bs3_asm_code_map_reset(struct bs3_asm_code_map * bs3codemap)
{
  long i;
  for (i = 0; i < 65536; i++)
  {
    bs3codemap->code[i]=0;
    bs3codemap->inUse[i] = 0;
  }
}

int bs3_asm_code_map_save(const char * filename ,struct bs3_asm_code_map * bs3codemap, int codemaptype )
{
    long startBlock;
    long endBlock;
    long lenBlock;
    char state;
    char isOk;
    int i;
    FILE * outfile;
    int err;
    err = BS3_ASM_CODE_MAP_ERR_OK;
    /* check if input parameters looks ok )*/
    if (     filename    == ((void *)0)                  ||
             filename[0] == 0                            ||
             bs3codemap  == ((void *)0)                  ||
            (codemaptype != BS3_ASM_CODE_MAP_TYPE_BINARY && 
             codemaptype != BS3_ASM_CODE_MAP_TYPE_HEXA )
        ) return BS3_ASM_CODE_MAP_ERR_BADSAVECALL;
    outfile = ((void *)0);
    state = BS3_ASM_CODE_MAP_SAVE_STATE_INIT;
    isOk = 1;

    while (isOk)
    {
        switch (state)
        {
            case BS3_ASM_CODE_MAP_SAVE_STATE_INIT:
                if (outfile != ((void *) 0)) 
                {
                    err = BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
                    isOk = 0;
                    break;
                }
                /* create a new file */
                outfile = fopen(filename,"wb");
                /* test if file creation succeed */
                if (outfile == ((void *)0)) 
                {
                    err = BS3_ASM_CODE_MAP_ERR_FILECREATE;
                    isOk = 0;
                    break;
                }
                /* insert signature */
                switch (codemaptype)
                {
                    case BS3_ASM_CODE_MAP_TYPE_BINARY:
                        for (i = 0 ; bs3_asm_code_map_binsignature[i] ; i++) fputc(bs3_asm_code_map_binsignature[i], outfile);
                        break;
                    case BS3_ASM_CODE_MAP_TYPE_HEXA:
                        for (i = 0 ; bs3_asm_code_map_hexsignature[i] ; i++) fputc(bs3_asm_code_map_binsignature[i], outfile);
                        fputc('\n', outfile);
                        break;
                    default:
                        err = BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
                        isOk = 0;
                    break;
                }
                state      = BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHSTART;
                startBlock = 0;
                endBlock   = 0;
                lenBlock   = 0;
                break;
            case BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHSTART:
                while (startBlock <= 65535 && bs3codemap->inUse[startBlock] == 0 ) startBlock++;
                if (startBlock == 65536)
                {
                    state = BS3_ASM_CODE_MAP_SAVE_STATE_END;
                    break;
                }
                state = BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHEND;
                endBlock = startBlock;
                break;
            case BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHEND:
                while (endBlock <= 65535 && bs3codemap->inUse[endBlock] != 0 ) endBlock++;
                state = BS3_ASM_CODE_MAP_SAVE_STATE_TRANSFER;
                break;
            case BS3_ASM_CODE_MAP_SAVE_STATE_TRANSFER:
                lenBlock = endBlock - startBlock;
                switch (codemaptype)
                {
                    case BS3_ASM_CODE_MAP_TYPE_BINARY:
                        /* write address and length */
                        fputc((unsigned char)(startBlock & 0x00FF), outfile); /* low byte address */
                        fputc((unsigned char)((startBlock>> 8) & 0x00FF), outfile); /* high byte address */
                        fputc((unsigned char)(lenBlock & 0x00FF), outfile); /* low byte length */
                        fputc((unsigned char)((lenBlock>> 8) & 0x00FF), outfile); /* high byte address */
                        /* write byte block*/
                        while (startBlock < endBlock) fputc(bs3codemap->code[startBlock++], outfile);
                        state = BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHSTART;
                        break;
                    case BS3_ASM_CODE_MAP_TYPE_HEXA:
                        /* write address and length in hexadecimal */
                        fputc(hexa_digit[(startBlock >> 12) & 0x0F], outfile); /* Fxxx hexa address */
                        fputc(hexa_digit[(startBlock >> 8 ) & 0x0F], outfile); /* xFxx hexa address */
                        fputc(hexa_digit[(startBlock >> 4 ) & 0x0F], outfile); /* xxFx hexa address */
                        fputc(hexa_digit[ startBlock        & 0x0F], outfile); /* xxxF hexa address */
                        fputc(',', outfile); /* comma separator */
                        fputc(hexa_digit[(startBlock >> 12) & 0x0F], outfile); /* Fxxx hexa length */
                        fputc(hexa_digit[(startBlock >> 8 ) & 0x0F], outfile); /* xFxx hexa length */
                        fputc(hexa_digit[(startBlock >> 4 ) & 0x0F], outfile); /* xxFx hexa length */
                        fputc(hexa_digit[ startBlock        & 0x0F], outfile); /* xxxF hexa length */
                        fputc('\n', outfile); /* line feed separator to hexa byte block */
                        /* write byte block in hexadecimal*/
                        while (startBlock < endBlock)
                        {
                            fputc(hexa_digit[(bs3codemap->code[startBlock] >> 4) & 0x0F ], outfile);
                            fputc(hexa_digit[ bs3codemap->code[startBlock]       & 0x0F ], outfile);
                            startBlock++;
                            if (lenBlock % 72 == 0)  fputc('\n', outfile);
                            lenBlock--;
                        }
                        fputc('\n', outfile); 
                        break;
                    default:
                        err = BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
                        isOk = 0;
                    break;
                }
                break;
        case BS3_ASM_CODE_MAP_SAVE_STATE_END:
                switch (codemaptype)
                {
                    case BS3_ASM_CODE_MAP_TYPE_BINARY:
                        fputc(0, outfile); /* zero address low byte*/
                        fputc(0, outfile); /* zero address high byte*/
                        fputc(0, outfile); /* zero length low byte */
                        fputc(0, outfile); /* zero length high byte */
                        break;
                    case BS3_ASM_CODE_MAP_TYPE_HEXA:
                        fputc('0', outfile); /* first hexa address  */
                        fputc('0', outfile); /* second hexa address */
                        fputc('0', outfile); /* third hexa address  */
                        fputc('0', outfile); /* fourth hexa address */
                        fputc(',', outfile); /* comma separator between address and length*/
                        fputc('0', outfile); /* first hexa length  */
                        fputc('0', outfile); /* second hexa length */
                        fputc('0', outfile); /* third hexa length  */
                        fputc('0', outfile); /* fourth hexa length */
                        fputc('\n', outfile); /* block separator ... for end of data*/
                        break;
                }
                fclose(outfile);
                isOk = 0;
                outfile = ((void *)0);
                break;
        default:
                break;
        }
    }
    return err;
}

/**
 * load an hexa or binary bs3 file,
 * multiple load is allowed as long as there is no overlap of memory block
*/
int bs3_asm_code_map_load(const char * filename ,struct bs3_asm_code_map * bs3codemap, int acceptOverlap)
{
    FILE * infile;
    int i;
    int err = BS3_ASM_CODE_MAP_ERR_OK;
    int achar;
    int isBin;
    int isHex;
    long address;
    long length;
    int abyte;
    int isok;
    int state;

    /* check if input parameters looks ok )*/
    if (     filename    == ((void *)0)                  ||
             filename[0] == 0                            ||
             bs3codemap  == ((void *)0)                  
        ) return BS3_ASM_CODE_MAP_ERR_BADLOADCALL;
    infile = fopen(filename, "rb");
    if (infile == ((void *)0)) return BS3_ASM_CODE_MAP_ERR_LOADFILE;

    /* check if it is a known file format */
    isBin = 1;
    isHex = 1;
    for (i =0 ; bs3_asm_code_map_binsignature[i] && bs3_asm_code_map_hexsignature[i] ; i++) 
    {
        achar = fgetc(infile);
        if (achar < 0 || (bs3_asm_code_map_binsignature[i] !=  (char)achar && bs3_asm_code_map_hexsignature[i] != (char)achar)) 
        {
            fclose(infile);
            return BS3_ASM_CODE_MAP_ERR_LOADBADFILE;
        }
        if (isBin && bs3_asm_code_map_binsignature[i] !=  (char)achar) isBin = 0;
        if (isHex && bs3_asm_code_map_hexsignature[i] !=  (char)achar) isHex = 0;
    }

    /* from here the file must be recognised as Binary or Hex : must be one or the other*/
    if (isBin && isHex) /* must never occu*/
    {
        fclose(infile);
        return BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
    }
    if (isBin)
    {
        isok    = 1;
        while (isok)
        {
            address = 0;
            length  = 0;
            /* load address*/
            achar   = fgetc(infile);
            isok    = isok && (achar != EOF);
            if (!isok) break; /* if EOF at address reading then consider it as ok */
            address = achar & 0x00FF;
            achar   = fgetc(infile);
            isok    = isok && (achar != EOF);
            address = (address << 8) | (achar & 0x00FF);
            /* load length */
            achar   = fgetc(infile);
            isok    = isok && (achar != EOF);
            length  = achar & 0x00FF;
            achar   = fgetc(infile);
            isok    = isok && (achar != EOF);
            length  = (length << 8) | (achar & 0x00FF);
            if (!isok) 
            {   
                fclose(infile);
                return BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
            }
            /* load byte block */
            if (length == 0 ) break; /* detect logical end of file */
            while (length)
            {
                achar = fgetc(infile);
                isok    = isok && (achar != EOF);
                if (!isok) 
                {   
                    fclose(infile);
                    return BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
                }
                if (bs3codemap->inUse[address] && acceptOverlap == 0) 
                {
                    fclose(infile);
                    return BS3_ASM_CODE_MAP_ERR_LOADOVERLAP;
                }
                bs3codemap->code[address] = (char)achar;
                bs3codemap->inUse[address] = 1;
                address++;
                length--;
            }
        }
        
    }
    if (isHex)
    {
        i = 0;
        isok    = 1;
        state   = BS3_ASM_CODE_MAP_LOAD_STATE_GETADDRESS;
        while (isok)
        {
            /* read a byte en hexa
              take two hexa digit et get the byte value, other character are ignotred */
            achar = fgetc(infile);
            isok = isok && (achar != EOF);
            if (!isok) break;
            switch (achar)
            {   
                case '0' ... '9':
                    achar -= '0';
                    break;
                case 'a' ... 'z':
                    achar -= 32;
                case 'A' ... 'Z':
                    achar -= 'A' +  10;
                    break;
                default:
                    achar = 255;
                    break;
            }
            if (achar == 255) continue;

            switch(state)
            {
                case BS3_ASM_CODE_MAP_LOAD_STATE_GETADDRESS:
                    if (i == 0) 
                    { 
                        address = 0;
                    }
                    address =  (address << 4) | achar;
                    i++;
                    if (i == 4) 
                    {
                        i = 0;
                        state = BS3_ASM_CODE_MAP_LOAD_STATE_GETLENGTH;
                    }
                    break;
                case BS3_ASM_CODE_MAP_LOAD_STATE_GETLENGTH:
                    if (i == 0)
                    {
                        length = 0;
                    }
                    length = (length << 4) | achar;
                    i++;
                    if (i == 4)
                    {
                        i = 0;
                        if (length == 0) 
                        {
                            state = BS3_ASM_CODE_MAP_LOAD_STATE_END;
                            isok = 0;
                            break;
                        }
                        state = BS3_ASM_CODE_MAP_LOAD_STATE_GETBLOCK;
                    }   
                    break;
                case BS3_ASM_CODE_MAP_LOAD_STATE_GETBLOCK:
                    if (i == 0)
                    {
                        abyte = 0;
                    }
                    abyte = (abyte << 4) | achar;
                    i++;
                    if (i == 2)
                    {
                        i = 0;
                        if (bs3codemap->inUse[address] && acceptOverlap == 0) 
                        {
                            fclose(infile);
                            return BS3_ASM_CODE_MAP_ERR_LOADOVERLAP;
                        }                        
                        bs3codemap->code[address] = (char)(abyte & 0x00FF);
                        bs3codemap->inUse[address] = 1;
                        length--;
                        if (length == 0) state = BS3_ASM_CODE_MAP_LOAD_STATE_GETADDRESS;
                        address++;
                    }
                    break;
            }
        }
        if (feof(infile) && state != BS3_ASM_CODE_MAP_LOAD_STATE_END) err = BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
    }
    fclose(infile);
    return err;
}