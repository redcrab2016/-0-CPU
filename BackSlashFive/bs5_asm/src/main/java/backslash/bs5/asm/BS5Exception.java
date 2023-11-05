package backslash.bs5.asm;

public class BS5Exception extends Exception {
    public int linenum;
    public int PC;

    public BS5Exception(String message) {
        super(message);
        linenum = 0;
        PC = 0;

    }    
    public BS5Exception(String message, Throwable other ) {
        super(message, other);
        linenum = 0;
        PC = 0;
    }    
}
