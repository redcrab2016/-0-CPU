package backslash.bs5.asm;
import java.io.OutputStream;
import java.io.PrintStream;
import java.util.*;
import java.util.Map.Entry;

// class mainly dedicated to asmParser (ANTLR4 generated class based on asmParser.g4), then be used for final result by BS5program
public class BS5program {
    // construction progress information
    private int PC;
    private int linenum;
    private int memorySize;
    private String currentGlobalLabel;
    private boolean isLastEvalDone;
    // report information
    private Map<Integer,BS5MemoryCell> bs5memoryMap;
    private Map<String,BS5Label> bs5Labels;
    private List<BS5Exception> exceptionLst;
    private Map<Integer, String> sourcecode;
    private String rootLabelName;

    public BS5program()  {
        this(65536);
    }

    public BS5program(int memorySize) {
        this.memorySize = memorySize>65536?65536:(memorySize<256?256:memorySize);
        reset();
    }

    public BS5program lastEval() {
        if (isLastEvalDone) return this;
        for (Entry<Integer,BS5MemoryCell> entry: bs5memoryMap.entrySet()) {
            try {
            if (!entry.getValue().isEvaluated()) {
                if (entry.getValue().lastException != null)  {
                    entry.getValue().lastException.PC = entry.getKey();
                    entry.getValue().lastException.linenum = entry.getValue().linenum;
                    throw entry.getValue().lastException;
                }
            }
            } catch (BS5Exception e) {
                    exceptionLst.add(e);
            }
        }
        isLastEvalDone = true;
        return this;
    }




    public BS5program reset() {
        PC = 0;
        linenum = 1;
        exceptionLst = new ArrayList<BS5Exception>();
        bs5memoryMap = new HashMap<Integer,BS5MemoryCell>();
        bs5Labels = new HashMap<String,BS5Label>();
        sourcecode = new HashMap<Integer,String>();
        currentGlobalLabel=null;
        isLastEvalDone = false;
        rootLabelName = "__GLOBAL__";
        addLabel(rootLabelName, true);
        return this;
    }

    public int getNbException() { return exceptionLst.size();}
    public List<BS5Exception> getExceptionList() { return exceptionLst;}

    public BS5program addSourceLine(String line) {
        line = line.stripTrailing();
        sourcecode.put(linenum, line);
        return this;
    }

    public BS5program report(OutputStream out) {
        try {
            lastEval(); // it does nothing if it is already done.
            PrintStream ps =  new PrintStream(out, true);
            if (getNbException() == 0) { // if program is ok (no error found)
                // program
                int address = 0;
                int minAddress = 0;
                int minIdx = -1;
                int idx = 0;
                int wordPerLine = 8; // number of word values to show per line in 'Assembly' column in "Program:" report section
                String codeline ="";
                List<BS5MemoryCell> lstCode;
                List<BS5MemoryCell> lstCodeSorted;
                List<String> lstCodeBlock;
                String lineFormat = "%1$04X | %2$-" + String.valueOf(wordPerLine * 5) + "s| %3$s\n";
                ps.println("Program:");
                ps.printf("Addr | %1$-" + String.valueOf(wordPerLine * 5) + "s| Line # | Parsed source code \n", "Assembly");
                for (int line=1; line <= linenum; line++) { // for each source code line
                    if (sourcecode.get(line)!=null) { // we have source code at line
                        codeline = String.format("%1$6d | %2$s", line, sourcecode.get(line));
                        lstCode = new ArrayList<BS5MemoryCell>();
                        // search memory cells corresponding to current code line
                        for (Entry<Integer,BS5MemoryCell> entry:bs5memoryMap.entrySet()) {
                            if (entry.getValue().getLinenum() == line) {
                                lstCode.add(entry.getValue());
                            }
                        }
                        // Sort the found memory cells at code line, by address
                        lstCodeSorted = new ArrayList<BS5MemoryCell>();
                        while (lstCode.size()>0) {
                            minAddress = 65536;
                            minIdx = -1;
                            idx = 0;
                            for (BS5MemoryCell mcell: lstCode) {
                                if (mcell.getAddr() < minAddress) {
                                    minIdx = idx;
                                    minAddress = mcell.getAddr();
                                }
                                idx++;
                            }
                            lstCodeSorted.add(lstCode.remove(minIdx));
                        }
                        // build word code string block
                        lstCodeBlock = new ArrayList<String>();
                        String aBlock = "";
                        idx = 0;
                        for (BS5MemoryCell mcell: lstCodeSorted) {
                            aBlock += String.format("%1$04X ", mcell.getValue());
                            idx++;
                            if (idx % wordPerLine == 0) {
                                lstCodeBlock.add(aBlock);
                                aBlock = "";
                            }
                        }
                        if (idx == 0) lstCodeBlock.add("");
                        if (idx != 0 && !aBlock.isEmpty()) lstCodeBlock.add(aBlock); 
                        // construct the line(s)
                        if (!lstCodeSorted.isEmpty()) { // if word code to show
                            //     1st line 
                            address = lstCodeSorted.get(0).getAddr();
                            ps.printf(lineFormat,
                                      address, 
                                      lstCodeBlock.get(0), 
                                      codeline);
                            //     possible other lines
                            for (int i = 1 ; i < lstCodeBlock.size() ; i++) {
                                address += wordPerLine;
                                ps.printf(lineFormat, 
                                        address,
                                        lstCodeBlock.get(i),
                                        "");                                
                            }
                            address = lstCodeSorted.get(0).getAddr() + lstCodeSorted.size();
                        } else { // no word code to show
                            ps.printf(lineFormat,
                                      address,
                                      "", 
                                      codeline);
                        }

                    }
                }
                // add new line
                ps.println("");
                if (bs5Labels.size() > 0) { // if there is label defined
                    boolean bFound = false;
                    // label addresses
                    ps.println("Label address:");
                    for (Entry<String,BS5Label> entry: bs5Labels.entrySet()) {
                        ps.printf(" %1$-40s : 0x%2$04X\n", entry.getKey(), entry.getValue().getAddr());
                        if (!entry.getValue().isUsed()) bFound = true;
                    }
                    if (bFound) {// found unused labels
                        // add new line
                        ps.println("");
                        // warning unused label
                        ps.println("Unused label:");
                        for (Entry<String,BS5Label> entry: bs5Labels.entrySet()) {
                            if (!entry.getValue().isUsed()) {
                                ps.printf(" %1$-40s : 0x%2$04X at line %3$d\n", entry.getKey(), entry.getValue().getAddr(), entry.getValue().getLinenum());
                                bFound = true;
                            }
                        }
                    }
                } else {
                    ps.println("Remark: There is no label defined in code");
                }
                // add new line
                ps.println("");
            } else { // semantic errors
                ps.println("Errors:");
                for (BS5Exception ex: exceptionLst) {
                    ps.printf(" Line %1$d : %2$s\n", ex.linenum, ex.getMessage());
                }
                // add new line
                ps.println("");
            }
            ps.flush();
            return this;
        } catch (Exception e) {
            System.err.println("FATAL: Report unhandled error:"+e.getMessage());
            e.printStackTrace();
            return this;
        }
    }

