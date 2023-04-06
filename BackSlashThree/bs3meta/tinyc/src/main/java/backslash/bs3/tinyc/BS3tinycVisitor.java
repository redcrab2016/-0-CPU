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

    @Override
    public List<Object> visitAddexpr(AddexprContext ctx) {
        List<Object> result = ctx.me1.accept(this);
        int i;
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result.add("    push_w1");
            result.add("    push_w0");
            result.add(ctx.getChild(i+1).accept(this));
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
            result.add(ctx.getChild(i+1).accept(this));
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
        // TODO Auto-generated method stub
        return super.visitAssignexpr(ctx);
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
        // TODO Auto-generated method stub
        return super.visitCondexpr(ctx);
    }

    @Override
    public List<Object> visitDeclare(DeclareContext ctx) {
        // TODO Auto-generated method stub
        return super.visitDeclare(ctx);
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
            labelcond=getNextLabel();
            labelcondend=getNextLabel();
            result.add("    push_w0");
            result.add(ctx.getChild(i+1).accept(this));
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
            result.add("    push_w0");
            result.add(ctx.getChild(i+1).accept(this));
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
        // TODO Auto-generated method stub
        return super.visitExpr(ctx);
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
            result.add("    push_w0");
            result.add(ctx.getChild(i+1).accept(this));
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
        result.add("    mov     w0, "+ctx.strValue);
        return result;
    }

    @Override
    public List<Object> visitJump(JumpContext ctx) {
        // TODO Auto-generated method stub
        
        return super.visitJump(ctx);
    }

    @Override
    public List<Object> visitLabel(LabelContext ctx) {
        // TODO Auto-generated method stub
        return super.visitLabel(ctx);
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
                labelcond=getNextLabel();
                result.add(ctx.getChild(i+1).accept(this));
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
        // TODO Auto-generated method stub
        return super.visitLogicORexpr(ctx);
    }

    @Override
    public List<Object> visitMulexpr(MulexprContext ctx) {
        List<Object> result = ctx.ce1.accept(this);
        int i;
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result.add("    push_w1");
            result.add("    push_w0");
            result.add(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
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
        // TODO Auto-generated method stub
        return super.visitParen_expr(ctx);
    }

    @Override
    public List<Object> visitProgram(ProgramContext ctx) {
        // TODO Auto-generated method stub
        return super.visitProgram(ctx);
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
            result.add(ctx.getChild(i+1).accept(this));
            senTypeRight = ((ShiftexprContext)ctx.getChild(i+1)).aType;
            result.add("    pop_w1");
            result.add("    cmp     w1, w0");
            // 'left' ope 'right' , w1=left, w0=right
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
            result.add("  eor       w0, w0");
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
        // TODO Auto-generated method stub
        return super.visitReturnfct(ctx);
    }

    @Override
    public List<Object> visitShiftexpr(ShiftexprContext ctx) {
        List<Object> result = ctx.ae1.accept(this);
        int i;
        for (i = 1 ; i < ctx.getChildCount() ; i+=2) {
            result.add("    push_w1");
            result.add("    push_w0");
            result.add(ctx.getChild(i+1).accept(this));
            result.add("    pop_w1");
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
                break;
            default: 
                break;
        }
        switch (ctx.altNum)
        {
            case 1:
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
        return super.visitSourcevar(ctx);
    }

    @Override
    public List<Object> visitStatement(StatementContext ctx) {
        // TODO Auto-generated method stub
        return super.visitStatement(ctx);
    }

    @Override
    public List<Object> visitTargetvar(TargetvarContext ctx) {
        List<Object> result = new ArrayList<Object>();
        switch (ctx.altNum) {
            case 1:
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
                if (!ctx.getChild(0).getText().equals("w0"))
                {
                    result.add("    mov     " + ctx.getChild(0).getText() + ", w0");
                }
                break;
            case 4:
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
        // TODO Auto-generated method stub
        return super.visitTerminal(node);
    }

    @Override
    public List<Object> visitStringdata(StringdataContext ctx) {
        List<Object> result = new ArrayList<Object>();
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
        // TODO Auto-generated method stub
        return super.visitBs3asmline(ctx);
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
