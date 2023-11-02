package backslash.bs5.asm;

public class BS5MemoryCell_oooo_oooo_iiii extends BS5MemoryCell_ccc_f {
    public BS5MemoryCell_oooo_oooo_iiii(BS5program prg, int addr, int linenum, String ccc, String f, 
    String mnemonic,  String immediat) throws BS5Exception {
       super(prg,addr,linenum,ccc,f);
       this.setImmediat(immediat, 0x00f, 0);
       this.setValue(getvalue_instruction(mnemonic,linenum) );
    }

    private int getvalue_instruction(String mnemonic, int linenum) throws BS5Exception {
        int result = -1;
        switch (mnemonic) {
            case "add R0, imm4":    result = 0b1111_0000_0000; break;
            case "sub R0, imm4":    result = 0b1111_0001_0000; break;
            case "shl R0, imm4":    result = 0b1111_0010_0000; break;
            case "shr R0, imm4":    result = 0b1111_0011_0000; break;
            default: throw new BS5Exception("Incorrect instruction '" + mnemonic + "' at line " + linenum);// Doh! Must never occur.
        }
        return result;
        }
    
}
/*
ccc f 1111 0000 iiii
	add R0, imm4

ccc f 1111 0001 iiii
	sub R0, imm4

ccc f 1111 0010 iiii
	shl R0, imm4

ccc f 1111 0011 iiii
	shr R0, imm4
 */