    public BS5program genVerilogReadmemh(OutputStream out) {
        lastEval(); // it does nothing if it is already done.
        PrintStream ps =  new PrintStream(out, true);
        if (getNbException() != 0) {
            ps.println("// Verilog readmemh content can't be generated due to error during the BS5 assembly.");
            return this;
        }
        if (bs5memoryMap.size() == 0) {
            ps.println("// Empty program: No BS5 assembly content.");
            return this;
        }
        ps.printf("// BS5 program containing %1$d words.\n", bs5memoryMap.size());
        int lastAddress = -1;
        int wordsOnLine = 0;
        int nbWordPerLine = 16; // number of word content per line
        int nbBlock = 0;
        BS5MemoryCell mcell;
        for (int addr= 0; addr <= memorySize ; addr++) {
            mcell = bs5memoryMap.get(addr);
            if (mcell != null) {
                // set address if needed
                if (lastAddress != addr) {
                    ps.printf("\n@%1$04X ",addr);
                    lastAddress = addr;
                    wordsOnLine = 1;
                    nbBlock++;
                }
                // write word content
                try {
                    ps.printf("%1$04X ", mcell.getValue());
                    wordsOnLine++;
                } catch (BS5Exception e) { // Oops! should never happen
                    ps.printf("\nUnexpected exception during the reading of memory cell value at address %1$04X : %2$s \n", addr, e.getMessage() );
                }
                // go to next line when there is enough word content written
                if (wordsOnLine == nbWordPerLine){
                    ps.println("");
                    wordsOnLine = 0;
                }
                lastAddress += 1;
            }
        }
        if (wordsOnLine != 0) ps.println("");
        ps.printf("// Program defined in %1$d part(s)\n",nbBlock);
        ps.flush();
        return this;
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
            if (labelObj != null) labelObj.useIt();
            return labelObj==null?-1:labelObj.getAddr(); // found global label address
        } else { // local label
            for (String labelKey: bs5Labels.keySet()) { // search each label entry
                if (labelKey.contains(label)) {  // if the label contains the local label
                    labelObj = bs5Labels.get(labelKey); // check if it is in line range
                    if (    labelObj.getName().equals(label) &&
                            labelObj.getLinenum() <= linenum &&
                            labelObj.getLinenumend() >= linenum ) {
                        labelObj.useIt();
                        return labelObj.getAddr(); // found local label address
                    }
                }
            }
        }
        return -1; // not found
    }

//  Add label (unused)
    public BS5program addLabel(String label) {
        return addLabel(label, false);
    }
// Add label (may be already in used)
    public BS5program addLabel(String label, boolean isused) {
        try {
            if (label.charAt(0) != '.') {  // global label
                if (bs5Labels.containsKey(label)) throw new BS5Exception("Duplicate label definition '" + label + "'");
                if (label.indexOf((int)'.') != -1) throw new BS5Exception("Can't define global label containing local label designation '" + label + "'");
                bs5Labels.put(label, new BS5Label(label, PC, linenum));
                if (currentGlobalLabel != null) {
                    bs5Labels.get(currentGlobalLabel).setLinenumend(linenum-1);
                }
                currentGlobalLabel = label;
            } else { // local label
                String localGlobal = currentGlobalLabel + label; // local label named globally
                if (bs5Labels.containsKey(localGlobal)) throw new BS5Exception("Duplicate local label definition '" + label + "' for global label '" + currentGlobalLabel + "'");
                bs5Labels.put(localGlobal, new BS5Label( bs5Labels.get(currentGlobalLabel),label, PC, linenum));
                label = localGlobal;
            }
            if (isused) bs5Labels.get(label).useIt();
        } catch (BS5Exception e) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        }
        return this;
    }

// Directive "org"
    public BS5program org(String addr) {
        try {
            PC = BS5MemoryCell.get16bitsNumeral(this,PC,linenum,addr);
        } catch (BS5Exception e) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        }
        return this;
    }

// Directive raw word data "dw"
    public BS5program dw(List<String> dw_list)  {
        for (String dw: dw_list) addWord(dw);
        return this;
    }

    private BS5program addMemoryCell(BS5MemoryCell mcell) {
        try {
            if (isMemoryMapped(PC)) throw new BS5Exception("Memory address " + String.format("0x%04x", PC) + " is already encoded with a value.");
            bs5memoryMap.put(PC, mcell);
            PC++;
        } catch (BS5Exception e) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        }
        return this;
    }

// Base 16 bits value added into memory map
    private BS5program addWord(String word) {
        return addMemoryCell(new BS5MemoryCell(this,PC,linenum,word));
    }

// Two registers encoding
    private BS5program add_oooo_yyyy_xxxx(String ccc, String f, String instruction, String ry, String rx) {
        try {
            return addMemoryCell(new BS5MemoryCell_oooo_yyyy_xxxx(this, PC, linenum, ccc, f, instruction, ry, rx));
        } catch (BS5Exception e ) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        } catch (Exception e2) {
            BS5Exception be = new BS5Exception("Error:"+e2.getMessage(),e2);
            be.PC = PC;
            be.linenum = linenum;
            exceptionLst.add(be);
        }
        return this;
    }

// 8 bits immediate (valid also for signed 8 bits immediate: oooo_siii_iiii )   
    private BS5program add_oooo_iiii_iiii(String ccc, String f, String instruction, String immediat) {
        try {
            return addMemoryCell(new BS5MemoryCell_oooo_iiii_iiii(this, PC, linenum, ccc, f, instruction, immediat));
        } catch (BS5Exception e) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        } catch (Exception e2) {
            BS5Exception be = new BS5Exception("Error:"+e2.getMessage(),e2);
            be.PC = PC;
            be.linenum = linenum;
            exceptionLst.add(be);
        }
        return this;
    }

