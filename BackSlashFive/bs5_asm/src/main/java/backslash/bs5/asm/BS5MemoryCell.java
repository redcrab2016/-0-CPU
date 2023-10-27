package backslash.bs5.asm;

public class BS5MemoryCell {
    private int value16bits;
    private String sourceValue;
    private boolean isEvaluated;
    protected BS5program prg;

    public BS5MemoryCell(BS5program prg) {
        setValue(0);
    }

    public BS5MemoryCell(BS5program prg,String sourceValue) {
        this.prg = prg;
        setValue(sourceValue);
    }

    public BS5MemoryCell(BS5program prg,int sourceValue) {
        this.prg = prg;
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
            value16bits = get16bitsNumeral(sourceValue);
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
        
    }

    public String getSourceValue() {
        return sourceValue;
    }

    public static int get16bitsNumeral(String numeral) throws BS5Exception {
        try {
            if (numeral.charAt(0) == '#'){
                numeral = numeral.substring(1);
            }
            return Integer.decode(numeral).intValue(); 
        } catch (Exception e) {
            throw new BS5Exception("Failed to convert in numeral '" + String.valueOf(numeral) + "'",e);
        }
    }
}
