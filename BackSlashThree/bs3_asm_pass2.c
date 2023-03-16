#include "bs3_asm.h"

/**
 * Set label address in assembly
 * at the same time search for unknown label
*/
int bs3_asm_pass2()
{
    struct bs3_asm_line bs3line;
    struct bs3_asm_line * pbs3line;
    struct bs3_asm_line bs3lineLabel;
    struct bs3_asm_line * pbs3lineLabel;
    char label[BS3_ASM_LINE_BUFFER];
    char filename[BS3_ASM_LINE_BUFFER];
    char c;
    int length;
    int i;
    int j;
    int k;
    int l;
    long addr;
    int err = BS3_ASM_PASS1_PARSE_ERR_OK;
    pbs3line = &bs3line;
    pbs3lineLabel = &bs3lineLabel;
    length = bs3_asm_line_size();
    /* for each bs3_asm_line that has parameter and does have assembly generated */
    for (i = 0 ; i < length; i++)
    {
        if (bs3_asm_line_at(i, pbs3line) == ((void *)0))
        {
            err = BS3_ASM_PASS2_ERR_UNEXPECTED;
            bs3_asm_report("<PASS 2>",0 ,0 ,err);
            break;
        }
        if (bs3line.assemblyLength == 0 || bs3line.nbParam == 0) continue; /* no code generated or no parameter, then go next asmline*/

        /* If there is a parameter of type LABEL or M0LABEL*/
        for (j = 0 ; j < bs3line.nbParam ; j++)
        {
            switch (bs3line.paramType[j])
            {
                case BS3_ASM_PARAM_TYPE_LABEL:
                case BS3_ASM_PARAM_TYPE_M0LABEL:
                    /* capture the label to search for*/
                    k = bs3line.param[j];
                    l = 0; 
                    while (bs3line.line[k]) {
                        switch(bs3line.line[k])
                        {
                            case '[':
                            case ']':
                                break;
                            default:
                                label[l++] = bs3line.line[k];
                        }
                        k++;
                    }
                    label[l] = 0;
                    /* Find the address of the label */
                    if (bs3_asm_line_atlabel(label, pbs3line, pbs3lineLabel) == ((void *)0)) /* label not found */
                    {
                        err = BS3_ASM_PASS2_ERR_LABELNOTFOUND; /* not found error */
                        bs3_asm_report_msg(bs3_asm_line_getFilename(pbs3line, filename), bs3line.linenum , bs3line.param[j] , err, label);
                        break;
                    }
                    /* compute address of update thanks to asmpattern */
                    if (bs3line.opeCode == BS3_INSTR_DW)  /* may contains several label designation */
                    {
                        /* address computation is param index * 2 */
                        /* write the label address value in assembly */
                        bs3line.assembly[ j * 2     ] =  (BYTE)(bs3lineLabel.assemblyAddress       & 0x00FF); /* label address low byte */
                        bs3line.assembly[ j * 2 + 1 ] = (BYTE)((bs3lineLabel.assemblyAddress >> 8) & 0x00FF); /* label address high byte */
                    }
                    else /* other instruction with a maximum of 1 label designation */ 
                    {
                        for (k = 0; c = bs3Instr[bs3line.opeCode].asmpattern[k]; k++)
                        {
                            switch(c)
                            {
                                case 'r':
                                    addr = (long)bs3lineLabel.assemblyAddress - ((long)bs3line.assemblyAddress + bs3line.assemblyLength);
                                    if (addr < -128 || addr > 127) 
                                    {
                                        err = BS3_ASM_PASS2_ERR_LABEL2FAR;
                                        bs3_asm_report(bs3_asm_line_getFilename(pbs3line, filename), bs3line.linenum , bs3line.param[j] , err);
                                        break;
                                    }
                                    bs3line.assembly[k] = (BYTE)(addr & 0x00FF);
                                    break;
                                case 'R':
                                    addr = (long)bs3lineLabel.assemblyAddress - ((long)bs3line.assemblyAddress + bs3line.assemblyLength);
                                    if (addr < -32768 || addr > 32767) 
                                    {
                                        err = BS3_ASM_PASS2_ERR_LABEL2FAR;
                                        bs3_asm_report(bs3_asm_line_getFilename(pbs3line, filename), bs3line.linenum , bs3line.param[j] , err);
                                        break;
                                    }
                                    bs3line.assembly[k] = (BYTE)(addr & 0x00FF);
                                    bs3line.assembly[k+1] = (BYTE)((addr >> 8) & 0x00FF);
                                    break;
                                case 'w':
                                    bs3line.assembly[k] = (BYTE)(bs3lineLabel.assemblyAddress & 0x00FF);
                                    bs3line.assembly[k+1] = (BYTE)((bs3lineLabel.assemblyAddress >> 8) & 0x00FF);
                                    break;
                                default:
                                    break;
                            }
                            if (err != BS3_ASM_PASS1_PARSE_ERR_OK) break;
                        }
                        j = bs3line.nbParam; /* to end the param browsing: only one label parameter for non DW*/
                    }
                    break;
                default:
                    continue;
            }
            if (err != BS3_ASM_PASS1_PARSE_ERR_OK) break;
        } /* end of for each param */
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK) break;
        /* write the value in asm_line and in bs3_asm_map */
        err = bs3_asm_line_commit(pbs3line);
        if (err != BS3_ASM_PASS1_PARSE_ERR_OK) bs3_asm_report(bs3_asm_line_getFilename(pbs3line, filename), bs3line.linenum , 1 , err);
    } /* end of for each asm line */
    return err;
}