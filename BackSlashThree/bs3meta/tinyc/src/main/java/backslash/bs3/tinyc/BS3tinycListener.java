package backslash.bs3.tinyc;

import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.tree.ErrorNode;
import org.antlr.v4.runtime.tree.TerminalNode;

import backslash.bs3.tinyc.generated.tinycBaseListener;
import backslash.bs3.tinyc.generated.tinycParser.AddexprContext;
import backslash.bs3.tinyc.generated.tinycParser.AndexprContext;
import backslash.bs3.tinyc.generated.tinycParser.AssignexprContext;
import backslash.bs3.tinyc.generated.tinycParser.Bs3asmblockContext;
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
import backslash.bs3.tinyc.generated.tinycParser.TargetvarContext;
import backslash.bs3.tinyc.generated.tinycParser.TermContext;
import backslash.bs3.tinyc.generated.tinycParser.TypeContext;
import backslash.bs3.tinyc.generated.tinycParser.UnaryexprContext;



public class BS3tinycListener extends tinycBaseListener {

    @Override
    public void enterAddexpr(AddexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterAddexpr(ctx);
    }

    @Override
    public void enterAndexpr(AndexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterAndexpr(ctx);
    }

    @Override
    public void enterAssignexpr(AssignexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterAssignexpr(ctx);
    }

    @Override
    public void enterBs3asmblock(Bs3asmblockContext ctx) {
        // TODO Auto-generated method stub
        super.enterBs3asmblock(ctx);
    }

    @Override
    public void enterCastexpr(CastexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterCastexpr(ctx);
    }

    @Override
    public void enterCondexpr(CondexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterCondexpr(ctx);
    }

    @Override
    public void enterDeclare(DeclareContext ctx) {
        // TODO Auto-generated method stub
        super.enterDeclare(ctx);
    }

    @Override
    public void enterEquaexpr(EquaexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterEquaexpr(ctx);
    }

    @Override
    public void enterEveryRule(ParserRuleContext ctx) {
        // TODO Auto-generated method stub
        super.enterEveryRule(ctx);
    }

    @Override
    public void enterExcluORexpr(ExcluORexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterExcluORexpr(ctx);
    }

    @Override
    public void enterExpr(ExprContext ctx) {
        // TODO Auto-generated method stub
        super.enterExpr(ctx);
    }

    @Override
    public void enterId_(Id_Context ctx) {
        // TODO Auto-generated method stub
        super.enterId_(ctx);
    }

    @Override
    public void enterIncluORexpr(IncluORexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterIncluORexpr(ctx);
    }

    @Override
    public void enterInteger(IntegerContext ctx) {
        // TODO Auto-generated method stub
        super.enterInteger(ctx);
    }

    @Override
    public void enterJump(JumpContext ctx) {
        // TODO Auto-generated method stub
        super.enterJump(ctx);
    }

    @Override
    public void enterLabel(LabelContext ctx) {
        // TODO Auto-generated method stub
        super.enterLabel(ctx);
    }

    @Override
    public void enterLogicANDexpr(LogicANDexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterLogicANDexpr(ctx);
    }

    @Override
    public void enterLogicORexpr(LogicORexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterLogicORexpr(ctx);
    }

    @Override
    public void enterMulexpr(MulexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterMulexpr(ctx);
    }

    @Override
    public void enterParen_expr(Paren_exprContext ctx) {
        // TODO Auto-generated method stub
        super.enterParen_expr(ctx);
    }

    @Override
    public void enterProgram(ProgramContext ctx) {
        // TODO Auto-generated method stub
        super.enterProgram(ctx);
    }

    @Override
    public void enterRegByte(RegByteContext ctx) {
        // TODO Auto-generated method stub
        super.enterRegByte(ctx);
    }

    @Override
    public void enterRegWord(RegWordContext ctx) {
        // TODO Auto-generated method stub
        super.enterRegWord(ctx);
    }

    @Override
    public void enterRelatexpr(RelatexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterRelatexpr(ctx);
    }

    @Override
    public void enterReturnfct(ReturnfctContext ctx) {
        // TODO Auto-generated method stub
        super.enterReturnfct(ctx);
    }

    @Override
    public void enterShiftexpr(ShiftexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterShiftexpr(ctx);
    }

    @Override
    public void enterSourcevar(SourcevarContext ctx) {
        // TODO Auto-generated method stub
        super.enterSourcevar(ctx);
    }

    @Override
    public void enterStatement(StatementContext ctx) {
        // TODO Auto-generated method stub
        super.enterStatement(ctx);
    }

    @Override
    public void enterTargetvar(TargetvarContext ctx) {
        // TODO Auto-generated method stub
        super.enterTargetvar(ctx);
    }

    @Override
    public void enterTerm(TermContext ctx) {
        // TODO Auto-generated method stub
        super.enterTerm(ctx);
    }

