package backslash.bs5.asm;

public class BS5Label {
    private BS5Label globalParent;
    private String name;
    private int addr;
    private int linenum;
    private int linenumend;
    private boolean isUsed;

    public BS5Label(String name, int addr, int linenum) {
        this(null,name,addr,linenum);
    }

    public BS5Label(BS5Label globalParent,String name, int addr, int linenum) {
        this.globalParent   = globalParent==null?this:globalParent;
        this.name           = name;
        this.addr           = addr;
        this.linenum        = linenum;
        linenumend          = Integer.MAX_VALUE; // usefull only for global labels
        isUsed              = false;
    }

    public BS5Label getGlobalParent()               { return globalParent;          }
    public String   getName()                       { return name;                  }
    public boolean  isLocal()                       { return !isGlobal();           }
    public boolean  isGlobal()                      { return globalParent == this;  }
    public int      getAddr()                       { return addr;                  }
    public int      getLinenum()                    { return linenum;               }
    public int      getLinenumend()                 { return linenumend;            }
    public void     setLinenumend(int endofscope)   { this.linenumend = endofscope; }
    public boolean  isUsed()                        { return isUsed;                }
    public BS5Label useIt()                         { isUsed = true; return this;   }

}
