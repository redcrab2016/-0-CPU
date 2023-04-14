package backslash.bs3.tinyc;

import java.util.ArrayList;
import java.util.List;

import org.antlr.v4.runtime.tree.ErrorNode;
import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.RuleNode;
import org.antlr.v4.runtime.tree.TerminalNode;

import backslash.bs3.tinyc.generated.tinycParserBaseVisitor;
import backslash.bs3.tinyc.generated.tinycParser;
import backslash.bs3.tinyc.generated.tinycParser.AddexprContext;
import backslash.bs3.tinyc.generated.tinycParser.AndexprContext;
import backslash.bs3.tinyc.generated.tinycParser.AssignexprContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3asmblockContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3asmlabelContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3asmlineContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3commentContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3labelrefContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3macroContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3macrodefContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3opeContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3paramContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3paramsContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3valueContext;
import backslash.bs3.tinyc.generated.tinycParser.CastexprContext;
import backslash.bs3.tinyc.generated.tinycParser.CondexprContext;
import backslash.bs3.tinyc.generated.tinycParser.DeclareContext;
import backslash.bs3.tinyc.generated.tinycParser.EquaexprContext;
import backslash.bs3.tinyc.generated.tinycParser.ExcluORexprContext;
import backslash.bs3.tinyc.generated.tinycParser.ExprContext;
import backslash.bs3.tinyc.generated.tinycParser.Id_Context;
import backslash.bs3.tinyc.generated.tinycParser.IncluORexprContext;
import backslash.bs3.tinyc.generated.tinycParser.IntegerContext;
import backslash.bs3.tinyc.generated.tinycParser.JumpContext;
import backslash.bs3.tinyc.generated.tinycParser.LabelContext;
import backslash.bs3.tinyc.generated.tinycParser.LogicANDexprContext;
import backslash.bs3.tinyc.generated.tinycParser.LogicORexprContext;
import backslash.bs3.tinyc.generated.tinycParser.MulexprContext;
import backslash.bs3.tinyc.generated.tinycParser.Paren_exprContext;
import backslash.bs3.tinyc.generated.tinycParser.ProgramContext;
import backslash.bs3.tinyc.generated.tinycParser.RegByteContext;
import backslash.bs3.tinyc.generated.tinycParser.RegWordContext;
import backslash.bs3.tinyc.generated.tinycParser.RelatexprContext;
import backslash.bs3.tinyc.generated.tinycParser.ReturnfctContext;
import backslash.bs3.tinyc.generated.tinycParser.ShiftexprContext;
import backslash.bs3.tinyc.generated.tinycParser.SourcevarContext;
import backslash.bs3.tinyc.generated.tinycParser.StatementContext;
import backslash.bs3.tinyc.generated.tinycParser.StringdataContext;
import backslash.bs3.tinyc.generated.tinycParser.StringdatarawContext;
import backslash.bs3.tinyc.generated.tinycParser.TargetvarContext;
import backslash.bs3.tinyc.generated.tinycParser.TermContext;
import backslash.bs3.tinyc.generated.tinycParser.TypeContext;
import backslash.bs3.tinyc.generated.tinycParser.UnaryexprContext;


public class BS3tinycVisitor extends tinycParserBaseVisitor<List<Object>> {

    private tinycParser parser;
    private int labelcounter;

    public BS3tinycVisitor(tinycParser _parser) {
        parser = _parser;
    }

    protected List<Object> addComment(List<Object> asmresult, String comment) {
        comment = String.valueOf(comment);
        if (comment.length() >70) comment = comment.substring(0, 70);
        int i;
        String cc="";
        char c=0;
        for (i = 0 ; i < comment.length(); i++) {
            c = comment.charAt(i);
            if (c == '\r' || c == '\n' ) return asmresult;
            if (c< 32) {
                cc = cc + "\\"+String.valueOf((int)c);
            } else {
                cc =cc + c;
            }
        }
        comment = cc;
        asmresult.add("; " +  comment);
        return asmresult;
    }

