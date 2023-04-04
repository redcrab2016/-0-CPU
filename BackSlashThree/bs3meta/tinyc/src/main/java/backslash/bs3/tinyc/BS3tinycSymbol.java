package backslash.bs3.tinyc;

public class BS3tinycSymbol {
    public BS3tinycType type;
    public String name;
    public boolean isExtern;
    public int size;

    public BS3tinycSymbol()
    {
        this("_nonane_", BS3tinycType.word(), false, 1);
    }

    public BS3tinycSymbol(String _name)
    {
        this();
        name = _name;
    }

    public BS3tinycSymbol(String _name, BS3tinycType _type, boolean _isExtern, int _size)
    {
        name = _name;
        type = _type;
        isExtern = _isExtern;
        size = _size;
    }

    public BS3tinycSymbol(String _name, BS3tinycType _type, boolean _isExtern, String _strsize)
    {
        name = _name;
        type = _type;
        isExtern = _isExtern;
        if (_strsize == null)  
            size = 1;
        else {
            char achar;
            size = 1;
            achar = _strsize.charAt(0);
            if ((achar >= '0' && achar <= '0') || (achar == '+'))
            {
                size = Integer.parseInt(_strsize);
            }
            if (achar == '$')
            {
                size = Integer.parseUnsignedInt(_strsize.substring(1, 5),16);
            }
        }

    }
}
