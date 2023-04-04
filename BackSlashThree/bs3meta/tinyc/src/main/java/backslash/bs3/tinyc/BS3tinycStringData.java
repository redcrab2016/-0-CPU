package backslash.bs3.tinyc;
import java.util.*;

public class BS3tinycStringData extends HashMap<String,String> {
    private int counter;
    public BS3tinycStringData() {
        super();
        counter = 0;
    }

    public BS3tinycStringData(int initialCapacity) {
        super(initialCapacity);
        counter = 0;
    }

    public BS3tinycStringData(Map<? extends String, ? extends String> m) {
        super(m);
        counter = 0;
    }

    public BS3tinycStringData(int initialCapacity, float loadFactor) {
        super(initialCapacity, loadFactor);
        counter = 0;
    }

    public BS3tinycStringData addString(String strValue) {
        if (!containsKey(strValue))
        {
            put(strValue, "__str"+String.valueOf(counter));
            counter++;
        }
        return this;
    }

    public String getBS3Label(String strValue) {
    
        return addString(strValue).get(strValue);
    }

    public List<Object> generateASM() {
        List<Object> result  = new ArrayList<Object>();
        String label;
        String value;
        int i;
        for (Entry<String,String> e : entrySet()) {
            label = e.getValue();
            value = e.getKey();
            if (value.length()>0)
            {
                String[] chunks = value.split("(?<=\\G.{" + 30 + "})");
                switch (chunks.length)
                {
                    case 1 : 
                        result.add(label + "   db \"" + value + "\", 0");
                        break;
                    default:
                        result.add(label + "   db \"" + chunks[0] +"\"");
                        for (i = 1; i < (chunks.length - 1) ; i++) {
                            result.add("   db \"" + chunks[i] +"\"");
                        }
                        result.add("   db \"" + chunks[chunks.length-1] +"\", 0");
                        break;
                }
            } else {
                result.add(label +"  db 0");
            }

        }
        return result;
    }

    
}