// 1 register and 4 bits immediate
    private BS5program add_oooo_iiii_xxxx(String ccc, String f, String instruction, String rx, String immediat) {
        try {
            return addMemoryCell(new BS5MemoryCell_oooo_iiii_xxxx(this, PC, linenum, ccc, f, instruction, rx, immediat));
        } catch (BS5Exception e) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        } catch (Exception e2) {
            BS5Exception be = new BS5Exception("Error:"+e2.getMessage(),e2);
            be.PC = PC;
            be.linenum = linenum;
            exceptionLst.add(be);
        }
        return this;
    }

// 1 register
    private BS5program add_oooo_oooo_xxxx(String ccc, String f, String instruction, String rx) {
        try {
            return addMemoryCell(new BS5MemoryCell_oooo_oooo_xxxx(this, PC, linenum, ccc, f, instruction, rx));
        } catch (BS5Exception e) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        } catch (Exception e2) {
            BS5Exception be = new BS5Exception("Error:"+e2.getMessage(),e2);
            be.PC = PC;
            be.linenum = linenum;
            exceptionLst.add(be);
        }
        return this;
    }

// 4 bits immediate
    private BS5program add_oooo_oooo_iiii(String ccc, String f, String instruction, String immediate)  {
        try {
            return addMemoryCell(new BS5MemoryCell_oooo_oooo_iiii(this, PC, linenum, ccc, f, instruction, immediate));
        } catch (BS5Exception e) {
            e.PC = PC;
            e.linenum = linenum;
            exceptionLst.add(e);
        } catch (Exception e2) {
            BS5Exception be = new BS5Exception("Error:"+e2.getMessage(),e2);
            be.PC = PC;
            be.linenum = linenum;
            exceptionLst.add(be);
        }
        return this;
    }


/*
ccc f 0000 yyyy xxxx
	mov [Rx], Ry
*/
    public BS5program asm_mov_atRx_Ry(String ccc, String f, String rx, String ry)  {
        return add_oooo_yyyy_xxxx(ccc, f==null?"nf":f, "mov [Rx], Ry", ry, rx);
    }
/*
ccc f 0001 yyyy xxxx
	mov Rx, [Ry] 
*/
    public BS5program asm_mov_Rx_atRy(String ccc, String f, String rx, String ry)  {
        return add_oooo_yyyy_xxxx(ccc, f==null?"nf":f, "mov Rx, [Ry]", ry, rx);
    }

/*
ccc f 0010 yyyy xxxx
	add Rx, [Ry]
*/
    public BS5program asm_add_Rx_atRy(String ccc, String f, String rx, String ry) {
        return add_oooo_yyyy_xxxx(ccc, f==null?"fl":f, "add Rx, [Ry]", ry, rx);
    }

/*
ccc f 0011 yyyy xxxx
	sub Rx, [Ry]
*/
    public BS5program asm_sub_Rx_atRy(String ccc, String f, String rx, String ry) {
        return add_oooo_yyyy_xxxx(ccc, f==null?"fl":f, "sub Rx, [Ry]", ry, rx);
    }

/*
ccc f 0100 yyyy xxxx
	mov Rx, Ry
*/
    public BS5program asm_mov_Rx_Ry(String ccc, String f, String rx, String ry) {
        return add_oooo_yyyy_xxxx(ccc, f==null?"nf":f, "mov Rx, Ry", ry, rx);
    }

/*
ccc f 0101 yyyy xxxx
	add Rx, Ry
*/
    public BS5program asm_add_Rx_Ry(String ccc, String f, String rx, String ry) {
        return add_oooo_yyyy_xxxx(ccc, f==null?"fl":f, "add Rx, Ry", ry, rx);
    }

/*
ccc f 0110 yyyy xxxx
	sub Rx, Ry
*/
    public BS5program asm_sub_Rx_Ry(String ccc, String f, String rx, String ry) {
        return add_oooo_yyyy_xxxx(ccc, f==null?"fl":f, "sub Rx, Ry", ry, rx);
    }
/*
ccc f 0111 iiii iiii
	mov low R0, imm8
*/
    public BS5program asm_mov_low_R0_imm8(String ccc, String f, String imm8) {
        return add_oooo_iiii_iiii(ccc, f==null?"nf":f, "mov low R0, imm8", imm8);
    }

/*
ccc f 1000 iiii iiii
	mov high R0, imm8
*/
    public BS5program asm_mov_high_R0_imm8(String ccc, String f, String imm8) {
        return add_oooo_iiii_iiii(ccc, f==null?"nf":f, "mov high R0, imm8", imm8);
    }

/*
ccc f 1001 siii iiii
	add R15, simm8
*/
    public BS5program asm_add_R15_simm8(String ccc, String f, String simm8) {
        return add_oooo_iiii_iiii(ccc, f==null?"nf":f, "add R15, simm8", simm8);
    }

/*
ccc f 1010 iiii xxxx
	mov X, Rx:imm4
*/
    public BS5program asm_mov_X_Rx_imm4(String ccc, String f, String rx, String imm4) {
        return add_oooo_iiii_xxxx(ccc, f==null?"nf":f, "mov C, Rx:imm4", rx, imm4);
    }

/*ccc f 1011 iiii xxxx
	mov Rx:imm4, X
*/
    public BS5program asm_mov_Rx_imm4_X(String ccc, String f, String rx, String imm4) {
        return add_oooo_iiii_xxxx(ccc, f==null?"nf":f, "mov Rx:imm4, C", rx, imm4);
    }

/*ccc f 1100 iiii xxxx
	mov Rx:imm4, 0
*/
    public BS5program asm_mov_Rx_imm4_0(String ccc, String f, String rx, String imm4) {
        return add_oooo_iiii_xxxx(ccc, f==null?"nf":f, "mov Rx:imm4, 0", rx, imm4);
    }

/*ccc f 1101 iiii xxxx
	mov Rx:imm4, 1
*/
    public BS5program asm_mov_Rx_imm4_1(String ccc, String f, String rx, String imm4) {
        return add_oooo_iiii_xxxx(ccc, f==null?"nf":f, "mov Rx:imm4, 1", rx, imm4);
    }

