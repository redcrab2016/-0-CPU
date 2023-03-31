package backslash.bs3.tinyc;

import org.antlr.v4.runtime.BailErrorStrategy;
import org.antlr.v4.runtime.FailedPredicateException;
import org.antlr.v4.runtime.InputMismatchException;
import org.antlr.v4.runtime.NoViableAltException;
import org.antlr.v4.runtime.Parser;
import org.antlr.v4.runtime.RecognitionException;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.misc.IntervalSet;
public class BS3tinycErrorStrategy extends BailErrorStrategy {

    @Override
    public void recover(Parser recognizer, RecognitionException e) {
        // TODO Auto-generated method stub
        super.recover(recognizer, e);
    }

    @Override
    public Token recoverInline(Parser recognizer) throws RecognitionException {
        // TODO Auto-generated method stub
        return super.recoverInline(recognizer);
    }

    @Override
    public void sync(Parser recognizer) {
        // TODO Auto-generated method stub
        super.sync(recognizer);
    }

    @Override
    protected void beginErrorCondition(Parser recognizer) {
        // TODO Auto-generated method stub
        super.beginErrorCondition(recognizer);
    }

    @Override
    protected void consumeUntil(Parser recognizer, IntervalSet set) {
        // TODO Auto-generated method stub
        super.consumeUntil(recognizer, set);
    }

    @Override
    protected void endErrorCondition(Parser recognizer) {
        // TODO Auto-generated method stub
        super.endErrorCondition(recognizer);
    }

    @Override
    protected String escapeWSAndQuote(String s) {
        // TODO Auto-generated method stub
        return super.escapeWSAndQuote(s);
    }

    @Override
    protected IntervalSet getErrorRecoverySet(Parser recognizer) {
        // TODO Auto-generated method stub
        return super.getErrorRecoverySet(recognizer);
    }

    @Override
    protected IntervalSet getExpectedTokens(Parser recognizer) {
        // TODO Auto-generated method stub
        return super.getExpectedTokens(recognizer);
    }

    @Override
    protected Token getMissingSymbol(Parser recognizer) {
        // TODO Auto-generated method stub
        return super.getMissingSymbol(recognizer);
    }

    @Override
    protected String getSymbolText(Token symbol) {
        // TODO Auto-generated method stub
        return super.getSymbolText(symbol);
    }

    @Override
    protected int getSymbolType(Token symbol) {
        // TODO Auto-generated method stub
        return super.getSymbolType(symbol);
    }

    @Override
    protected String getTokenErrorDisplay(Token t) {
        // TODO Auto-generated method stub
        return super.getTokenErrorDisplay(t);
    }

    @Override
    public boolean inErrorRecoveryMode(Parser recognizer) {
        // TODO Auto-generated method stub
        return super.inErrorRecoveryMode(recognizer);
    }

    @Override
    public void reportError(Parser recognizer, RecognitionException e) {
        // TODO Auto-generated method stub
        super.reportError(recognizer, e);
    }

    @Override
    protected void reportFailedPredicate(Parser recognizer, FailedPredicateException e) {
        // TODO Auto-generated method stub
        super.reportFailedPredicate(recognizer, e);
    }

    @Override
    protected void reportInputMismatch(Parser recognizer, InputMismatchException e) {
        // TODO Auto-generated method stub
        super.reportInputMismatch(recognizer, e);
    }

    @Override
    public void reportMatch(Parser recognizer) {
        // TODO Auto-generated method stub
        super.reportMatch(recognizer);
    }

    @Override
    protected void reportMissingToken(Parser recognizer) {
        // TODO Auto-generated method stub
        super.reportMissingToken(recognizer);
    }

    @Override
    protected void reportNoViableAlternative(Parser recognizer, NoViableAltException e) {
        // TODO Auto-generated method stub
        super.reportNoViableAlternative(recognizer, e);
    }

    @Override
    protected void reportUnwantedToken(Parser recognizer) {
        // TODO Auto-generated method stub
        super.reportUnwantedToken(recognizer);
    }

    @Override
    public void reset(Parser recognizer) {
        // TODO Auto-generated method stub
        super.reset(recognizer);
    }

    @Override
    protected Token singleTokenDeletion(Parser recognizer) {
        // TODO Auto-generated method stub
        return super.singleTokenDeletion(recognizer);
    }

    @Override
    protected boolean singleTokenInsertion(Parser recognizer) {
        // TODO Auto-generated method stub
        return super.singleTokenInsertion(recognizer);
    }
    
}
