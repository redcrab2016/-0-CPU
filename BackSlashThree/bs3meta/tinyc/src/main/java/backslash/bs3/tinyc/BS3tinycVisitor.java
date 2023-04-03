package backslash.bs3.tinyc;

import java.util.ArrayList;
import java.util.List;

import org.antlr.v4.runtime.tree.ErrorNode;
import org.antlr.v4.runtime.tree.ParseTree;
import org.antlr.v4.runtime.tree.RuleNode;
import org.antlr.v4.runtime.tree.TerminalNode;

import backslash.bs3.tinyc.generated.tinycBaseVisitor;
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


public class BS3tinycVisitor extends tinycBaseVisitor<List<Object>> {

    @Override
    public List<Object> visitAddexpr(AddexprContext ctx) {
        // TODO Auto-generated method stub
        ctx.addSymbol("",null,true);
        return super.visitAddexpr(ctx);
    }

    @Override
    public List<Object> visitAndexpr(AndexprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitAndexpr(ctx);
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
        // TODO Auto-generated method stub
        return super.visitCastexpr(ctx);
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
        // TODO Auto-generated method stub
        return super.visitEquaexpr(ctx);
    }

    @Override
    public List<Object> visitExcluORexpr(ExcluORexprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitExcluORexpr(ctx);
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
        // TODO Auto-generated method stub
        return super.visitIncluORexpr(ctx);
    }

    @Override
    public List<Object> visitInteger(IntegerContext ctx) {
        // TODO Auto-generated method stub
        return super.visitInteger(ctx);
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
        // TODO Auto-generated method stub
        return super.visitLogicANDexpr(ctx);
    }

    @Override
    public List<Object> visitLogicORexpr(LogicORexprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitLogicORexpr(ctx);
    }

    @Override
    public List<Object> visitMulexpr(MulexprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitMulexpr(ctx);
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
        // TODO Auto-generated method stub
        return super.visitRegByte(ctx);
    }

    @Override
    public List<Object> visitRegWord(RegWordContext ctx) {
        // TODO Auto-generated method stub
        return super.visitRegWord(ctx);
    }

    @Override
    public List<Object> visitRelatexpr(RelatexprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitRelatexpr(ctx);
    }

    @Override
    public List<Object> visitReturnfct(ReturnfctContext ctx) {
        // TODO Auto-generated method stub
        return super.visitReturnfct(ctx);
    }

    @Override
    public List<Object> visitShiftexpr(ShiftexprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitShiftexpr(ctx);
    }

    @Override
    public List<Object> visitSourcevar(SourcevarContext ctx) {
        // TODO Auto-generated method stub
        return super.visitSourcevar(ctx);
    }

    @Override
    public List<Object> visitStatement(StatementContext ctx) {
        // TODO Auto-generated method stub
        return super.visitStatement(ctx);
    }

    @Override
    public List<Object> visitTargetvar(TargetvarContext ctx) {
        // TODO Auto-generated method stub
        return super.visitTargetvar(ctx);
    }

    @Override
    public List<Object> visitTerm(TermContext ctx) {
        // TODO Auto-generated method stub
        return super.visitTerm(ctx);
    }

    @Override
    public List<Object> visitType(TypeContext ctx) {
        // TODO Auto-generated method stub
        return super.visitType(ctx);
    }

    @Override
    public List<Object> visitUnaryexpr(UnaryexprContext ctx) {
        // TODO Auto-generated method stub
        return super.visitUnaryexpr(ctx);
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


}
