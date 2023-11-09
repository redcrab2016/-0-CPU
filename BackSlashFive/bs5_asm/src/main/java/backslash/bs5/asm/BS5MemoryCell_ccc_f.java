package backslash.bs5.asm;

public class BS5MemoryCell_ccc_f extends BS5MemoryCell {
    private String ccc;
    private String f;
    private int oooo_oooo_oooo;
    private boolean isReadyToSetValue= false;
    protected BS5MemoryCell immediat;
    protected int immediat_mask;
    protected int immediat_shift;
    protected BS5MemoryCell_ccc_f(BS5program prg, int addr, 
                                              int linenum,String ccc, String f) throws BS5Exception {
        super(prg, addr,linenum);
        this.ccc = ccc==null?"al":ccc;
        this.f = f;
        this.oooo_oooo_oooo = 0;
        this.isEvaluated = false;
        immediat = null;
        isReadyToSetValue = true;
        setValue(oooo_oooo_oooo);
    }
    
    protected BS5MemoryCell_ccc_f setValue(int oooo_oooo_oooo) throws BS5Exception {
        if (!isReadyToSetValue) return this;
        this.oooo_oooo_oooo = oooo_oooo_oooo;
        if (immediat == null)
        {
            super.setValue(getvalue_ccc(ccc) | getvalue_f(f) | (this.oooo_oooo_oooo & 0x0fff));
        } else {
            isEvaluated = immediat.isEvaluated();
            if (isEvaluated) {
               super.setValue(  getvalue_ccc(ccc) | 
                                getvalue_f(f) | 
                                (this.oooo_oooo_oooo & 0x0fff) | 
                                ((immediat.getValue() & immediat_mask) << immediat_shift)); 
            }
        }
        return this;
    }

    public boolean isEvaluated() throws BS5Exception{
        if (immediat == null) return super.isEvaluated();
        if (!isEvaluated && immediat.isEvaluated()) setValue(oooo_oooo_oooo);
        return isEvaluated;
    }

    public int getValue() throws BS5Exception {
        if (immediat == null) return super.getValue();
        if (isEvaluated()) return super.getValue();
        setValue(oooo_oooo_oooo);
        return super.getValue(); 
    }
    
    protected BS5MemoryCell_ccc_f setImmediat(String imm, int mask, int shift) {
        immediat = new BS5MemoryCell(prg, this.addr, this.linenum, imm);
        immediat_mask = mask;
        immediat_shift = shift;
        return this;
    }

    protected int getvalue_reg(String reg) {
        if (reg.toUpperCase().charAt(0) != 'R') return Integer.parseInt("Not a register : "+ reg);
        return (Integer.parseInt(reg.substring(1))) & 0x0f;        
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
            default: throw new BS5Exception("Incorrect conditionnal execution code '" + ccc + "' at line " + linenum);
        }
        return result;
    }

    private int getvalue_f(String f) throws BS5Exception {
        int result = -1;
        switch (f.toLowerCase()) {
            case "nf": result = 0b000_0_0000_0000_0000; break;
            case "fl": result = 0b000_1_0000_0000_0000; break; 
            default: throw new BS5Exception("Incorrect flag code '" + f + "' at line " + linenum);
        }
        return result;
    }
}
