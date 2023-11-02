package backslash.bs5.asm;

// valid also for oooo_siii_iiii
public class BS5MemoryCell_oooo_iiii_iiii extends BS5MemoryCell_ccc_f {
    public BS5MemoryCell_oooo_iiii_iiii(BS5program prg, int addr, int linenum, String ccc, String f, 
    String mnemonic,  String immediat) throws BS5Exception {
       super(prg,addr,linenum,ccc,f);
       this.setImmediat(immediat, 0x0ff, 0);
       this.setValue(getvalue_instruction(mnemonic,linenum) );
    }

    private int getvalue_instruction(String mnemonic, int linenum) throws BS5Exception {
        int result = -1;
        switch (mnemonic) {
            case "mov low R0, imm8":    result = 0b0111_0000_0000; break;
            case "mov high R0, imm8":   result = 0b1000_0000_0000; break;
            case "add R15, simm8":      result = 0b1001_0000_0000; break;
            default: throw new BS5Exception("Incorrect instruction '" + mnemonic + "' at line " + linenum); // Doh! Must never occur.
        }
        return result;
        }
    
}

/*
ccc f 0111 iiii iiii
	mov low R0, imm8
ccc f 1000 iiii iiii
	mov high R0, imm8
ccc f 1001 siii iiii
	add R15, simm8
 */