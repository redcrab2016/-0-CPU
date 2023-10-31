package backslash.bs5.asm;

public class BS5MemoryCell_oooo_iiii_xxxx extends BS5MemoryCell_ccc_f {
    public BS5MemoryCell_oooo_iiii_xxxx(BS5program prg, int addr, int linenum, String ccc, String f, 
    String mnemonic,  String rx, String immediat) throws BS5Exception {
       super(prg,addr,linenum,ccc,f);
       this.setImmediat(immediat, 0x00f, 4);
       this.setValue(getvalue_instruction(mnemonic,linenum) | getvalue_reg(rx) );
    }

    private int getvalue_instruction(String mnemonic, int linenum) throws BS5Exception {
        int result = -1;
        switch (mnemonic) {
            case "mov C, Rx:imm4":  result = 0b1010_0000_0000; break;
            case "mov Rx:imm4, C":  result = 0b1011_0000_0000; break;
            case "mov Rx:imm4, 0":  result = 0b1100_0000_0000; break;
            case "mov Rx:imm4, 1":  result = 0b1101_0000_0000; break;
            case "not Rx:imm4":     result = 0b1110_0000_0000; break;
            default: throw new BS5Exception("Incorrect instruction '" + mnemonic + "' at line " + linenum);
        }
        return result;
    }

}

/* 
ccc f 1010 iiii xxxx
	mov C, Rx:imm4

ccc f 1011 iiii xxxx
	mov Rx:imm4, C

ccc f 1100 iiii xxxx
	mov Rx:imm4, 0

ccc f 1101 iiii xxxx
	mov Rx:imm4, 1

ccc f 1110 iiii xxxx
	not Rx:imm4
 

 */