/*ccc f 1110 iiii xxxx
	not Rx:imm4
*/
    public BS5program asm_not_Rx_imm4(String ccc, String f, String rx, String imm4) {
        return add_oooo_iiii_xxxx(ccc, f==null?"fl":f, "not Rx:imm4", rx, imm4);
    }

/*
ccc f 1111 0000 iiii
	add R0, imm4
*/
    public BS5program asm_add_R0_imm4(String ccc, String f, String imm4) {
        return add_oooo_oooo_iiii(ccc, f==null?"fl":f, "add R0, imm4", imm4);
    }

/*
ccc f 1111 0001 iiii
	sub R0, imm4
*/
    public BS5program asm_sub_R0_imm4(String ccc, String f, String imm4) {
        return add_oooo_oooo_iiii(ccc, f==null?"fl":f, "sub R0, imm4", imm4);
    }

/*
ccc f 1111 0010 iiii
	shl R0, imm4
*/
    public BS5program asm_shl_R0_imm4(String ccc, String f, String imm4) {
        return add_oooo_oooo_iiii(ccc, f==null?"fl":f, "shl R0, imm4", imm4);
    }

/*
ccc f 1111 0011 iiii
	shr R0, imm4
*/
    public BS5program asm_shr_R0_imm4(String ccc, String f, String imm4) {
        return add_oooo_oooo_iiii(ccc, f==null?"fl":f, "shr R0, imm4", imm4);
    }

/*
ccc f 1111 0100 xxxx
    add Rx, 1
*/
    public BS5program asm_add_Rx_1(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"fl":f,"add Rx, 1", rx);
    }

/*
ccc f 1111 0101 xxxx
    sub Rx, 1
*/
    public BS5program asm_sub_Rx_1(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"fl":f,"sub Rx, 1", rx);
    }

/*
ccc f 1111 0110 xxxx
    mov low R0, low Rx
*/
    public BS5program asm_mov_low_R0_low_Rx(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"nf":f,"mov low R0, low Rx", rx);
    }

/*
ccc f 1111 0111 xxxx
    mov low R0, high Rx
*/
    public BS5program asm_mov_low_R0_high_Rx(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"nf":f,"mov low R0, high Rx", rx);
    }

/*
ccc f 1111 1000 xxxx
    mov high R0, low Rx
*/
    public BS5program asm_mov_high_R0_low_Rx(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"nf":f,"mov high R0, low Rx", rx);
    }

/*
ccc f 1111 1001 xxxx
    mov high R0, high Rx
*/
    public BS5program asm_mov_high_R0_high_Rx(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"nf":f,"mov high R0, high Rx", rx);
    }

/*
ccc f 1111 1010 xxxx
    mov low Rx, 0
*/
    public BS5program asm_mov_low_Rx_0(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"nf":f,"mov low Rx, 0", rx);
    }

/*
ccc f 1111 1011 xxxx
    mov high Rx, 0
*/
    public BS5program asm_mov_high_Rx_0(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"nf":f,"mov high Rx, 0", rx);
    }

/*
ccc f 1111 1100 xxxx
    mov Rx, 0
*/
    public BS5program asm_mov_Rx_0(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"nf":f,"mov Rx, 0", rx);
    }

/*
ccc f 1111 1101 xxxx
    not Rx
*/
    public BS5program asm_not_Rx(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"fl":f,"not Rx", rx);
    }

/*
ccc f 1111 1110 xxxx
    and R0, Rx
*/
    public BS5program asm_and_R0_Rx(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"fl":f,"and R0, Rx", rx);
    }

/*
ccc f 1111 1111 xxxx
    or  R0, Rx
*/
    public BS5program asm_or_R0_Rx(String ccc, String f, String rx) {
        return add_oooo_oooo_xxxx(ccc, f==null?"fl":f,"or R0, Rx", rx);
    }

// Microprograms 

    // Microprogram prolog
    //	ccc nf add R15, 1  ; if ccc != al
	//  al  nf add R15, size  ; if ccc != al
    private BS5program asm_prologMicroprogram(String ccc, String f, int microprogramsize) {
        if (ccc == null || ccc.toLowerCase().equals("al")) return this;
        return  asm_add_R15_simm8(ccc, "nf", "+1").
                asm_add_R15_simm8("al", "nf", "+"+microprogramsize);
    }

/* Microprogram for versatility */
/*
ccc f mov low Rx, imm8 
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  mov low R0, imm8
	al  nf mov Rx, R0
*/
    public BS5program asm_mov_low_Rx_imm8(String ccc, String f, String rx, String  imm8) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al","nf","R0",rx).
                asm_mov_low_R0_imm8("al", f, imm8).
                asm_mov_Rx_Ry("al","nf",rx, "R0");
    }

/*
ccc f  mov high Rx, imm8
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  mov high R0, imm8
	al  nf mov Rx, R0
*/
    public BS5program asm_mov_high_Rx_imm8(String ccc, String f, String rx, String  imm8) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al","nf","R0",rx).
                asm_mov_high_R0_imm8("al", f, imm8).
                asm_mov_Rx_Ry("al","nf",rx,"R0");
    }

/*
ccc f add Rx, imm4
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  add R0, imm4
	al  nf mov Rx, R0
*/
    public BS5program asm_add_Rx_imm4(String ccc, String f, String rx, String imm4) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_add_R0_imm4("al", f, imm4).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f sub Rx, imm4
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  sub R0, imm4
	al  nf mov Rx, R0
*/
    public BS5program asm_sub_Rx_imm4(String ccc, String f, String rx, String imm4) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_sub_R0_imm4("al", f, imm4).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f shl Rx, imm4
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  shl R0, imm4
	al  nf mov Rx, R0
*/
    public BS5program asm_shl_Rx_imm4(String ccc, String f, String rx, String imm4) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_shl_R0_imm4("al", f, imm4).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f shr Rx, imm4
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  shr R0, imm4
	al  nf mov Rx, R0
*/
    public BS5program asm_shr_Rx_imm4(String ccc, String f, String rx, String imm4) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_shr_R0_imm4("al", f, imm4).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f mov low Rx, low Ry
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  mov low R0, low Ry
	al  nf mov Rx, R0
