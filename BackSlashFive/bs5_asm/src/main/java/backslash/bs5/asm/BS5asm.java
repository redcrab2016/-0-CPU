package backslash.bs5.asm;

import  backslash.bs5.asm.generated.*;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;

import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.CharStreams;
import org.antlr.v4.runtime.CommonTokenStream;

public class BS5asm {
    public static void main( String[] args )
    {   
        try 
        {
            String inputFile = null;
            String outputFileNameBase = "out";
            if ( args.length>0 ) inputFile = args[0]; 
            InputStream is = System.in;
            if ( inputFile!=null ) {
                is = new FileInputStream(inputFile);
                File ifile = new File(inputFile);
                outputFileNameBase = ifile.getName();
                int lastDotindex = outputFileNameBase.lastIndexOf(".");
                if (lastDotindex >0) {
                    outputFileNameBase = outputFileNameBase.substring(0,lastDotindex);
                } 
                outputFileNameBase = (ifile.getParent() == null)?"":(ifile.getParent()+"/") + outputFileNameBase;
            }
            CharStream input = CharStreams.fromStream(is);
            BS5ErrorListener errorListener = new BS5ErrorListener();
            // prepare the lexer with its dedicated error listener
            asmLexer lexer = new asmLexer(input);
            lexer.removeErrorListeners();
            lexer.addErrorListener(errorListener);
            // prepare the token stream
            CommonTokenStream tokens = new CommonTokenStream(lexer);
            // prepare the parser with its token stream and error listener
            asmParser parser = new asmParser(tokens);
            parser.removeErrorListeners();
            parser.addErrorListener(errorListener);
            // launch the parsing
            //ParseTree tree = 
            parser.program(); // parse 
            //System.out.println("tree:" +tree..toStringTree());

            if (errorListener.hasError()) { // lexical and syntaxic error report
                int nbError = errorListener.getReport().size();
                System.out.println("Report " + String.valueOf(nbError) + " error" + (nbError>1?"s":""));
                int i = 1;
                for (String msg : errorListener.getReport()) {
                    System.out.println("  #"+ String.valueOf(i++) + " " + msg);
                }
                System.exit(1);
            } else {
                BS5program prg = parser.prg;
                if (prg.getNbException() >0) { // semantic error report
                    prg.report(System.out);
                    System.exit(1);
                }
                // All looks ok, then generate the assembly report
                FileOutputStream fos = new FileOutputStream(new File(outputFileNameBase+".rpt"));
                prg.report(fos);
                fos.close();
                // generate the Verilog bin file to be used with $readmemh
                fos = new FileOutputStream(new File(outputFileNameBase+".bin"));
                prg.genVerilogReadmemh(fos);
                fos.close();
                System.exit(0);
            }
        } catch (Exception e)
        {
            e.printStackTrace();
        }
    }    
}

