package backslash.bs3.tinyc;

import java.util.ArrayList;
import java.util.List;

import org.antlr.v4.runtime.tree.ErrorNode;
import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.RuleNode;
import org.antlr.v4.runtime.tree.TerminalNode;

import backslash.bs3.tinyc.generated.tinycBaseVisitor;
import backslash.bs3.tinyc.generated.tinycParser.ExprContext;
import backslash.bs3.tinyc.generated.tinycParser.Id_Context;
import backslash.bs3.tinyc.generated.tinycParser.IntegerContext;
import backslash.bs3.tinyc.generated.tinycParser.Paren_exprContext;
import backslash.bs3.tinyc.generated.tinycParser.ProgramContext;
import backslash.bs3.tinyc.generated.tinycParser.StatementContext;
import backslash.bs3.tinyc.generated.tinycParser.Sum_Context;
import backslash.bs3.tinyc.generated.tinycParser.TermContext;
import backslash.bs3.tinyc.generated.tinycParser.TestContext;

public class BS3tinycVisitor extends tinycBaseVisitor<ArrayList<Object>> {

    private int _labelcnt;
    public BS3tinycVisitor()
    {
        _labelcnt = 0;
    }

    @Override
    public ArrayList<Object> visitExpr(ExprContext ctx) {
        ArrayList<Object> result = new ArrayList<Object>();
        for (int i = ctx.getChildCount()-1; i >=0 ; i-- ) /* reverse aggregate */
        {
			ParseTree c = ctx.getChild(i);
			ArrayList<Object> childResult = c.accept(this);
			result = aggregateResult(result, childResult);
        }
        return result;
    }

    @Override
    public ArrayList<Object> visitId_(Id_Context ctx) {
        ArrayList<Object> result  = new ArrayList<Object>();
        if (ctx.getParent() instanceof TermContext) {
            result.add("    ld      w0, [" + ctx.getText() + "]");
        } else {
            result.add("    sr      w0, [" + ctx.getText() + "]");
        }
        return result;
    }

    @Override
    public ArrayList<Object> visitInteger(IntegerContext ctx) {
        ArrayList<Object> result = new ArrayList<Object>();
        result.add("    mov     w0, " + ctx.getText());
        return result;
    }

    @Override
    public ArrayList<Object> visitParen_expr(Paren_exprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitParen_expr(ctx);
    }

    @Override
    public ArrayList<Object> visitProgram(ProgramContext ctx) {
        // TODO Auto-generated method stub
        return super.visitProgram(ctx);
    }

    @Override
    public ArrayList<Object> visitStatement(StatementContext ctx) {
        ArrayList<Object> result = new ArrayList<Object>();
        ParseTree c;
        ArrayList<Object> childResult;
        String label ="._statement";
        switch(ctx.subctx)
        {
            case 1: /* IF x y */
                _labelcnt++;
                String labelif = label + String.valueOf(_labelcnt);
                _labelcnt++;
                String labelif2 = label + String.valueOf(_labelcnt);
                _labelcnt++;
                String labelendif = label + String.valueOf(_labelcnt);            
                c = ctx.getChild(1);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    cmp     w0, 0");
                result.add("    jnz     "+labelif);
                result.add("    jump    "+labelendif); 
                result.add(labelif);
                c = ctx.getChild(2);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add(labelendif);
                break;
            case 2: /* IF x y ELSE z */
                _labelcnt++;
                labelif = label + String.valueOf(_labelcnt);
                _labelcnt++;
                labelif2 = label + String.valueOf(_labelcnt);
                _labelcnt++;
                labelendif = label + String.valueOf(_labelcnt);            
                c = ctx.getChild(1);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    cmp     w0, 0");
                result.add("    jnz     "+labelif);
                result.add("    jump    "+labelif2); 
                result.add(labelif);
                c = ctx.getChild(2);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    jump    "+labelendif);
                result.add(labelif2);
                c = ctx.getChild(4);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add(labelendif);
                break;
            case 3: /* WHILE x y */
                _labelcnt++;
                String labelwhile = label + String.valueOf(_labelcnt);
                _labelcnt++;
                String labelwhile2 = label + String.valueOf(_labelcnt);
                _labelcnt++;
                String labelendwhile = label + String.valueOf(_labelcnt);
                result.add(labelwhile);
                c = ctx.getChild(1);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    cmp     w0, 0");
                result.add("    jnz     "+labelwhile2);
                result.add("    jump    "+labelendwhile);
                result.add(labelwhile2);
                c = ctx.getChild(2);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    jump    "+labelwhile);
                result.add(labelendwhile);
                break;
            case 4: /* DO x WHILE y */
                _labelcnt++;
                labelwhile = label + String.valueOf(_labelcnt);
                _labelcnt++;
                labelwhile2 = label + String.valueOf(_labelcnt);
                _labelcnt++;
                labelendwhile = label + String.valueOf(_labelcnt);
                result.add(labelwhile);
                c = ctx.getChild(1);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                c = ctx.getChild(3);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    cmp     w0, 0");
                result.add("    jz     "+labelwhile2);
                result.add("    jump    "+labelwhile);
                result.add(labelwhile2);
                break;
            case 5: /* { x } */
                c = ctx.getChild(1);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                break;
            case 6: /* EXPRESSION */
                c = ctx.getChild(0);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                break;
            default: 
                break;
        }
        return result;
    }

