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

    List<Object> generateASM() {
        List<Object> result = new ArrayList<Object>();
        for (Entry<String, BS3tinycSymbol> e : entrySet()) {
            BS3tinycSymbol s =  e.getValue();
            if (s.type != null && (s.type.isWord || s.type.isByte) && !s.type.isLabel && !s.isExtern) {
                if (s.type.isWord) {
                    result.add(s.name + "    dw  0");
                    if (s.size > 1)
                        result.add("    space   "+String.valueOf((s.size-1)*2));
                } else {
                    result.add(s.name + "   db  0");
                    if (s.size > 1)
                        result.add("    space   "+String.valueOf(   (s.size % 2) == 1? s.size: s.size-1   ));
                }

            }
        }
        return result;
    }

}