*/
    public BS5program asm_mov_low_Rx_low_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_mov_low_R0_low_Rx("al", f, ry).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f mov low Rx, high Ry
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  mov low R0, high Ry
	al  nf mov Rx, R0
*/
    public BS5program asm_mov_low_Rx_high_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_mov_low_R0_high_Rx("al", f, ry).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f mov high Rx, low Ry
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  mov high R0, low Ry
	al  nf mov Rx, R0
*/
    public BS5program asm_mov_high_Rx_low_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_mov_high_R0_low_Rx("al", f, ry).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f mov high Rx, high Ry
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  mov high R0, high Ry
	al  nf mov Rx, R0
*/
    public BS5program asm_mov_high_Rx_high_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_mov_high_R0_high_Rx("al", f, ry).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f and Rx, Ry
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  and R0, Ry
	al  nf mov Rx, R0
*/
    public BS5program asm_and_Rx_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_and_R0_Rx("al", f, ry).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f or  Rx, Ry
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  f  or R0, Ry
	al  nf mov Rx, R0
*/
    public BS5program asm_or_Rx_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_or_R0_Rx("al", f, ry).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/* Microprogram for 16 bits immediate */

/*
ccc f mov Rx, imm16   ; (Rx != R0 and imm16 > 15)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  f  mov Rx, R0
*/
    public BS5program asm_mov_Rx_imm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_Ry("al", f, rx, "R0");
    }

/*
ccc f mov R0, imm16   ; ( imm16 > 15 )
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov R0,R0 ;   if f=fl otherwise no instruction
or
	ccc nf mov low R0, low imm16
	ccc nf mov high R0, high imm16
	ccc  f mov R0,R0 ;   if f=fl otherwise no instruction

*/
    public BS5program asm_mov_R0_imm16(String ccc, String f, String imm16) {
        asm_mov_low_R0_imm8(ccc, "nf", "L8:" + imm16).
        asm_mov_high_R0_imm8(ccc, "nf", "H8:"+ imm16);
        if ("fl".equalsIgnoreCase(f)) asm_mov_Rx_Ry(ccc, f, "R0", "R0");
        return this;
    }

/*
ccc f add Rx, imm16   ; (Rx != R0 and imm16 > 15)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  add Rx, R0
*/
    public BS5program asm_add_Rx_imm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_Ry("al", f, rx, "R0");
    }

/*
ccc f sub Rx, imm16   ; (Rx != R0 and imm16 > 15)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  sub Rx, R0
*/
    public BS5program asm_sub_Rx_imm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_sub_Rx_Ry("al", f, rx, "R0");
    }

/*
ccc f mov Rx, [imm16] ; ( Rx can be R0 )
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  f  mov Rx, [R0]
*/
    public BS5program asm_mov_Rx_atImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", f, rx, "R0");
    }

/*
ccc f mov [imm16], Rx ; ( Rx != R0 )
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  f  mov [R0], Rx
*/
    public BS5program asm_mov_atImm16_Rx(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R0", rx);
    }

/*
ccc f add Rx, [imm16] ; ( Rx != R0 )
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  f  add Rx, [R0]
*/
    public BS5program asm_add_Rx_atImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_atRy("al", f, rx, "R0");
    }

/*
ccc f sub Rx, [imm16] ; ( Rx != R0 )
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  f  sub Rx, [R0]
*/
    public BS5program asm_sub_Rx_atImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,3).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_sub_Rx_atRy("al", f, rx, "R0");
    }

/*
ccc f and Rx, imm16   ; (Rx != R0)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  f  and R0, Rx
	al  nf mov Rx, R0
*/
    public BS5program asm_and_Rx_imm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,4).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_and_R0_Rx("al", f, rx).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }
   
/*
ccc f or Rx, imm16    ; (Rx != R0)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  f  or R0, Rx
	al  nf mov Rx, R0
*/
    public BS5program asm_or_Rx_imm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,4).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_or_R0_Rx("al", f, rx).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f and Rx, [imm16] ; (Rx != R0)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  nf mov R0, [R0]
	al  f  and R0, Rx
	al  nf mov Rx, R0
*/
   public BS5program asm_and_Rx_atImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,5).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_and_R0_Rx("al", f, rx).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f or Rx, [imm16]  ; (Rx != R0)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf mov low R0, low imm6
	al  nf mov high R0, high imm16
	al  nf mov R0, [R0]
	al  f  or R0, Rx
	al  nf mov Rx, R
*/
   public BS5program asm_or_Rx_atImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f,5).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_or_R0_Rx("al", f, rx).
                asm_mov_Rx_Ry("al", "nf", rx, "R0");
    }

/*
ccc f mov STACK, Rx
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 2  ; if ccc != al
	al nf sub R13,1
	al f  mov [R13], Rx
*/
    public BS5program asm_mov_stack_Rx(String ccc, String f, String rx) {
        return  asm_prologMicroprogram(ccc, f, 2).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", f, "R13", rx);
    }

/*
ccc f mov STACK, [Rx]  (R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al nf sub R13, 1
	al nf mov R0, [Rx]
	al f  mov [R13], R0
*/
    public BS5program asm_mov_stack_atRx(String ccc, String f, String rx) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_Rx_atRy("al", "nf", "R0", rx).
                asm_mov_atRx_Ry("al", f, "R13", "R0");
    }

/*
ccc f mov STACK, imm16 (R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf sub R13, 1
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R13], R0
*/
    public BS5program asm_mov_stack_imm16(String ccc, String f, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 4).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R13", "R0");
    }

/*
ccc f mov STACK, [imm16] ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf sub R13, 1
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf mov R0, [R0]
	al  f  mov [R13], R0
*/
    public BS5program asm_mov_stack_atImm16(String ccc, String f, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 5).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, "R13", "R0");
    }

/*  Pop value from stack (3 microprograms)
ccc f mov Rx, STACK (if R15 then it is like a return from call procedure)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 2  ; if ccc != al
	al  f mov Rx, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_Rx_stack(String ccc, String f, String rx) {
        return  asm_prologMicroprogram(ccc, f, 2).
                asm_mov_Rx_atRy("al", f, rx, "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov [Rx], STACK ( Rx != R0, R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, [R13]
	al  nf add R13,1
	al  f mov [Rx], R0
*/
    public BS5program asm_mov_atRx_stack(String ccc, String f, String rx) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_atRy("al", "nf", "R0", "R13").
                asm_add_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", f, rx, "R0");
    }

