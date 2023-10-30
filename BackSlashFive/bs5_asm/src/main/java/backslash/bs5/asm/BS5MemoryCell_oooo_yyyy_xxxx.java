package backslash.bs5.asm;

public class BS5MemoryCell_oooo_yyyy_xxxx extends BS5MemoryCell_ccc_f_noimm {
    public BS5MemoryCell_oooo_yyyy_xxxx(BS5program prg, int addr, int linenum, String ccc, String f, 
         String mnemonic, String ry, String rx) throws BS5Exception {
            super(prg,addr,linenum,ccc,f);
            this.setValue(getvalue_instruction(mnemonic,linenum) | (getvalue_reg(ry)<<4) | getvalue_reg(rx));
         }

    private int getvalue_instruction(String mnemonic, int linenum) throws BS5Exception {
        int result = -1;
        switch (mnemonic) {
            case "mov [Rx], Ry":    result = 0b0000_0000_0000; break;
            case "mov Rx, [Ry]":    result = 0b0001_0000_0000; break;
            case "add Rx, [Ry]":    result = 0b0010_0000_0000; break;
            case "sub Rx, [Ry]":    result = 0b0011_0000_0000; break;
            case "mov Rx, Ry":      result = 0b0100_0000_0000; break;
            case "add Rx, Ry":      result = 0b0101_0000_0000; break;
            case "sub Rx, Ry":      result = 0b0110_0000_0000; break;
            default: throw new BS5Exception("Incorrect instruction '" + mnemonic + "' at line " + linenum);
        }
        return result;
    }

    private int getvalue_reg(String reg) {
        return (Integer.parseInt(reg.substring(1))) & 0x0f;        
    }
}
/*2 registers parameters 

ccc f 0000 yyyy xxxx
	mov [Rx], Ry

ccc f 0001 yyyy xxxx
	mov Rx, [Ry] 

ccc f 0010 yyyy xxxx
	add Rx, [Ry]

ccc f 0011 yyyy xxxx
	sub Rx, [Ry]

ccc f 0100 yyyy xxxx
	mov Rx, Ry

ccc f 0101 yyyy xxxx
	add Rx, Ry

ccc f 0110 yyyy xxxx
	sub Rx, Ry
 */
