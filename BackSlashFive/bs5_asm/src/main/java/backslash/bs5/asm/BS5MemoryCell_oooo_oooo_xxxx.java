package backslash.bs5.asm;

public class BS5MemoryCell_oooo_oooo_xxxx extends BS5MemoryCell_ccc_f {
    public BS5MemoryCell_oooo_oooo_xxxx (BS5program prg, int addr, int linenum, String ccc, String f, 
    String mnemonic,  String rx) throws BS5Exception {
       super(prg,addr,linenum,ccc,f);
       this.setValue(getvalue_instruction(mnemonic,linenum) | getvalue_reg(rx));
    }

    private int getvalue_instruction(String mnemonic, int linenum) throws BS5Exception {
    int result = -1;
    switch (mnemonic) {
        case "add Rx, 1":               result = 0b1111_0100_0000; break;
        case "sub Rx, 1":               result = 0b1111_0101_0000; break;
        case "mov low R0, low Rx":      result = 0b1111_0110_0000; break;
        case "mov low R0, high Rx":     result = 0b1111_0111_0000; break;
        case "mov high R0, low Rx":     result = 0b1111_1000_0000; break;
        case "mov high R0, high Rx":    result = 0b1111_1001_0000; break;
        case "mov low Rx, 0":           result = 0b1111_1010_0000; break;
        case "mov high Rx, 0":          result = 0b1111_1011_0000; break;
        case "mov Rx, 0":               result = 0b1111_1100_0000; break;
        case "not Rx":                  result = 0b1111_1101_0000; break;
        case "and R0, Rx":              result = 0b1111_1110_0000; break;
        case "or R0, Rx":               result = 0b1111_1111_0000; break;
        default: throw new BS5Exception("Incorrect instruction '" + mnemonic + "' at line " + linenum);// Doh! Must never occur.
    }
    return result;
    }

}

/*
ccc f 1111 0100 xxxx
	add Rx, 1

ccc f 1111 0101 xxxx
	sub Rx, 1

ccc f 1111 0110 xxxx
	mov low R0, low Rx

ccc f 1111 0111 xxxx
	mov low R0, high Rx

ccc f 1111 1000 xxxx
	mov high R0, low Rx

ccc f 1111 1001 xxxx
	mov high R0, high Rx

ccc f 1111 1010 xxxx
	mov low Rx, 0

ccc f 1111 1011 xxxx
	mov high Rx, 0

ccc f 1111 1100 xxxx
	mov Rx, 0

ccc f 1111 1101 xxxx
	not Rx

ccc f 1111 1110 xxxx
	and R0, Rx

ccc f 1111 1111 xxxx
	or  R0, Rx
 */