/*
ccc f mov [imm16], STACK (R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
    al  nf sub R13, 1
	al  nf mov [R13],R1
	al  nf add R13, 1
	al  nf mov R1, [R13]
	al  f  mov [R0], R1
	al  nf sub R13, 1
	al  nf mov R1, [R13]
	al  nf add R13, 1
	al  nf add R13, 1
*/
    public BS5program asm_mov_atImm16_stack(String ccc, String f, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_add_Rx_1("al", "nf", "R13").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_mov_atRx_Ry("al",f, "R0", "R1").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/* 
   Call procedure  (5 microprograms)
ccc f mov STACK:R15, Rx (Rx != R0 & Rx != R15)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf mov R0, R15
	al  nf add R0, 4
	al  nf sub R13, 1
	al  nf mov [R13], R0
	al  f  mov R15, Rx
*/
    public BS5program asm_mov_stackR15_Rx(String ccc, String f, String rx) {
        return  asm_prologMicroprogram(ccc, f, 5).
                asm_mov_Rx_Ry("al", "nf", "R0", "R15").
                asm_add_R0_imm4("al", "nf", "4").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R0").
                asm_mov_Rx_Ry("al", f, "R15", rx);
    }

/*
ccc f mov STACK:R15, [Rx] (Rx != R0 & Rx != R15)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf mov R0, R15
	al  nf add R0, 4
	al  nf sub R13, 1
	al  nf mov [R13], R0
	al  f  mov R15, [Rx]
*/
    public BS5program asm_mov_stackR15_atRx(String ccc, String f, String rx) {
        return  asm_prologMicroprogram(ccc, f, 5).
                asm_mov_Rx_Ry("al", "nf", "R0", "R15").
                asm_add_R0_imm4("al", "nf", "4").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R0").
                asm_mov_Rx_atRy("al", f, "R15", rx);
    }
/*
ccc f mov STACK:R15, imm16 (R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 7  ; if ccc != al
	al  nf mov R0, R15
	al  nf add R0, 6
	al  nf sub R13, 1
	al  nf mov [R13], R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov R15, R0
*/
    public BS5program asm_mov_stackR15_imm16(String ccc, String f, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 7).
                asm_mov_Rx_Ry("al", "nf", "R0", "R15").
                asm_add_R0_imm4("al", "nf", "6").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al","nf", "H8:" + imm16).
                asm_mov_Rx_Ry("al", f, "R15", "R0");
    }

/*
ccc f mov STACK:R15, [imm16] (R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 8  ; if ccc != al
	al  nf mov R0, R15
	al  nf add R0, 7
	al  nf sub R13, 1
	al  nf mov [R13], R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf mov R0, [R0]
	al  f  mov R15, R0
*/
    public BS5program asm_mov_stackR15_atImm16(String ccc, String f, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 8).
                asm_mov_Rx_Ry("al", "nf", "R0", "R15").
                asm_add_R0_imm4("al", "nf", "7").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al","nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_Rx_Ry("al", f, "R15", "R0");
    }

/*
ccc f add STACK:R15, simm8  (near address, R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf mov R0, R15
	al  nf add R0, 4
	al  nf sub R13, 1
	al  nf mov [R13], R0
	al  f  add F15, simm8	
*/
    public BS5program asm_mov_stackR15_simm8(String ccc, String f, String simm8) {
        return  asm_prologMicroprogram(ccc, f, 5).
                asm_mov_Rx_Ry("al", "nf", "R0", "R15").
                asm_add_R0_imm4("al", "nf", "4").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R0").
                asm_add_R15_simm8("al", f, simm8);
    }

/*
   Write to stack/local context (24 microprograms)
ccc f mov STACK:imm4, Rx (Rx != R0, R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, imm4
	al  f  mov [R0], Rx
*/
    public BS5program asm_mov_stackImm4_Rx(String ccc, String f, String imm4, String rx) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_mov_atRx_Ry("al",f, "R0", rx);
    }

/*
ccc f mov STACK:imm4, imm16 ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, imm4
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackImm4_imm16(String ccc, String f, String imm4, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 10).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:imm4, [Rx] (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 9  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, imm4
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov R0, [Rx]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackImm4_atRx(String ccc, String f, String imm4, String rx) {
        return  asm_prologMicroprogram(ccc, f, 9).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R0", rx).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:imm4, [imm16] (R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, imm4
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16	
	al  nf mov R0, [R0]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackImm4_atImm16(String ccc, String f, String imm4,String imm16) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:imm16, Rx (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf add R0, R13
	al  f  mov [R0], Rx
*/
    public BS5program asm_mov_stackImm16_Rx(String ccc, String f, String imm16, String rx) {
        return  asm_prologMicroprogram(ccc, f, 4).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_mov_atRx_Ry("al", f, "R0", rx);
    }

/*
ccc f mov STACK:imm16a, imm16b ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov low R0, low imm16a
	al  nf mov high R0, high imm16a
	al  nf add R0, R13
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16b
	al  nf mov high R0, high imm16b	
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackImm16_imm16(String ccc, String f, String imm16a, String imm16b) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16a).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16a).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16b).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16b).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:imm16, [Rx] (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf add R0, R13
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov R0, [Rx]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackImm16_atRx(String ccc, String f, String imm16, String rx) {
        return  asm_prologMicroprogram(ccc, f, 10).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R0", rx).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:imm16a, [imm16b] ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 12  ; if ccc != al
	al  nf mov low R0, low imm16a
	al  nf mov high R0, high imm16a
	al  nf add R0, R13
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16b
	al  nf mov high R0, high imm16b	
	al  nf mov R0, [R0]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackImm16_atImm16(String ccc, String f, String imm16a, String imm16b) {
        return  asm_prologMicroprogram(ccc, f, 12).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16a).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16a).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16b).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16b).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:Rx, Ry  (Rx and Ry != R15 and R0)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  nf add R0, R13
	al  f  mov [R0], Ry
*/
    public BS5program asm_mov_stackRx_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_mov_atRx_Ry("al", f, "R0", ry);
    }

