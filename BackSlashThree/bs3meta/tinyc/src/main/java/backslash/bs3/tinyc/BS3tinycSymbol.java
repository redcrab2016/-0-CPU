package backslash.bs3.tinyc;

public class BS3tinycSymbol {
    public BS3tinycType type;
    public String name;
    public boolean isExtern;

    public BS3tinycSymbol()
    {
        this("_nonane_", BS3tinycType.word(), false);
    }

    public BS3tinycSymbol(String _name)
    {
        this();
        name = _name;
    }

    public BS3tinycSymbol(String _name, BS3tinycType _type, boolean _isExtern)
    {
        name = _name;
        type = _type;
        isExtern = _isExtern;
    }
}
