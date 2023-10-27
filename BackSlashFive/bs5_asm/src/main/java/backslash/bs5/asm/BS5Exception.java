package backslash.bs5.asm;

public class BS5Exception extends Exception {
    public BS5Exception(String message) {
        super(message);
    }    
    public BS5Exception(String message, Throwable other ) {
        super(message, other);
    }    
}