/*
ccc f mov STACK:Rx, imm16 (Rx != R0 and R15, R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, Rx
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackRx_imm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 10).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_Rx_Ry("al", "nf", "R0", rx).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:Rx, [Ry] (Rx,Ry != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 9  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, Rx
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov R0, [Ry]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackRx_atRy(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f, 9).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_Rx_Ry("al", "nf", "R0", rx).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R0", ry).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov STACK:Rx, [imm16] (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, Rx
	al  nf sub R13, 1
	al  nf [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16	
	al  nf mov R0, [R0]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_stackRx_atImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_Rx_Ry("al", "nf", "R0", rx).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");        
    }

/*
ccc f mov LOCAL:imm4, Rx (Rx != R0, R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, imm4
	al  f  mov [R0], Rx
*/
    public BS5program asm_mov_localImm4_Rx(String ccc, String f, String imm4, String rx) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_mov_atRx_Ry("al", f, "R0", rx);
    }

/*
ccc f mov LOCAL:imm4, imm16 ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, imm4
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localImm4_imm16(String ccc, String f, String imm4, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 10).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }


/*
ccc f mov LOCAL:imm4, [Rx] (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 9  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, imm4
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov R0, [Rx]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localImm4_atRx(String ccc, String f, String imm4, String rx) {
        return  asm_prologMicroprogram(ccc, f, 9).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R0", rx).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov LOCAL:imm4, [imm16] ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, imm4
	al  nf sub R13, 1
	al  nf [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16	
	al  nf mov R0, [R0]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localImm4_atImm16(String ccc, String f, String imm4,String imm16) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }


/*
ccc f mov LOCAL:imm16, Rx (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf add R0, R12
	al  f  mov [R0], Rx
*/
    public BS5program asm_mov_localImm16_Rx(String ccc, String f, String imm16, String rx) {
        return  asm_prologMicroprogram(ccc, f, 4).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_Ry("al", "nf", "R0", "R12").
                asm_mov_atRx_Ry("al", f, "R0", rx);
    }

/*
ccc f mov LOCAL:imm16a, imm16b ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov low R0, low imm16a
	al  nf mov high R0, high imm16a
	al  nf add R0, R12
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16b
	al  nf mov high R0, high imm16b	
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localImm16_imm16(String ccc, String f, String imm16a, String imm16b) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16a).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16a).
                asm_add_Rx_Ry("al", "nf", "R0", "R12").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16b).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16b).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov LOCAL:imm16, [Rx] (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf mov low R0, low imm16a
	al  nf mov high R0, high imm16a
	al  nf add R0, R12
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov R0, [Rx]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localImm16_atRx(String ccc, String f, String imm16, String rx) {
        return  asm_prologMicroprogram(ccc, f, 10).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_Ry("al", "nf", "R0", "R12").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R0", rx).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov LOCAL:imm16a, [imm16b] ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 12  ; if ccc != al
	al  nf mov low R0, low imm16a
	al  nf mov high R0, high imm16a
	al  nf add R0, R12
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16b
	al  nf mov high R0, high imm16b	
	al  nf mov R0, [R0]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localImm16_atImm16(String ccc, String f, String imm16a, String imm16b) {
        return  asm_prologMicroprogram(ccc, f, 12).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16a).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16a).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16b).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16b).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov LOCAL:Rx, Ry  (Rx and Ry != R15 and R0)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, Rx
	al  nf add R0, R12
	al  f  mov [R0], Ry
*/
    public BS5program asm_mov_localRx_Ry(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_add_Rx_Ry("al", "nf", "R0", "R12").
                asm_mov_atRx_Ry("al", f, "R0", ry);
    }

/*
ccc f mov LOCAL:Rx, imm16 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, Rx
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localRx_imm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 10).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_add_Rx_Ry("al", "nf", "R0", rx).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov LOCAL:Rx, [Ry] (Rx,Ry != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 9  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, Rx
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov R0, [Ry]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localRx_atRy(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f, 9).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_add_Rx_Ry("al", "nf", "R0", rx).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R0", ry).
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov LOCAL:Rx, [imm16] (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, Rx
	al  nf sub R13, 1
	al  nf [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16	
	al  nf mov R0, [R0]
	al  f  mov [R1], R0
	al  nf mov R1, [R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_localRx_atImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_Rx_Ry("al", "nf", "R0", "R12").
                asm_add_Rx_Ry("al", "nf", "R0", rx).
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, "R1", "R0").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");        
    }

/*
	Read from stack/local context (18 microprograms)
ccc f mov Rx, STACK:imm4 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, imm4
	al  f mov Rx, [R0]
*/
    public BS5program asm_mov_RX_stackImm4(String ccc, String f, String rx, String imm4) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_mov_Rx_atRy("al", f, rx, "R0");
    }

/*
ccc f mov [Rx], STACK:imm4 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, imm4
	al  nf mov R0, [R0]
	al  f  mov [Rx], R0
*/
    public BS5program asm_mov_atRX_stackImm4(String ccc, String f, String rx, String imm4) {
        return  asm_prologMicroprogram(ccc, f, 4).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, rx, "R0");
    }

/*

ccc f mov [imm16], STACK:imm4 ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, imm4
	al  nf mov R0, [R0]
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R0], R1
	al  nf mov R1,[R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_atImm16_stackImm4(String ccc, String f, String imm16, String imm4) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_Rx_Ry("al", "nf", "R0", "R13").
                asm_add_R0_imm4("al", "nf", imm4).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al","nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R0", "R1").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov Rx, STACK:imm16 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf add R0, R13
	al  f mov Rx, [R0]
*/
    public BS5program asm_mov_Rx_stackImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 4).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_mov_Rx_atRy("al", f, rx, "R0");
    }

/*
ccc f mov [Rx], STACK:imm16 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf add R0, R13
	al  nf mov R0, [R0]
	al  f  mov [Rx], R0
*/
    public BS5program asm_mov_atRx_stackImm16(String ccc, String f, String rx, String imm16) {
        return  asm_prologMicroprogram(ccc, f, 5).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, rx, "R0");
    }

/*
ccc f mov [imm16a], STACK:imm16b ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 12  ; if ccc != al
	al  nf mov low R0, low imm16b
	al  nf mov high R0, high imm16b
	al  nf add R0, R13
	al  nf mov R0, [R0]
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16a
	al  nf mov high R0, high imm16a
	al  f  mov [R0], R1
	al  nf mov R1,[R13]
	al  nf add R13, 1
*/
    public BS5program asm_mov_atImm16a_stackImm16b(String ccc, String f, String imm16a, String imm16b) {
        return  asm_prologMicroprogram(ccc, f, 12).
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16b).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16b).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16a).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16a).
                asm_mov_atRx_Ry("al", f, "R0", "R1").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
    }