    @Override
    public ArrayList<Object> visitSum_(Sum_Context ctx) {
        ArrayList<Object> result = new ArrayList<Object>();
        ParseTree c;
        ArrayList<Object> childResult;
        switch(ctx.subctx)
        {
            case 1:
                c = ctx.getChild(0);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                break;
            case 2: /* + */
                c = ctx.getChild(0);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    push_w0");
                c = ctx.getChild(2);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    pop_w1");
                result.add("    add     w0, w1");
                break;
            case 3: /* - */
                c = ctx.getChild(2);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    push_w0");
                c = ctx.getChild(0);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    pop_w1");
                result.add("    sub     w0, w1");
                break;
        }
        return result;
    }

    @Override
    public ArrayList<Object> visitTerm(TermContext ctx) {
        // TODO Auto-generated method stub
        return super.visitTerm(ctx);
    }

    @Override
    public ArrayList<Object> visitTest(TestContext ctx) {
        ArrayList<Object> result = new ArrayList<Object>();
        ParseTree c;
        ArrayList<Object> childResult;
        String label ="._test";
        switch(ctx.subctx)
        {
            case 1:
                c = ctx.getChild(0);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                break;
            case 2: /* < */
                c = ctx.getChild(0);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    push_w0");
                c = ctx.getChild(2);
                childResult = c.accept(this);
                result = aggregateResult(result, childResult);
                result.add("    pop_w1");
                result.add("    cmp     w1, w0");
                _labelcnt++;
                label = label + String.valueOf(_labelcnt);
                result.add("    eor     w0, w0");
                result.add("    jc     " + label );
                result.add("    inc     w0");
                result.add(label);

                break;
        }
        return result;
    }

    @Override
    protected ArrayList<Object> aggregateResult(ArrayList<Object> aggregate, ArrayList<Object> nextResult) {
        aggregate.addAll(nextResult);
        return aggregate;
    }

    @Override
    protected ArrayList<Object> defaultResult() {
        return new ArrayList<Object>();
    }

    @Override
    protected boolean shouldVisitNextChild(RuleNode node, ArrayList<Object> currentResult) {
        // TODO Auto-generated method stub
        return super.shouldVisitNextChild(node, currentResult);
    }

    @Override
    public ArrayList<Object> visit(ParseTree tree) {
        /* TODO add prolog/epilog in ArrayList */
        return super.visit(tree);
    }

    @Override
    public ArrayList<Object> visitChildren(RuleNode node) {
        // TODO Auto-generated method stub
        return super.visitChildren(node);
    }

    @Override
    public ArrayList<Object> visitErrorNode(ErrorNode node) {
        ArrayList<Object> result = new ArrayList<Object>();
        result.add("; ERROR on : "+ node.getText());
        return super.visitErrorNode(node);
    }

    @Override
    public ArrayList<Object> visitTerminal(TerminalNode node) {
        // TODO Auto-generated method stub
        return super.visitTerminal(node);
    }


}
