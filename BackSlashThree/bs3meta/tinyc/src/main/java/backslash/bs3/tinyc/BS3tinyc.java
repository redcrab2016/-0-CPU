package backslash.bs3.tinyc;
import backslash.bs3.tinyc.generated.*;

import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.tree.*;

import java.io.FileInputStream;
import java.io.InputStream;
import java.util.List;


/**
 * Hello world!
 *
 */
public class BS3tinyc 
{
    public static void main( String[] args )
    {
        try 
        {
            String inputFile = null;
            if ( args.length>0 ) inputFile = args[0];
            InputStream is = System.in;
            if ( inputFile!=null ) {
                is = new FileInputStream(inputFile);
            }
            CharStream input = CharStreams.fromStream(is);

            BS3tinycErrorListener errorListener;

            tinycLexer lexer = new tinycLexer(input);
            lexer.removeErrorListeners();

            errorListener = new BS3tinycErrorListener();

            lexer.addErrorListener(errorListener);
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            tinycParser parser = new tinycParser(tokens);
            parser.removeErrorListeners();
            parser.addErrorListener(errorListener);
            ParseTree tree = parser.program(); // parse 
            //System.out.println("tree:" +tree..toStringTree());

            if (errorListener.hasError()) {
                int nbError = errorListener.getReport().size();
                System.out.println("Report " + String.valueOf(nbError) + " error" + (nbError>1?"s":""));
                int i = 1;
                for (String msg : errorListener.getReport()) {
                    System.out.println("  #"+ String.valueOf(i++) + " " + msg);
                }
                System.exit(1);
            } else {
                BS3tinycVisitor visitor = new BS3tinycVisitor(parser);
                List<Object> result = visitor.visit(tree);
                System.out.println("; BS3 Assembly code result ");
                for (Object O : result)
                {
                    System.out.println(O.toString() );
                }
                System.exit(0);
            }
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
