package backslash.bs5.asm;

public class BS5MemoryCell {
    private int value16bits;
    protected String sourceValue;
    protected boolean isEvaluated;
    protected BS5program prg;
    protected int linenum;
    protected int addr;

    public BS5MemoryCell(BS5program prg, int addr, int linenum) {
        this.addr = addr;
        this.prg = prg;
        this.linenum = linenum;
        setValue(0);
        isEvaluated = false;
    }

    public BS5MemoryCell(BS5program prg, int addr, int linenum, String sourceValue) {
         this.addr = addr;
        this.prg = prg;
        this.linenum = linenum;
        setValue(sourceValue);
    }

    public BS5MemoryCell(BS5program prg, int addr, int linenum, int sourceValue) {
        this.addr = addr;
        this.prg = prg;
        this.linenum = linenum;
        setValue(sourceValue);
    }

    public void setValue(int sourceValue) {
        this.sourceValue = String.valueOf(sourceValue);
        value16bits = sourceValue;
        isEvaluated = true;
    }

    public void setValue(String sourceValue) {
        this.sourceValue = sourceValue;
        try {
            value16bits = get16bitsNumeral(prg,addr,linenum,sourceValue);
            isEvaluated = true;

        } catch (BS5Exception e) {
            value16bits = 0;
            isEvaluated = false;
        }
    }

    public boolean isEvaluated(){
        return isEvaluated;
    }

    public int getValue() {
        if (isEvaluated) return value16bits;
        setValue(sourceValue);
        return value16bits; 
    }

    public String getSourceValue() {
        return sourceValue;
    }

    public int getAddr() { return this.addr; }
    public int getLinenum() { return this.linenum;}

    public static int get16bitsNumeral(BS5program prg,int addr,int linenum,String numeral) throws BS5Exception {
        String modifier="";
        String toEvaluate="";
        int idx = numeral.lastIndexOf(":");
        int result = 0;
        // by default
        modifier ="";
        toEvaluate = numeral;
        // does a modifier exist
        if (idx == 0) { // Doh ! Weird: this must never happened
            throw new BS5Exception("can't intepret the numeral '"+ numeral + "' with ':' at beginning.");
        }
        if (idx > 0) {
            modifier = numeral.substring(0, idx);  // get modifiers without last ':'
            toEvaluate = numeral.substring(idx+1);
        }
        try {
            if (toEvaluate.charAt(0) == '#')    toEvaluate  = toEvaluate.substring(1);
            if (toEvaluate.charAt(0) == '\'')   result      = toEvaluate.charAt(1);
            else                                result      = Integer.decode(toEvaluate).intValue(); 
        } catch (Exception e) {
            result = prg.findLabelAddress(toEvaluate, linenum);
            if (result == -1) {
                throw new BS5Exception("Failed to convert in numeral symbol address '" + String.valueOf(toEvaluate) + "'",e);
            }
        }
        // modifier action
        String [] mods = modifier.split(":");
        // reverse order of modifier execution
        for (int i = mods.length-1; i >=0 ; i--) {
            String mod = mods[i];
            switch (mod) {
                case "L8": result = result & 0x00FF;break; // low 8 bits of 16 bits value
                case "H8": result = (result & 0x0FF00) >> 8; break; // high 8 bits of 16 bits value
                case "O8": 
                    result = result - addr -1 ;
                    if (result > 127 || result < -128) throw new BS5Exception("Too far address to compute 8 bits(-128 to +127) signed offset addr '"  + toEvaluate +  "' - "+addr +"-1 = " + result + " at line "+ linenum);
                    result = result & 0x00FF;
                    break;
            }
        }
        // after modifer action(s): final result.
        return result;
    }
}
