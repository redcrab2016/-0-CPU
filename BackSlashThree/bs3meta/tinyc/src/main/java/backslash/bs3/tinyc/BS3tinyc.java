package backslash.bs3.tinyc;
import backslash.bs3.tinyc.generated.*;

import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;
import org.antlr.v4.runtime.ParserRuleContext;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.tree.*;

import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;


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

            tinycLexer lexer = new tinycLexer(input);
            lexer.addErrorListener(new BS3tinycErrorListener());
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            tinycParser parser = new tinycParser(tokens);
            parser.addErrorListener(new BS3tinycErrorListener());
            ParseTree tree = parser.program(); // parse 
            BS3tinycVisitor visitor = new BS3tinycVisitor();
            //tree.accept(visitor);
            //visitor.visitProgram(tree);
            ArrayList<Object> result = visitor.visit(tree);

            for (Object O : result)
            {
                System.out.println("==>'"+ O.toString() + "'");
            }
            //ParseTreeWalker walker = new ParseTreeWalker(); // create standard walker
            //tinycListener extractor = new BS3tinycListener(System.out);
            //parser.addParseListener(extractor);
            //walker.walk(extractor, tree); // initiate walk of tree with listener
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }
}
