package backslash.bs5.asm;
import java.util.*;

public class BS5program {
    private int PC;
    private int linenum;
    private Map<Integer,BS5MemoryCell> bs5memoryMap;
    private Map<String,BS5Label> bs5Labels;
    private int memorySize;
    private String currentGlobalLabel;

    public BS5program() throws BS5Exception {
        this(65536);
    }

    public BS5program(int memorySize) throws BS5Exception {
        PC = 0;
        linenum = 0;
        this.memorySize = memorySize>65536?65536:(memorySize<256?256:memorySize);
        bs5memoryMap = new HashMap<Integer,BS5MemoryCell>();
        bs5Labels = new HashMap<String,BS5Label>();
        currentGlobalLabel=null;
        addLabel("__GLOBAL__");
    }

// does addr is correct (in range) ,if no then exception, if yes: does the memory is already mapped to a value ?
    private boolean isMemoryMapped(int addr) throws BS5Exception {
        if (addr < 0 || addr >= memorySize) throw new BS5Exception("Out of range memory address (program too large ?)"); 
        return bs5memoryMap.containsKey(addr);
    }

    public BS5program incrementLine() {
        linenum++;
        return this;
    }

    // if return -1 then label not found
    public int findLabelAddress(String label, int linenum) {
        BS5Label labelObj;
        if (label.charAt(0) != '.') { // global label, get it directly
            labelObj = bs5Labels.get(label);
            return labelObj==null?-1:labelObj.getAddr(); // found global label address
        } else { // local label
            for (String labelKey: bs5Labels.keySet()) { // search each label entry
                if (labelKey.contains(label)) {  // if the label contains the local label
                    labelObj = bs5Labels.get(labelKey); // check if it is in line range
                    if (    labelObj.getName().equals(label) &&
                            labelObj.getLinenum() <= linenum &&
                            labelObj.getLinenumend() >= linenum ) {
                        return labelObj.getAddr(); // found local label address
                    }
                }
            }
        }
        return -1; // not fiound
    }

//  Add label
    public BS5program addLabel(String label) throws BS5Exception {
        if (label.charAt(0) != '.') {  // global label
            if (bs5Labels.containsKey(label)) throw new BS5Exception("Duplicate label definition '" + label + "'");
            bs5Labels.put(label, new BS5Label(label, PC, linenum));
            if (currentGlobalLabel != null) {
                bs5Labels.get(currentGlobalLabel).setLinenumend(linenum-1);
            }
            currentGlobalLabel = label;
        } else { // local label
            String localGlobal = currentGlobalLabel + label; // local label named globally
            if (bs5Labels.containsKey(localGlobal)) throw new BS5Exception("Duplicate local label definition '" + label + "' for global label '" + currentGlobalLabel + "'");
            bs5Labels.put(localGlobal, new BS5Label( bs5Labels.get(currentGlobalLabel),label, PC, linenum));           
        }
        return this;
    }

// Base 16 bits value added into memory map
    public BS5program addWord(String word) throws BS5Exception {
        if (isMemoryMapped(PC)) throw new BS5Exception("Memory address " + String.format("0x%04x", PC) + " is already encoded with a value.");
        bs5memoryMap.put(PC, new BS5MemoryCell(this,PC,linenum,word));
        PC++;
        return this;
    }

// Directive "org"
    public BS5program org(String addr) throws BS5Exception {
        PC = BS5MemoryCell.get16bitsNumeral(this,PC,linenum,addr);
        return this;
    }

// Directive raw word data "dw"
    public BS5program dw(List<String> dw_list) throws BS5Exception {
        for (String dw: dw_list) addWord(dw);
        return this;
    }
    
}
