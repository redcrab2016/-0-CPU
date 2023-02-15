#include <stdio.h>
#include <stdlib.h>
#include "bs3_type.h"
#include "bs3_asm_code_map.h"

static const char bs3_asm_code_map_binsignature[] = "CT4CJO12"; /* "BS3BIN01" with next character  */ 
static const char bs3_asm_code_map_hexsignature[] = "CT4IFY12"; /* "BS3HEX01" with next character  */ 
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
  [BS3_ASM_CODE_MAP_ERR_LOADOVERLAP]    = "Loading of BS3 files with overlap",
  [BS3_ASM_CODE_MAP_ERR_EMBEDBADTARGET] = "Failed to access target bs3 file to add an embed file",
  [BS3_ASM_CODE_MAP_ERR_BADEMBEDFILE]   = "Failed to access embed file",
  [BS3_ASM_CODE_MAP_ERR_BADMETABLOCK]   = "Unknown announce block (bad metablock)"
};


void bs3_asm_code_map_reset(struct bs3_asm_code_map * bs3codemap)
{
  long i;
  for (i = 0; i < 65536; i++)
  {
    bs3codemap->code[i]=0;
    bs3codemap->inUse[i] = 0;
  }
  if (bs3codemap->next)
  {
    bs3_asm_code_map_free(bs3codemap->next);
    bs3codemap->next = (struct bs3_asm_code_map *)0;
  }
}