/*
ccc f mov Rx, STACK:Ry (Rx,Ry != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, Ry
	al  f  mov Rx, [R0]
*/
    public BS5program asm_mov_Rx_stackRy(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f, 3).
                asm_mov_Rx_Ry("al","nf", "R0", "R13").
                asm_add_Rx_Ry("al", "nf", "R0", ry).
                asm_mov_Rx_atRy("al", f, rx, "R0");
     }

/*
ccc f mov [Rx], STACK:Ry (Rx,Ry != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov R0, R13
	al  nf add R0, Ry
	al  nf mov R0, [R0]
	al  f  mov [Rx], R0
*/
    public BS5program asm_mov_atRx_stackRy(String ccc, String f, String rx, String ry) {
        return  asm_prologMicroprogram(ccc, f, 4).
                asm_mov_Rx_Ry("al","nf", "R0", "R13").
                asm_add_Rx_Ry("al", "nf", "R0", ry).
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_mov_atRx_Ry("al", f, rx, "R0");
     }

/*
ccc f mov [imm16], STACK:Rx
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, Rx
	al  nf add R0, R13
	al  nf mov R0, [R0]
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R0], R1
	al  nf mov R1,[R13]
	al  nf add R13, 1
*/
     public BS5program asm_mov_atImm16_stackRx(String ccc, String f, String imm16, String rx) {
        return  asm_prologMicroprogram(ccc, f, 11).
                asm_mov_Rx_Ry("al", "nf", "R0", rx).
                asm_add_Rx_Ry("al", "nf", "R0", "R13").
                asm_mov_Rx_atRy("al", "nf", "R0", "R0").
                asm_sub_Rx_1("al", "nf", "R13").
                asm_mov_atRx_Ry("al", "nf", "R13", "R1").
                asm_mov_Rx_Ry("al", "nf", "R1", "R0").
                asm_mov_low_R0_imm8("al", "nf", "L8:" + imm16).
                asm_mov_high_R0_imm8("al", "nf", "H8:" + imm16).
                asm_mov_atRx_Ry("al", f, "R0", "R1").
                asm_mov_Rx_atRy("al", "nf", "R1", "R13").
                asm_add_Rx_1("al", "nf", "R13");
     }

/*
ccc f mov Rx, LOCAL:imm4 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, imm4
	al  f mov Rx, [R0]

ccc f mov [Rx], LOCAL:imm4 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, imm4
	al  nf mov R0, [R0]
	al  f  mov [Rx], R0

ccc f mov [imm16], LOCAL:imm4 (R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, imm4
	al  nf mov R0, [R0]
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R0], R1
	al  nf mov R1,[R13]
	al  nf add R13, 1

ccc f mov Rx, LOCAL:imm16 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf add R0, R12
	al  f mov Rx, [R0]

ccc f mov [Rx], LOCAL:imm16 (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 5  ; if ccc != al
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  nf add R0, R12
	al  nf mov R0, [R0]
	al  f  mov [Rx], R0

ccc f mov [imm16a], LOCAL:imm16b ( R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 12  ; if ccc != al
	al  nf mov low R0, low imm16b
	al  nf mov high R0, high imm16b
	al  nf add R0, R12
	al  nf mov R0, [R0]
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16a
	al  nf mov high R0, high imm16a
	al  f  mov [R0], R1
	al  nf mov R1,[R13]
	al  nf add R13, 1

ccc f mov Rx, LOCAL:Ry (Rx,Ry != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, Ry
	al  f  mov Rx, [R0]

ccc f mov [Rx], LOCAL:Ry (Rx,Ry != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov R0, R12
	al  nf add R0, Ry
	al  nf mov R0, [R0]
	al  f  mov [Rx], R0

ccc f mov [imm16], LOCAL:Rx (Rx != R0 , R0 modified)
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 11  ; if ccc != al
	al  nf mov R0, Rx
	al  nf add R0, R12
	al  nf mov R0, [R0]
	al  nf sub R13, 1
	al  nf mov [R13], R1
	al  nf mov R1, R0
	al  nf mov low R0, low imm16
	al  nf mov high R0, high imm16
	al  f  mov [R0], R1
	al  nf mov R1,[R13]
	al  nf add R13, 1

	Local context / Stack context setting (2 microprograms)
ccc f mov STACK, LOCAL 
	ccc f mov R13, R12

ccc f mov LOCAL, STACK 
	ccc f mov R12, R13

	Standard instructions (add, sub, shl, shr, and, or , not) by stack (7 microprograms)
ccc f add STACK
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov R0, [R13]
	al  nf add R13, 1
	al   f add R0, [R13]
	al  nf mov [R13], R0

ccc f sub STACK
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 4  ; if ccc != al
	al  nf mov R0, [R13]
	al  nf add R13, 1
	al   f sub R0, [R13]
	al  nf mov [R13], R0

ccc f shl STACK, imm4
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, [13]
	al  f  shl R0, imm4
	al  nf mov [R13], R0

ccc f shr STACK, imm4
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf mov R0, [13]
	al  f  shr R0, imm4
	al  nf mov [R13], R0

ccc f and STACK
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf add R13, 1
	al  nf mov R0, [R13]
	al  nf mov [R13], R1
	al  nf add R13, 1
	al  nf mov R1, [R13]
	al  f  and R0, R1
	al  nf mov [R13], R0
	al  nf sub R13, 1
	al  nf mov R1, [R13]
	al  nf add R13, 1

ccc f or STACK
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 10  ; if ccc != al
	al  nf add R13, 1
	al  nf mov R0, [R13]
	al  nf mov [R13], R1
	al  nf add R13, 1
	al  nf mov R1, [R13]
	al  f  or  R0, R1
	al  nf mov [R13], R0
	al  nf sub R13, 1
	al  nf mov R1, [R13]
	al  nf add R13, 1
	
ccc f not STACK
	ccc nf add R15, 1  ; if ccc != al
	al  nf add R15, 3  ; if ccc != al
	al  nf R0, [R13]
	al  f  not R0
	al  nf [R13], R0

*/

}

