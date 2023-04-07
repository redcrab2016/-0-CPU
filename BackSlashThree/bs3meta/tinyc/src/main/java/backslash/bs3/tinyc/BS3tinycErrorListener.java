package backslash.bs3.tinyc;

import java.util.ArrayList;
import java.util.BitSet;
import java.util.List;

import org.antlr.v4.runtime.BaseErrorListener;
import org.antlr.v4.runtime.Parser;
import org.antlr.v4.runtime.RecognitionException;
import org.antlr.v4.runtime.Recognizer;
import org.antlr.v4.runtime.atn.ATNConfigSet;
import org.antlr.v4.runtime.dfa.DFA;

public class BS3tinycErrorListener extends BaseErrorListener  {

    private boolean hasError;
    private List<String> errorReport;
    public BS3tinycErrorListener() {
        super();
        hasError = false;
        errorReport = new ArrayList<String>();
    }

    public boolean hasError() { return hasError; }

    public List<String> getReport() { return errorReport;}

    @Override
    public void reportAmbiguity(Parser recognizer, DFA dfa, int startIndex, int stopIndex, boolean exact,
            BitSet ambigAlts, ATNConfigSet configs) {
        // TODO Auto-generated method stub
        super.reportAmbiguity(recognizer, dfa, startIndex, stopIndex, exact, ambigAlts, configs);
    }

    @Override
    public void reportAttemptingFullContext(Parser recognizer, DFA dfa, int startIndex, int stopIndex,
            BitSet conflictingAlts, ATNConfigSet configs) {
        // TODO Auto-generated method stub
        super.reportAttemptingFullContext(recognizer, dfa, startIndex, stopIndex, conflictingAlts, configs);
    }

    @Override
    public void reportContextSensitivity(Parser recognizer, DFA dfa, int startIndex, int stopIndex, int prediction,
            ATNConfigSet configs) {
        // TODO Auto-generated method stub
        super.reportContextSensitivity(recognizer, dfa, startIndex, stopIndex, prediction, configs);
    }

    @Override
    public void syntaxError(Recognizer<?, ?> recognizer, Object offendingSymbol, int line, int charPositionInLine,
            String msg, RecognitionException e) {
        hasError = true;
        errorReport.add("Line " + String.valueOf(line) + ":" + String.valueOf(charPositionInLine) + " " + msg);
        super.syntaxError(recognizer, offendingSymbol, line, charPositionInLine, msg, e);
    }
    
}