int bs3_asm_code_map_embed(const char * filename, const char * embedfile, BYTE rombank, BYTE rambank, int codemaptype)
{
    int err;
    int i;
    FILE * outfile;
    FILE * infile;
    err = BS3_ASM_CODE_MAP_ERR_OK;
    if ( filename   == ((void *)0)  ||
         embedfile  == ((void *)0)     ) return BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
    /* test if filename and embedfile files do exist */
    infile = fopen(filename,"r");
    if (infile == 0) return BS3_ASM_CODE_MAP_ERR_EMBEDBADTARGET;
    fclose(infile);
    infile = fopen(embedfile,"rb");
    if (infile == 0) return BS3_ASM_CODE_MAP_ERR_BADEMBEDFILE;
    /* append embed announce block */
    outfile = fopen(filename,"ab"); /* open for appening data */
    switch (codemaptype)
    {
        case BS3_ASM_CODE_MAP_TYPE_BINARY:
            /* add signature */
            for (i = 0 ; bs3_asm_code_map_binsignature[i] ; i++) fputc(bs3_asm_code_map_binsignature[i]-1, outfile);
            /* address, length : announce data block signature */
            fputc(0xFF, outfile); /* 0xFFFF address low byte*/
            fputc(0xFF, outfile); /* 0xFFFF address high byte*/
            fputc(0x03, outfile); /* 0xFFFF length low byte */
            fputc(0x00, outfile); /* 0xFFFF length high byte */
            /* announce data block */
            fputc(0x00, outfile); /* 0 = rom and ram bank number */
            fputc((int)rombank,outfile); /* rom bank number */
            fputc((int)rambank,outfile); /* ram bank number */
            /* end block stream */
            fputc(0, outfile); /* zero address low byte*/
            fputc(0, outfile); /* zero address high byte*/
            fputc(0, outfile); /* zero length low byte */
            fputc(0, outfile); /* zero length high byte */
            break;
        case BS3_ASM_CODE_MAP_TYPE_HEXA:
            /* add signature */
            for (i = 0 ; bs3_asm_code_map_hexsignature[i] ; i++) fputc(bs3_asm_code_map_hexsignature[i]-1, outfile);
            fputc('\n', outfile);
            /* address, length : announce data block signature */
            fputc('F', outfile);
            fputc('F', outfile);
            fputc('F', outfile);
            fputc('F', outfile);
            fputc(',', outfile);
            fputc('0', outfile);
            fputc('0', outfile);
            fputc('0', outfile);
            fputc('3', outfile);
            fputc('\n',outfile);
            /* announce data block*/
            fputc('0', outfile);
            fputc('0', outfile); /* 0 = rom and ram bank number */
            fputc(hexa_digit[(rombank >> 4) & 0x0F ], outfile);
            fputc(hexa_digit[ rombank       & 0x0F ], outfile); /* rom bank number*/
            fputc(hexa_digit[(rambank >> 4) & 0x0F ], outfile);
            fputc(hexa_digit[ rambank       & 0x0F ], outfile); /* rom bank number*/
            fputc('\n', outfile);
            /* end block stream */
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
        default:
            err = BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
            break;
    }
    /* append embed file */
    while (! feof(infile))
    {
        i = fgetc(infile);
        if (i >-1) fputc(i,outfile);
    }
    /* end of append */
    fclose(outfile);
    fclose(infile);
    return err;
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
                        for (i = 0 ; bs3_asm_code_map_binsignature[i] ; i++) fputc(bs3_asm_code_map_binsignature[i]-1, outfile);
                        break;
                    case BS3_ASM_CODE_MAP_TYPE_HEXA:
                        for (i = 0 ; bs3_asm_code_map_hexsignature[i] ; i++) fputc(bs3_asm_code_map_hexsignature[i]-1, outfile);
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
                        fputc(hexa_digit[(lenBlock >> 12) & 0x0F], outfile); /* Fxxx hexa length */
                        fputc(hexa_digit[(lenBlock >> 8 ) & 0x0F], outfile); /* xFxx hexa length */
                        fputc(hexa_digit[(lenBlock >> 4 ) & 0x0F], outfile); /* xxFx hexa length */
                        fputc(hexa_digit[ lenBlock        & 0x0F], outfile); /* xxxF hexa length */
                        fputc('\n', outfile); /* line feed separator to hexa byte block */
                        /* write byte block in hexadecimal*/
                        lenBlock = 0;
                        while (startBlock < endBlock)
                        {
                            fputc(hexa_digit[(bs3codemap->code[startBlock] >> 4) & 0x0F ], outfile);
                            fputc(hexa_digit[ bs3codemap->code[startBlock]       & 0x0F ], outfile);
                            startBlock++;
                            lenBlock++;
                            if (lenBlock % 40 == 0)  fputc('\n', outfile);
                        }
                        if (lenBlock % 40 != 0) fputc('\n', outfile);
                        state = BS3_ASM_CODE_MAP_SAVE_STATE_SEARCHSTART; 
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


static int bs3_asm_code_map_load_(FILE * infile, struct bs3_asm_code_map * bs3codemap, int acceptOverlap)
{
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
    int foundSignatureOnce;
    BYTE rombank;
    BYTE rambank;
    foundSignatureOnce = 0;

    while (!feof(infile)) /* consume the file completly*/
    {
        /* check if it is a known file format */
        isBin = 1;
        isHex = 1;
        for (i =0 ; bs3_asm_code_map_binsignature[i] && bs3_asm_code_map_hexsignature[i] ; i++) 
        {
            achar = fgetc(infile);
            if (achar < 0 || (bs3_asm_code_map_binsignature[i] !=  (char)(achar+1) && bs3_asm_code_map_hexsignature[i] != (char)(achar+1))) 
            {
                if ((achar < 0) && (!foundSignatureOnce) ) return BS3_ASM_CODE_MAP_ERR_LOADBADFILE;
                if ((achar < 0) && (foundSignatureOnce) ) return BS3_ASM_CODE_MAP_ERR_OK;
                isBin = 1;
                isHex = 1;
                i = -1;
                continue;
            }
            if (isBin && (bs3_asm_code_map_binsignature[i] !=  (char)achar+1)) isBin = 0;
            if (isHex && (bs3_asm_code_map_hexsignature[i] !=  (char)achar+1)) isHex = 0;
        }

        /* from here the file must be recognised as Binary or Hex : must be one or the other*/
        if (isBin && isHex) /* must never occu*/
        {
            return BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
        }
        foundSignatureOnce = 1;
        if (isBin)
        {
            isok    = 1;
            while (isok)
            {
                bs3codemap->rombank = rombank;
                bs3codemap->rambank = rambank;

                address = 0;
                length  = 0;
                /* load address*/
                achar   = fgetc(infile);
                isok    = isok && (achar != EOF);
                if (!isok) break; /* if EOF at address reading then consider it as ok */
                address = achar & 0x00FF;
                achar   = fgetc(infile);
                isok    = isok && (achar != EOF);
                address = address |(((WORD)(achar & 0x00FF)) << 8);
                /* load length */
                achar   = fgetc(infile);
                isok    = isok && (achar != EOF);
                length  = achar & 0x00FF;
                achar   = fgetc(infile);
                isok    = isok && (achar != EOF);
                length  = length | (((WORD)(achar & 0x00FF)) << 8);
                if (!isok) 
                {   
                    fclose(infile);
                    return BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
                }
                /* load byte block */
                if (length == 0 ) break; /* detect logical end of file */
                if (address == 65535 && length > 1) /* it is an announce block */
                {
                    switch(length)
                    {
                        case 3: /* should be (currently) rom/ram bank number annoucement */
                            achar = fgetc(infile); /* read meta block type : 1 of 3 bytes  */
                            isok = isok && (achar != EOF);
                            if (!isok)
                            {
                                return BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
                            }
                            switch (achar) /* Meta block type selector */
                            {
                                case 0: /* 0 = ram/rom bank number announcement*/
                                    /* get new rom bank number*/
                                    achar = fgetc(infile); /* 2 of 3 bytes */
                                    isok = isok && (achar != EOF);
                                    if (!isok)
                                    {
                                        return BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
                                    }       
                                    rombank = (BYTE)achar;
                                    /* get new ram bank number */
                                    achar = fgetc(infile); /* 3 of 3 bytes */
                                    isok = isok && (achar != EOF);
                                    if (!isok)
                                    {
                                        return BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
                                    }       
                                    rambank = (BYTE)achar;
                                    /* new code map */
                                    bs3codemap = bs3_asm_code_map_new_(bs3codemap);
                                    break;
                                default: 
                                    return BS3_ASM_CODE_MAP_ERR_BADMETABLOCK;
                            }
                            break;
                        default:
                            return BS3_ASM_CODE_MAP_ERR_BADMETABLOCK;
                    }
                }
                else
                {
                    while (length)
                    {
                        achar = fgetc(infile);
                        isok    = isok && (achar != EOF);
                        if (!isok) 
                        {   
                            return BS3_ASM_CODE_MAP_ERR_UNEXPECTEDEOF;
                        }
                        if (bs3codemap->inUse[address] && acceptOverlap == 0) 
                        {
                            return BS3_ASM_CODE_MAP_ERR_LOADOVERLAP;
                        }
                        bs3codemap->code[address] = (char)achar;
                        bs3codemap->inUse[address] = 1;
                        address++;
                        length--;
                    }
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
                bs3codemap->rombank = rombank;
                bs3codemap->rambank = rambank;                
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
                    case 'a' ... 'f':
                        achar -= 32;
                    case 'A' ... 'F':
                        achar -= 'A' ;
                        achar += 10;
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
                            if (address == 65535 && length > 1) state = BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK;
                        }   
                        break;
                    case BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK:
                        if (length != 3 ) return BS3_ASM_CODE_MAP_ERR_BADMETABLOCK;
                        if (i == 0)
                        {
                            abyte = 0;
                        }
                        abyte = (abyte << 4) | achar;
                        i++;
                        if (i == 2) /* 1 of 3 bytes */
                        {
                            i = 0;
                            switch(abyte)
                            {
                                case 0: /*rom/ram bank number */
                                    state = BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK_ROMBANK;
                                    break;
                                default:
                                    return BS3_ASM_CODE_MAP_ERR_BADMETABLOCK;
                            }
                        }
                        break;
                    case BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK_ROMBANK:
                        if (i == 0)
                        {
                            abyte = 0;
                        }
                        abyte = (abyte << 4) | achar;
                        i++;
                        if (i == 2) /* 2 of 3 bytes */
                        {
                            i = 0;
                            rombank = abyte;
                            state = BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK_RAMBANK;
                        }
                        break;
                    case BS3_ASM_CODE_MAP_LOAD_STATE_METABLOCK_RAMBANK:
                        if (i == 0)
                        {
                            abyte = 0;
                        }
                        abyte = (abyte << 4) | achar;
                        i++;
                        if (i == 2) /* 3 of 3 bytes */
                        {
                            i = 0;
                            rambank = abyte;
                            /* new code map */
                            bs3codemap = bs3_asm_code_map_new_(bs3codemap);
                            state = BS3_ASM_CODE_MAP_LOAD_STATE_GETADDRESS;
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
    } /* end of of while not EOF */
    return err;
}

/**
 * load an hexa or binary bs3 file,
 * multiple load is allowed as long as there is no overlap of memory block (acceptOverlap)
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
    err = bs3_asm_code_map_load_(infile, bs3codemap, acceptOverlap);
    fclose(infile);
    return err;
}

int bs3_asm_code_map_isvalid(const char * filename)
{
    struct bs3_asm_code_map * temp;
    int result;
    temp = bs3_asm_code_map_new();
    if (temp == 0) return BS3_ASM_CODE_MAP_ERR_UNEXPECTED;
    result = bs3_asm_code_map_load(filename,temp,0);
    bs3_asm_code_map_free(temp);
    return result;
}


struct bs3_asm_code_map * bs3_asm_code_map_new_(struct bs3_asm_code_map * current )
{
    struct bs3_asm_code_map * newmap;
    newmap =  (struct bs3_asm_code_map *)calloc(1,sizeof(struct bs3_asm_code_map));
    if (current)
    {
        current->next = newmap;
    }
    newmap->dynamic=1; /* indicat that object has been created dynamically */
    newmap->next = (struct bs3_asm_code_map *)0; /* be sure there is no next map */
    return newmap;
}


struct bs3_asm_code_map * bs3_asm_code_map_new()
{
    return bs3_asm_code_map_new_(0);
}

void bs3_asm_code_map_free(struct bs3_asm_code_map * bs3codemap)
{
     struct bs3_asm_code_map * nextmap;
    while (bs3codemap != 0)
    {
        nextmap = bs3codemap->next;
        if (bs3codemap->dynamic) free(bs3codemap);
        bs3codemap = nextmap;
    }
}