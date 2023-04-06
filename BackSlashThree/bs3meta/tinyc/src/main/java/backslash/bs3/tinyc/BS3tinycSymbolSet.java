package backslash.bs3.tinyc;
import java.util.*;

public class BS3tinycSymbolSet extends HashMap<String, BS3tinycSymbol> {

    public BS3tinycSymbolSet() {
        super();
    }

    public BS3tinycSymbolSet(int initialCapacity) {
        super(initialCapacity);
    }

    public BS3tinycSymbolSet(Map<? extends String, ? extends BS3tinycSymbol> m) {
        super(m);
    }

    public BS3tinycSymbolSet(int initialCapacity, float loadFactor) {
        super(initialCapacity, loadFactor);
    }
    
    public boolean add(String name, BS3tinycType aType, boolean isExtern, String size)
    {
        if (containsKey(name)) return false;
        BS3tinycSymbol s = new  BS3tinycSymbol(name,aType, isExtern,size);
        put(name, s);
        return true;
    }

    public boolean add(String name, BS3tinycType aType, boolean isExtern)
    {
        return add(name, aType, isExtern, "1");
    }

    public boolean has(String name) {
        return containsKey(name);
    }

}