    protected List<Object> addComment(List<Object> asmresult, ParseTree node, String comment) {
        if (node == null) return asmresult;
        if (comment != null && comment.length() > 30) {
            comment = comment.substring(0, 30);
        }
        if (node instanceof TerminalNode)
        {
            TerminalNode tn=(TerminalNode)node;
            return addComment(asmresult, "[BS3tinyc]" + 
            tn.getSymbol().getLine() + 
            ":" + (tn.getSymbol().getCharPositionInLine() + 1) +
            ":'" + tn.getText()+"'" +
            (comment==null?"":(comment))) ;
        }
        return asmresult;
    }
    
    protected List<Object> addComment(List<Object> asmresult, ParseTree node) {
        return  addComment(asmresult, node, null);
    }

    @Override
    public List<Object> visitAddexpr(AddexprContext ctx) {
        List<Object> result = ctx.me1.accept(this);
        int i;
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result.add("    push_w1");
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
            switch (ctx.getChild(i).getText().charAt(0)) {
                case '+':
                    result.add("    add     w0, w1");
                    break;
                case '-':
                    result.add("    sub     w1, w0");
                    result.add("    mov     w0, w1");
                    break;
            }
            result.add("    pop_w1");
        }
        return result;
    }

    @Override
    public List<Object> visitAndexpr(AndexprContext ctx) {
        List<Object> result = ctx.ee1.accept(this);
        int i;
        if (ctx.getChildCount() > 1) {
            result.add("    push_w1");            
        }
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
            result.add("    and     w1, w0");
            result.add("    mov     w0, w1");
        }
        if (ctx.getChildCount() > 1) {
            result.add("    pop_w1");            
        }        
        return result;
    }

    @Override
    public List<Object> visitAssignexpr(AssignexprContext ctx) {
        List<Object> result = new ArrayList<Object>();
        int i;
        for (i = ctx.getChildCount()-1; i >= 0 ; i--) {
            result.addAll(ctx.getChild(i).accept(this));
        }
        return result;
    }

    @Override
    public List<Object> visitBs3asmblock(Bs3asmblockContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3asmblock(ctx);
    }

    @Override
    public List<Object> visitCastexpr(CastexprContext ctx) {
        List<Object> result = new ArrayList<Object>();
        switch (ctx.altNum) {
            case 1:
                result.addAll(ctx.ce.accept(this));
                if (ctx.aType.isWord != ctx.ce.aType.isWord) {
                    result.add("    eor     b1, b1");
                }
                break;
            case 2:
                result.addAll(ctx.getChild(0).accept(this));
                break;
            default: 
                break;
        }
        return result;
    }

    @Override
    public List<Object> visitCondexpr(CondexprContext ctx) {
        List<Object> result = ctx.getChild(0).accept(this);
        String labelcond;
        String labelcond2;
        String labelcondend;
        if (ctx.getChildCount()> 1) {
            labelcond = getNextLabel();
            labelcond2 = getNextLabel();
            labelcondend = getNextLabel();
            result = addComment(result, ctx.getChild(1));
            result.add("    cmp     w0, 0");
            result.add("    jnz     "+labelcond);
            result.add("    jump    "+labelcond2);
            result.add(labelcond);
            result.addAll(ctx.getChild(2).accept(this));
            result.add("    jump    "+labelcondend);
            result = addComment(result, ctx.getChild(3));
            result.add(labelcond2);
            result.addAll(ctx.getChild(4).accept(this));
            result.add(labelcondend);
        } 
        return result;
    }

    @Override
    public List<Object> visitDeclare(DeclareContext ctx) {
        return defaultResult();
    }

    @Override
    public List<Object> visitEquaexpr(EquaexprContext ctx) {
        List<Object> result = ctx.re1.accept(this);
        int i;
        String labelcond;
        String labelcondend;
        if (ctx.ope != null) {
            result.add("    push_w1");
        }
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result = addComment(result, ctx.getChild(i));
            labelcond=getNextLabel();
            labelcondend=getNextLabel();
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
            result.add("      cmp       w1, w0");
            // 'left' ope 'right', w1=left w0=right
            switch (ctx.getChild(i).getText().charAt(0)) {
                case '=':
                    result.add("    jz     " + labelcond);
                    break;
                case '!':
                    result.add("    jnz     " + labelcond);
                    break;
            }
            result.add("    eor     w0, w0");
            result.add("    j       " + labelcondend);
            result.add(labelcond);
            result.add("    mov     w0, 1");
            result.add(labelcondend);
        }
        if (ctx.ope != null) {
            result.add("    pop_w1");
        }
    return result;
    }

    @Override
    public List<Object> visitExcluORexpr(ExcluORexprContext ctx) {
        List<Object> result = ctx.ae1.accept(this);
        int i;
        if (ctx.getChildCount() > 1) {
            result.add("    push_w1");            
        }
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result = addComment(result, ctx.getChild(i));
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
            result.add("    eor     w1, w0");
            result.add("    mov     w0, w1");
        }
        if (ctx.getChildCount() > 1) {
            result.add("    pop_w1");            
        }        
        return result;
    }

    @Override
    public List<Object> visitExpr(ExprContext ctx) {
        return visitChildren(ctx);
    }

    @Override
    public List<Object> visitId_(Id_Context ctx) {
        // TODO Auto-generated method stub
        return super.visitId_(ctx);
    }

    @Override
    public List<Object> visitIncluORexpr(IncluORexprContext ctx) {
        List<Object> result = ctx.eoe1.accept(this);
        int i;
        if (ctx.getChildCount() > 1) {
            result.add("    push_w1");            
        }
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result = addComment(result, ctx.getChild(i));
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
            result.add("    eor     w1, w0");
            result.add("    mov     w0, w1");
        }
        if (ctx.getChildCount() > 1) {
            result.add("    pop_w1");            
        }        
        return result;
    }

    @Override
    public List<Object> visitInteger(IntegerContext ctx) {
        List<Object> result = new ArrayList<Object>();
        result = addComment(result, ctx.getChild(0));
        result.add("    mov     w0, "+ctx.strValue);
        return result;
    }

    @Override
    public List<Object> visitJump(JumpContext ctx) {
        List<Object> result = new ArrayList<Object>();
        result = addComment(result, ctx.getChild(0));
        result.add("    jump        " + ctx.target.getText());
        return result;
    }

    @Override
    public List<Object> visitLabel(LabelContext ctx) {
        List<Object> result = new ArrayList<Object>();
        result.add(ctx.getChild(0).getText());
        return result;
    }

    @Override
    public List<Object> visitLogicANDexpr(LogicANDexprContext ctx) {
        List<Object> result = ctx.ioe1.accept(this);
        int i;
        String labelcond;
        String labelcondfinish;
        
        if (ctx.getChildCount() > 1) {
            labelcondfinish = getNextLabel();
            labelcond=getNextLabel();
            result.add("    cmp     w0, 0");
            result.add("    jnz     " + labelcond);
            result.add("    jump    " + labelcondfinish);
            result.add(labelcond);
            for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
                result = addComment(result, ctx.getChild(i));
                labelcond=getNextLabel();
                result.addAll(ctx.getChild(i+1).accept(this));
                result.add("    cmp     w0, 0");
                result.add("    jnz     " + labelcond);
                result.add("    jump    "+ labelcondfinish);
                result.add(labelcond);
            }
            result.add("    mov     w0, 1");
            result.add(labelcondfinish);
        }        
        return result;
    }

    @Override
    public List<Object> visitLogicORexpr(LogicORexprContext ctx) {
        List<Object> result = ctx.land1.accept(this);
        int i;
        String labelcond;
        String labelcondfinish;
        
        if (ctx.getChildCount() > 1) {
            labelcondfinish = getNextLabel();
            labelcond=getNextLabel();
            result.add("    cmp     w0, 0");
            result.add("    jz      " + labelcond);
            result.add("    mov     w0, 1");
            result.add("    jump    " + labelcondfinish);
            result.add(labelcond);
            for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
                result = addComment(result, ctx.getChild(i));
                labelcond=getNextLabel();
                result.addAll(ctx.getChild(i+1).accept(this));
                result.add("    cmp     w0, 0");
                result.add("    jz      " + labelcond);
                result.add("    mov     w0, 1");
                result.add("    jump    " + labelcondfinish);
                result.add(labelcond);
            }
            result.add("    eor     w0, w0");
            result.add(labelcondfinish);
        }        
        return result;
    }

    @Override
    public List<Object> visitMulexpr(MulexprContext ctx) {
        List<Object> result = ctx.ce1.accept(this);
        int i;
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result.add("    push_w1");
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
            result = addComment(result, ctx.getChild(i));
            switch (ctx.getChild(i).getText().charAt(0)) {
                case '*':

                    if (ctx.aType.isSigned)
                        result.add("    mul     w0, w1, w0");
                    else 
                        result.add("    imul    w0, w1, w0");
                    break;
                case '/':
                    if (ctx.aType.isSigned)
                        result.add("    div     w1, w0, w1");
                    else 
                        result.add("    idiv    w1, w0, w1");
                    result.add("    mov     w0, w1");
                    break;
                case '%':
                    if (ctx.aType.isSigned)
                        result.add("    div     w1, w0, w0");
                    else 
                        result.add("    idiv    w1, w0, w0");
                    break;
            }
            result.add("    pop_w1");

        }
        return result;
    }

    @Override
    public List<Object> visitParen_expr(Paren_exprContext ctx) {
        return ctx.getChild(1).accept(this);
    }

    @Override
    public List<Object> visitProgram(ProgramContext ctx) {
        List<Object> result = visitChildren(ctx);
        result.add("    hlt");
        result.addAll(parser.symbols.generateASM());
        result.addAll(parser.stringdata.generateASM());
        return result;
    }

    @Override
    public List<Object> visitRegByte(RegByteContext ctx) {
        return super.visitRegByte(ctx);
    }

    @Override
    public List<Object> visitRegWord(RegWordContext ctx) {
        return super.visitRegWord(ctx);
    }

    @Override
    public List<Object> visitRelatexpr(RelatexprContext ctx) {
        List<Object> result = ctx.se1.accept(this);
        BS3tinycType senTypeRight;
        BS3tinycType senTypeLeft = ctx.se1.aType;
        String lblcond;
        String lblcondend;
        int i;
        if (ctx.ope != null) {
            result.add("    push_w1");
        }
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            lblcond = getNextLabel();
            lblcondend = getNextLabel();
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            senTypeRight = ((ShiftexprContext)ctx.getChild(i+1)).aType;
            result.add("    pop_w1");
            result.add("    cmp     w1, w0");
            // 'left' ope 'right' , w1=left, w0=right
            result = addComment(result, ctx.getChild(i));
            switch (ctx.getChild(i).getText().charAt(0)) {
                case '<':
                    if (ctx.getChild(i).getText().length()==2) { // '<='  
                        if (senTypeLeft.isSigned || senTypeRight.isSigned) { // jle
                            result.add("    jle     "+lblcond);
                        } else { // jbe
                            result.add("    jbe     "+lblcond);
                        }
                    } else { // '<'
                        if (senTypeLeft.isSigned || senTypeRight.isSigned) { // jl
                            result.add("    jl      "+lblcond);
                        } else { // jb/jnc
                            result.add("    jnc     "+lblcond);
                        }
                    }
                    break;
                case '>':
                    if (ctx.getChild(i).getText().length()==2) { // '>='
                        if (senTypeLeft.isSigned || senTypeRight.isSigned) { // jge
                            result.add("    jge     "+lblcond);
                        } else { // jae/jc
                            result.add("    jc      "+lblcond);
                        }
                    } else { // '>'
                        if (senTypeLeft.isSigned || senTypeRight.isSigned) { // jg
                            result.add("    jg      "+lblcond);
                        } else { // ja
                            result.add("    ja      "+lblcond);
                        }
                    }
                    break;
            } // end switch
            result.add("    eor       w0, w0");
            result.add("    j       "+lblcondend);
            result.add(lblcond);
            result.add("    mov     w0, 1");
            result.add(lblcondend);                             

            senTypeLeft = senTypeRight;
            result.add("    mov     w0, w1");
        }
        if (ctx.ope != null) {
            result.add("    pop_w1");
        }
        return result;

    }

    @Override
    public List<Object> visitReturnfct(ReturnfctContext ctx) {
        List<Object> result = new ArrayList<Object>();
        result = addComment(result, ctx.getChild(0));
        if (ctx.getChildCount() > 1) {
            result.addAll(ctx.getChild(1).accept(this));
        }
        result.add("    ret");
        return result;
    }

    @Override
    public List<Object> visitShiftexpr(ShiftexprContext ctx) {
        List<Object> result = ctx.ae1.accept(this);
        int i;
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result.add("    push_w1");
            result.add("    push_w0");
            result.addAll(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
            result = addComment(result, ctx.getChild(i));
            switch (ctx.getChild(i).getText().charAt(0)) {
                case '<':
                    result.add("    shl     w1, b0");
                    break;
                case '>':
                    if (ctx.aType.isSigned)
                        result.add("    sar     w1, b0");
                    else 
                        result.add("    shr     w1, w0");
                    break;
            }
            result.add("    mov     w0, w1");
            result.add("    pop_w1");
        }
        return result;
    }

    @Override
    public List<Object> visitSourcevar(SourcevarContext ctx) {
        List<Object> result = new ArrayList<Object>();
        switch (ctx.altNum)
        {
            case 6:
                result.add("    push_w3");
            case 5:
                result.add("    push_w2");
            case 4:
                result.add("    push_w1");
                result=addComment(result, ctx.getChild(0).getChild(0));
                break;
            default: 
                break;
        }
        switch (ctx.altNum)
        {
            case 1:
                result=addComment(result, ctx.ir.getChild(0));
                if (ctx.ope != null) /* if '*' or '&' */
                {
                    if (ctx.ope.getText().charAt(0) == '*') {
                        if (ctx.tr == null || (ctx.tr.aType.isWord || !ctx.tr.aType.isByte)) {
                            if (ctx.er != null)
                            {
                                result.addAll(ctx.er.accept(this));
                                if (parser.symbols.get(ctx.ir.getText()).type.isWord) {
                                    result.add("    shl     w0");
                                }
                                result.add("    add     w0, " + ctx.ir.getText());
                                result.add("    ld      w0, [w0]");
                            } else {
                                result.add("    ld      w0, [" + ctx.ir.getText() + "]");
                            }                            
                            result.add("    ld      w0, [w0]");
                        } else { /* byte type */
                            if (ctx.er != null)
                            {
                                result.addAll(ctx.er.accept(this));
                                if (parser.symbols.get(ctx.ir.getText()).type.isWord) {
                                    result.add("    shl     w0");
                                }
                                result.add("    add     w0, " + ctx.ir.getText());
                                result.add("    ld      w0, [w0]");
                            } else {
                                result.add("    ld      w0, [" + ctx.ir.getText() + "]");
                            }                            
                            result.add("    ld      b0, [w0]");
                            result.add("    eor     b1, b1");
                        }

                    } else { // '&'
                        if (ctx.er != null)
                        {
                            result.addAll(ctx.er.accept(this));
                            result.add("    add     w0, " + ctx.ir.getText());
                            if (parser.symbols.get(ctx.ir.getText()).type.isWord) {
                                result.add("    shl     w0");
                            }
                        } else {
                            result.add("    leaf_w0     " + ctx.ir.getText());
                        }
                    } 
                } else { // no '&' or '*'
                    if (ctx.aType.isWord || !ctx.aType.isByte) { // id is word or is not byte then consider as word
                        if (ctx.er == null) { // simple 'id' (not 'id[...]'')
                            result.add("    ld      w0,[" + ctx.ir.getText() + "]");
                        } else  { // array 'id[...]' (not simple 'id')
                            result.addAll(ctx.er.accept(this));
                            if (parser.symbols.get(ctx.ir.getText()).type.isWord) {
                            result.add("    shl     w0");
                            }   
                            result.add("    add     w0, " + ctx.ir.getText());
                            result.add("    ld      w0, [w0]");
                        }
                    } else { /* otherwise consider as byte */
                        if (ctx.er == null) { // simple 'id' (not 'id[...]'')
                            result.add("    ld      w0,[" + ctx.ir.getText() + "]");
                        } else  { // array 'id[...]' (not simple 'id')
                            result.addAll(ctx.er.accept(this));
                            if (parser.symbols.get(ctx.ir.getText()).type.isWord) {
                                result.add("    shl     w0");
                            }   
                            result.add("    add     w0, " + ctx.ir.getText());
                            result.add("    ld      b0, [w0]");
                            result.add("    eor     b1, b1");
                        }
                    }
                }
                break;
            case 6:
                result.addAll(ctx.getChild(8).accept(this));
                result.add("    push_w0");
            case 5:
                result.addAll(ctx.getChild(6).accept(this));
                result.add("    push_w0");            
            case 4:
                result.addAll(ctx.getChild(4).accept(this));
                result.add("    push_w0");              
            case 3:
                result.addAll(ctx.getChild(2).accept(this));
            case 2:
                switch (ctx.altNum)
                {
                    case 6:
                        result.add("    pop_w1");
                        result.add("    pop_w2");
                        result.add("    pop_w3");
                        break;
                    case 5:
                        result.add("    pop_w1");
                        result.add("    pop_w2");
                        break;
                    case 4:
                        result.add("    pop_w1");
                        break;
                    default: 
                        break;
                }
                result.add("    call        "+ctx.getChild(0).getText());
                switch (ctx.altNum)
                {
                    case 6:
                        result.add("    pop_w1");
                        result.add("    pop_w2");
                        result.add("    pop_w3");
                    case 5:
                        result.add("    pop_w1");
                        result.add("    pop_w2");
                    case 4:
                        result.add("    pop_w1");
                    break;
                    default: 
                        break;
                }                
                break;
            case 7:
                if (!ctx.getChild(0).getText().equals("w0")) { // if w1, w2 or w3
                    result.add("    mov     " + ctx.getChild(0).getText() +", w0");
                }
                break;
            case 8:
                if (!ctx.getChild(0).getText().equals("b0")) { // if w1, w2 or w3
                    result.add("    mov     " + ctx.getChild(0).getText() +", b0");
                }            
                break;
            case 9:
                result = ctx.getChild(0).accept(this);
                break;

        }
        return result;
    }

    @Override
    public List<Object> visitStatement(StatementContext ctx) {
        List<Object> result = new ArrayList<Object>();
        String labelcond;
        String labelcond2;
        String labelcondend;
        switch (ctx.altNum) {
            case 5: // for ( x ; y ; z ) s
                labelcond = getNextLabel();
                labelcond2 = getNextLabel();
                labelcondend = getNextLabel();
                result=addComment(result, ctx.getChild(0));
                result.addAll(ctx.getChild(2).accept(this)); // eval x
                result.add(labelcond);
                result.addAll(ctx.getChild(4).accept(this)); // eval y
                result.add("    or      w0, w0");
                result.add("    jnz     " + labelcond2);
                result.add("    jump    " + labelcondend);
                result.add(labelcond2);
                result.addAll(ctx.getChild(8).accept(this));
                result.addAll(ctx.getChild(6).accept(this));
                result.add("    jump    " + labelcond );
                result.add(labelcondend);
                result=addComment(result, ctx.getChild(0), "end");
                break;
            case 6: // if x y 
                labelcond = getNextLabel();
                labelcondend = getNextLabel();
                result=addComment(result, ctx.getChild(0)); // if comment
                result.addAll(ctx.getChild(1).accept(this));
                result.add("    or      w0, w0");
                result.add("    jnz     " + labelcond);
                result.add("    jump    " + labelcondend);
                result.add(labelcond);
                result.addAll(ctx.getChild(2).accept(this));
                result.add(labelcondend);
                result=addComment(result, ctx.getChild(0), "end");
                break;
            case 7: // if x y else z
                labelcond = getNextLabel();
                labelcond2 = getNextLabel();
                labelcondend = getNextLabel();
                result=addComment(result, ctx.getChild(0));
                result.addAll(ctx.getChild(1).accept(this));
                result.add("    or      w0, w0");
                result.add("    jnz     " + labelcond);
                result.add("    jump    " + labelcond2);
                result=addComment(result, ctx.getChild(0), "then");
                result.add(labelcond);
                result.addAll(ctx.getChild(2).accept(this));
                result.add("    jump    " + labelcondend);
                result=addComment(result, ctx.getChild(0), "else");
                result=addComment(result, ctx.getChild(3));
                result.add(labelcond2);
                result.addAll(ctx.getChild(4).accept(this));
                result.add(labelcondend);
                result=addComment(result, ctx.getChild(0), "end");
            break;
            case 8: // while x y
                labelcond = getNextLabel();
                labelcond2 = getNextLabel();
                labelcondend = getNextLabel();
                result=addComment(result, ctx.getChild(0));
                result=addComment(result, ctx.getChild(0), "condition");
                result.add(labelcond);
                result.addAll(ctx.getChild(1).accept(this));
                result.add("    or      w0, w0");
                result.add("    jnz     " + labelcond2);
                result.add("    jump    " + labelcondend);
                result=addComment(result, ctx.getChild(0), "begin statement");
                result.add(labelcond2);
                result.addAll(ctx.getChild(2).accept(this));
                result.add("    jump    " + labelcond);
                result=addComment(result, ctx.getChild(0), "end statement");
                result.add(labelcondend);
                result=addComment(result, ctx.getChild(0), "end");
                break;
            case 9: // do x while y
                labelcond = getNextLabel();
                labelcond2 = getNextLabel();
                labelcondend = getNextLabel();
                result=addComment(result, ctx.getChild(0));
                result=addComment(result, ctx.getChild(0), "begin statement");
                result.add(labelcond);
                result.addAll(ctx.getChild(1).accept(this));
                result=addComment(result, ctx.getChild(0), "end statement");
                result=addComment(result, ctx.getChild(0), "condition");
                result.addAll(ctx.getChild(2).accept(this));
                result.add("    cmp     w0,0");
                result.add("    jnz     " + labelcond2);
                result.add("    jump    " + labelcondend);
                result.add(labelcond2);
                result.add("    jump    " + labelcond);
                result.add(labelcondend);
                result=addComment(result, ctx.getChild(0), "end");
                break;
            default: 
                result.addAll(visitChildren(ctx));
                break;
        }
        return result;
    }

    @Override
    public List<Object> visitTargetvar(TargetvarContext ctx) {
        List<Object> result = new ArrayList<Object>();
        switch (ctx.altNum) {
            case 1:
                result=addComment(result, ctx.getChild(0).getChild(0));
                if (ctx.aType.isWord || !ctx.aType.isByte) { /* word id */
                    if (ctx.er == null) { 
                        result.add("    sr      w0, [" + ctx.getChild(0).getText()+ "]");
                    }  else {
                        result.add("    push_w1");
                        result.add("    push_w0");
                        result.addAll(ctx.er.accept(this));
                        result.add("    shl     w0");
                        result.add("    add     w0, "+ ctx.getChild(0).getText());
                        result.add("    pop_w1");
                        result.add("    sr  w1, [w0]");
                        result.add("    pop_w1");
                    }                 
                } else { /* byte id */
                    if (ctx.er == null) {
                        result.add("    sr      b0, [" + ctx.getChild(0).getText()+ "]");
                        result.add("    eor     b1, b1");
                    }  else {
                        result.add("    push_w1");
                        result.add("    push_w0");
                        result.addAll(ctx.er.accept(this));
                        result.add("    add     w0, "+ ctx.getChild(0).getText());
                        result.add("    pop_w1");
                        result.add("    sr      b2, [w0]");
                        result.add("    mov     b0, b2");
                        result.add("    eor     b1, b1");
                        result.add("    pop_w1");
                    }                 
                }
                break;
            case 2:
                result.add("    push_w1");
                result.add("    push_w0");
                result.addAll(ctx.er.accept(this));
                result.add("    pop_w1");
                if (ctx.aType.isWord || !ctx.aType.isByte) {
                    result.add("    sr      w1, [w0]");
                    result.add("    mov     w0, w1");
                } else {
                    result.add("    sr      b2, [w0]");
                    result.add("    mov     b0, b2");
                    result.add("    eor     b1, b1");
                }
                result.add("    pop_w1");
                break;
            case 3:
                result=addComment(result, ctx.getChild(0).getChild(0));
                if (!ctx.getChild(0).getText().equals("w0"))
                {
                    result.add("    mov     " + ctx.getChild(0).getText() + ", w0");
                }
                break;
            case 4:
                result=addComment(result, ctx.getChild(0).getChild(0));
                if (!ctx.getChild(0).getText().equals("b0"))
                {
                    result.add("    mov     " + ctx.getChild(0).getText() + ", b0");
                }
                break;            
        }
        return result;
    }

    @Override
    public List<Object> visitTerm(TermContext ctx) {
        return ctx.getChild(0).accept(this);
    }

    @Override
    public List<Object> visitType(TypeContext ctx) {
        // TODO Auto-generated method stub
        return super.visitType(ctx);
    }

    @Override
    public List<Object> visitUnaryexpr(UnaryexprContext ctx) {
        List<Object> result  = ctx.tr.accept(this);

        if (ctx.ope != null) {
            switch (ctx.ope.getText().charAt(0)) {
                case '-':
                    if (ctx.aType.isWord || !ctx.aType.isByte) {
                        result.add("    neg     w0");
                    } else {
                        result.add("    eor     b1, b1");
                        result.add("    neg     w0");
                    }
                    break;
                case '~':
                    if (ctx.aType.isWord || !ctx.aType.isByte) {
                        result.add("    not     w0");
                    } else {
                        result.add("    eor     b1, b1");
                        result.add("    not     w0");
                    }
                    break;
                case '!':
                    if (ctx.tr.aType.isWord || !ctx.tr.aType.isByte){ // term is Word
                        result.add("    or      w0, w0");
                    } else { // term is Byte
                        result.add("    or      b0, b0");
                    }
                    String lbl1 = getNextLabel();
                    String lbl2 = getNextLabel();
                    result.add("    jz      "+lbl1);
                    result.add("    eor     w0, w0");
                    result.add("    j       " + lbl2);
                    result.add(lbl1);
                    result.add("    mov     w0, 1");
                    result.add(lbl2);
                    result.add("    ");
                    break;
                default: 
                    break;
            }
        }
        return result;
    }

    @Override
    public List<Object> visit(ParseTree tree) {
        // TODO Auto-generated method stub
        return super.visit(tree);
    }

    @Override
    public List<Object> visitChildren(RuleNode node) {
        // TODO Auto-generated method stub
        return super.visitChildren(node);
    }

    @Override
    public List<Object> visitErrorNode(ErrorNode node) {
        // TODO Auto-generated method stub
        return super.visitErrorNode(node);
    }

    @Override
    public List<Object> visitTerminal(TerminalNode node) {
        return addComment(defaultResult(), node);
    }

    @Override
    public List<Object> visitStringdata(StringdataContext ctx) {
        List<Object> result = defaultResult();
        result.add("    mov     w0, "+ parser.stringdata.getBS3Label(ctx.strValue));
        return result;
    }

    @Override
    public List<Object> visitBs3asmlabel(Bs3asmlabelContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3asmlabel(ctx);
    }

    @Override
    public List<Object> visitBs3asmline(Bs3asmlineContext ctx) {
        List<Object> result = new ArrayList<Object>();
        result.add(ctx.getText());
        return result;
    }

    @Override
    public List<Object> visitBs3comment(Bs3commentContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3comment(ctx);
    }

    @Override
    public List<Object> visitBs3labelref(Bs3labelrefContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3labelref(ctx);
    }

    @Override
    public List<Object> visitBs3macro(Bs3macroContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3macro(ctx);
    }

    @Override
    public List<Object> visitBs3macrodef(Bs3macrodefContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3macrodef(ctx);
    }

    @Override
    public List<Object> visitBs3ope(Bs3opeContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3ope(ctx);
    }

    @Override
    public List<Object> visitBs3param(Bs3paramContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3param(ctx);
    }

    @Override
    public List<Object> visitBs3params(Bs3paramsContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3params(ctx);
    }

    @Override
    public List<Object> visitBs3value(Bs3valueContext ctx) {
        // TODO Auto-generated method stub
        return super.visitBs3value(ctx);
    }

    @Override
    public List<Object> visitStringdataraw(StringdatarawContext ctx) {
        // TODO Auto-generated method stub
        return super.visitStringdataraw(ctx);
    }

    @Override
    protected List<Object> aggregateResult(List<Object> aggregate, List<Object> nextResult) {
        aggregate.addAll(nextResult);
        return aggregate;
    }

    @Override
    protected List<Object> defaultResult() {
        return new ArrayList<Object>();
    }

    @Override
    protected boolean shouldVisitNextChild(RuleNode node, List<Object> currentResult) {
        // TODO Auto-generated method stub
        return super.shouldVisitNextChild(node, currentResult);
    }

    private String getNextLabel()
    {
        labelcounter++;
        return "__L"+ String.valueOf(labelcounter);
    }


}
