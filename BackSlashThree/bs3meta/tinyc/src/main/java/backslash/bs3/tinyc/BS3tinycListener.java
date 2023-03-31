package backslash.bs3.tinyc;

import java.io.OutputStream;
import java.io.PrintStream;

import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ErrorNode;
import org.antlr.v4.runtime.tree.TerminalNode;

import backslash.bs3.tinyc.generated.tinycBaseListener;
import backslash.bs3.tinyc.generated.tinycParser.ExprContext;
import backslash.bs3.tinyc.generated.tinycParser.Id_Context;
import backslash.bs3.tinyc.generated.tinycParser.IntegerContext;
import backslash.bs3.tinyc.generated.tinycParser.Paren_exprContext;
import backslash.bs3.tinyc.generated.tinycParser.ProgramContext;
import backslash.bs3.tinyc.generated.tinycParser.StatementContext;
import backslash.bs3.tinyc.generated.tinycParser.Sum_Context;
import backslash.bs3.tinyc.generated.tinycParser.TermContext;
import backslash.bs3.tinyc.generated.tinycParser.TestContext;

public class BS3tinycListener extends tinycBaseListener {
    private PrintStream ps;

    BS3tinycListener(OutputStream _os)
    {
        this.ps = (_os instanceof PrintStream)?((PrintStream)_os):(new PrintStream(_os));
    }

    @Override
    public void enterEveryRule(ParserRuleContext ctx) {
        // TODO Auto-generated method stub
     //   ps.println(ctx);
        super.enterEveryRule(ctx);
    }

    @Override
    public void enterExpr(ExprContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterExpr(ctx);
    }

    @Override
    public void enterId_(Id_Context ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterId_(ctx);
    }

    @Override
    public void enterInteger(IntegerContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterInteger(ctx);
    }

    @Override
    public void enterParen_expr(Paren_exprContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterParen_expr(ctx);
    }

    @Override
    public void enterProgram(ProgramContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterProgram(ctx);
    }

    @Override
    public void enterStatement(StatementContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterStatement(ctx);
    }

    @Override
    public void enterSum_(Sum_Context ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterSum_(ctx);
    }

    @Override
    public void enterTerm(TermContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterTerm(ctx);
    }

    @Override
    public void enterTest(TestContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.enterTest(ctx);
    }

    @Override
    public void exitEveryRule(ParserRuleContext ctx) {
        // TODO Auto-generated method stub
        //ps.println(ctx);
        super.exitEveryRule(ctx);
    }

    @Override
    public void exitExpr(ExprContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitExpr(ctx);
    }

    @Override
    public void exitId_(Id_Context ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitId_(ctx);
    }

    @Override
    public void exitInteger(IntegerContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitInteger(ctx);
    }

    @Override
    public void exitParen_expr(Paren_exprContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitParen_expr(ctx);
    }

    @Override
    public void exitProgram(ProgramContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitProgram(ctx);
    }

    @Override
    public void exitStatement(StatementContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitStatement(ctx);
    }

    @Override
    public void exitSum_(Sum_Context ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitSum_(ctx);
    }

    @Override
    public void exitTerm(TermContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitTerm(ctx);
    }

    @Override
    public void exitTest(TestContext ctx) {
        // TODO Auto-generated method stub
        ps.println(ctx);
        super.exitTest(ctx);
    }

    @Override
    public void visitErrorNode(ErrorNode node) {
        // TODO Auto-generated method stub
        ps.println(node);
        super.visitErrorNode(node);
    }

    @Override
    public void visitTerminal(TerminalNode node) {
        // TODO Auto-generated method stub
        ps.println("Terminal:" + String.valueOf(node));
        super.visitTerminal(node);
    }

   
   
}