    @Override
    public void enterType(TypeContext ctx) {
        // TODO Auto-generated method stub
        super.enterType(ctx);
    }

    @Override
    public void enterUnaryexpr(UnaryexprContext ctx) {
        // TODO Auto-generated method stub
        super.enterUnaryexpr(ctx);
    }

    @Override
    public void exitAddexpr(AddexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitAddexpr(ctx);
    }

    @Override
    public void exitAndexpr(AndexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitAndexpr(ctx);
    }

    @Override
    public void exitAssignexpr(AssignexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitAssignexpr(ctx);
    }

    @Override
    public void exitBs3asmblock(Bs3asmblockContext ctx) {
        // TODO Auto-generated method stub
        super.exitBs3asmblock(ctx);
    }

    @Override
    public void exitCastexpr(CastexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitCastexpr(ctx);
    }

    @Override
    public void exitCondexpr(CondexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitCondexpr(ctx);
    }

    @Override
    public void exitDeclare(DeclareContext ctx) {
        // TODO Auto-generated method stub
        super.exitDeclare(ctx);
    }

    @Override
    public void exitEquaexpr(EquaexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitEquaexpr(ctx);
    }

    @Override
    public void exitEveryRule(ParserRuleContext ctx) {
        // TODO Auto-generated method stub
        super.exitEveryRule(ctx);
    }

    @Override
    public void exitExcluORexpr(ExcluORexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitExcluORexpr(ctx);
    }

    @Override
    public void exitExpr(ExprContext ctx) {
        // TODO Auto-generated method stub
        super.exitExpr(ctx);
    }

    @Override
    public void exitId_(Id_Context ctx) {
        // TODO Auto-generated method stub
        super.exitId_(ctx);
    }

    @Override
    public void exitIncluORexpr(IncluORexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitIncluORexpr(ctx);
    }

    @Override
    public void exitInteger(IntegerContext ctx) {
        // TODO Auto-generated method stub
        super.exitInteger(ctx);
    }

    @Override
    public void exitJump(JumpContext ctx) {
        // TODO Auto-generated method stub
        super.exitJump(ctx);
    }

    @Override
    public void exitLabel(LabelContext ctx) {
        // TODO Auto-generated method stub
        super.exitLabel(ctx);
    }

    @Override
    public void exitLogicANDexpr(LogicANDexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitLogicANDexpr(ctx);
    }

    @Override
    public void exitLogicORexpr(LogicORexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitLogicORexpr(ctx);
    }

    @Override
    public void exitMulexpr(MulexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitMulexpr(ctx);
    }

    @Override
    public void exitParen_expr(Paren_exprContext ctx) {
        // TODO Auto-generated method stub
        super.exitParen_expr(ctx);
    }

    @Override
    public void exitProgram(ProgramContext ctx) {
        // TODO Auto-generated method stub
        super.exitProgram(ctx);
    }

    @Override
    public void exitRegByte(RegByteContext ctx) {
        // TODO Auto-generated method stub
        super.exitRegByte(ctx);
    }

    @Override
    public void exitRegWord(RegWordContext ctx) {
        // TODO Auto-generated method stub
        super.exitRegWord(ctx);
    }

    @Override
    public void exitRelatexpr(RelatexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitRelatexpr(ctx);
    }

    @Override
    public void exitReturnfct(ReturnfctContext ctx) {
        // TODO Auto-generated method stub
        super.exitReturnfct(ctx);
    }

    @Override
    public void exitShiftexpr(ShiftexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitShiftexpr(ctx);
    }

    @Override
    public void exitSourcevar(SourcevarContext ctx) {
        // TODO Auto-generated method stub
        super.exitSourcevar(ctx);
    }

    @Override
    public void exitStatement(StatementContext ctx) {
        // TODO Auto-generated method stub
        super.exitStatement(ctx);
    }

    @Override
    public void exitTargetvar(TargetvarContext ctx) {
        // TODO Auto-generated method stub
        super.exitTargetvar(ctx);
    }

    @Override
    public void exitTerm(TermContext ctx) {
        // TODO Auto-generated method stub
        super.exitTerm(ctx);
    }

    @Override
    public void exitType(TypeContext ctx) {
        // TODO Auto-generated method stub
        super.exitType(ctx);
    }

    @Override
    public void exitUnaryexpr(UnaryexprContext ctx) {
        // TODO Auto-generated method stub
        super.exitUnaryexpr(ctx);
    }

    @Override
    public void visitErrorNode(ErrorNode node) {
        // TODO Auto-generated method stub
        super.visitErrorNode(node);
    }

    @Override
    public void visitTerminal(TerminalNode node) {
        // TODO Auto-generated method stub
        super.visitTerminal(node);
    }

}
