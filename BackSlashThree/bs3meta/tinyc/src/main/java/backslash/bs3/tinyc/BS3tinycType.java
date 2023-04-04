package backslash.bs3.tinyc;

public class BS3tinycType {
    public boolean isLabel;
    public boolean isWord;
    public boolean isByte;
    public boolean isSigned;
    public boolean isUnsigned;
    public BS3tinycType()
    {
        isWord = true;
        isSigned = false;
        isLabel = false;
        isByte = ! isWord;
        isUnsigned = !isSigned;
    }
    public BS3tinycType(boolean _isWord, Boolean _isSigned)
    {
        this.isWord = _isWord;
        this.isSigned = _isSigned;
        isLabel = false;
        isByte = ! isWord;
        isUnsigned = !isSigned;
    }

    public static BS3tinycType _void()
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = false;
        t.isSigned = false;
        t.isLabel = true;
        t.isByte = false;
        t.isUnsigned = false;
        return t;
    }

    public static BS3tinycType unsignedword()
    {
        return new BS3tinycType(true, false);
    }

    public static BS3tinycType signedword()
    {
        return new BS3tinycType(true, true);
    }

    public static BS3tinycType word()
    {
        return unsignedword();
    }

    public static BS3tinycType unsignedbyte()
    {
        return new BS3tinycType(true, false);
    }

    public static BS3tinycType signedbyte()
    {
        return new BS3tinycType(true, true);
    }

    public static BS3tinycType _byte()
    {
        return unsignedbyte();
    }


    public static BS3tinycType cond3Type(BS3tinycType left, BS3tinycType right)
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = left.isWord || right.isWord;
        t.isSigned = left.isSigned || right.isSigned;
        t.isLabel = false;
        t.isByte = !t.isWord;
        t.isUnsigned = !t.isSigned;
        return t;
    }

    public static BS3tinycType iorType(BS3tinycType left, BS3tinycType right)
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = left.isWord || right.isWord;
        t.isSigned = left.isSigned || right.isSigned;
        t.isLabel = false;
        t.isByte = !t.isWord;
        t.isUnsigned = !t.isSigned;
        return t;
    }


    public static BS3tinycType eorType(BS3tinycType left, BS3tinycType right)
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = left.isWord || right.isWord;
        t.isSigned = left.isSigned || right.isSigned;
        t.isLabel = false;
        t.isByte = !t.isWord;
        t.isUnsigned = !t.isSigned;
        return t;
    }

    public static BS3tinycType andType(BS3tinycType left, BS3tinycType right)
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = left.isWord || right.isWord;
        t.isSigned = left.isSigned || right.isSigned;
        t.isLabel = false;
        t.isByte = !t.isWord;
        t.isUnsigned = !t.isSigned;
        return t;
    }


    public static BS3tinycType shiftType(BS3tinycType left, BS3tinycType right)
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = true;
        t.isSigned = left.isSigned || right.isSigned;
        t.isLabel = false;
        t.isByte = !t.isWord;
        t.isUnsigned = !t.isSigned;
        return t;
    }

    public static BS3tinycType addType(BS3tinycType left, BS3tinycType right)
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = true;
        t.isSigned = left.isSigned || right.isSigned;
        t.isLabel = false;
        t.isByte = !t.isWord;
        t.isUnsigned = !t.isSigned;
        return t;
    }

    public static BS3tinycType mulType(BS3tinycType left, BS3tinycType right)
    {
        BS3tinycType t = new BS3tinycType();
        t.isWord = true;
        t.isSigned = left.isSigned || right.isSigned;
        t.isLabel = false;
        t.isByte = !t.isWord;
        t.isUnsigned = !t.isSigned;
        return t;
    }

    public BS3tinycType setSigned()
    {
        isSigned = true;
        isUnsigned = false;       
        return this;
    }

    public BS3tinycType setUnsigned()
    {
        isSigned = false;
        isUnsigned = true;       
        return this;
    }

}
