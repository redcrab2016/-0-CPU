package backslash.bs5.asm;

public class BS5MemoryCell_ccc_f_noimm extends BS5MemoryCell {
    private String ccc;
    private String f;
    private int oooo_oooo_oooo;
    protected BS5MemoryCell_ccc_f_noimm(BS5program prg, int addr, 
                                              int linenum,String ccc, String f) throws BS5Exception {
        super(prg, addr,linenum);
        this.ccc = ccc;
        this.f = f;
        this.oooo_oooo_oooo = 0;
        this.isEvaluated = false;
    }
    
    public BS5MemoryCell_ccc_f_noimm setValue(int oooo_oooo_oooo) throws BS5Exception {
        this.oooo_oooo_oooo = oooo_oooo_oooo;
        super.setValue(getvalue_ccc(ccc) | getvalue_f(f) | (this.oooo_oooo_oooo & 0x0fff));
        return this;
    }

    private int getvalue_ccc(String ccc) throws BS5Exception {
        int result = -1;
        switch (ccc.toLowerCase()) {
            case "al": result = 0b000_0_0000_0000_0000; break;
            case "zs": result = 0b001_0_0000_0000_0000; break; 
            case "zc": result = 0b010_0_0000_0000_0000; break;
            case "cs": result = 0b011_0_0000_0000_0000; break;
            case "cc": result = 0b100_0_0000_0000_0000; break;
            case "xs": result = 0b101_0_0000_0000_0000; break;
            case "xc": result = 0b110_0_0000_0000_0000; break;
            case "ne": result = 0b111_0_0000_0000_0000; break;
            default: throw new BS5Exception("Incorrect conditionnal execution code '"+ccc+"' at line " + linenum);
        }
        return result;
    }

    private int getvalue_f(String f) throws BS5Exception {
        int result = -1;
        switch (f.toLowerCase()) {
            case "nf": result = 0b000_0_0000_0000_0000; break;
            case "fl": result = 0b000_1_0000_0000_0000; break; 
            default: throw new BS5Exception("Incorrect flag code '"+f+"' at line " + linenum);
        }
        return result;
    }
}
