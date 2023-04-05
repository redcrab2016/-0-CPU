grammar tinyc;

options {contextSuperClass=org.antlr.v4.runtime.RuleContextWithAltNum;}

@header {
import java.util.*;
import backslash.bs3.tinyc.*;
}

@parser::members {
public Map<String,BS3tinycSymbol> symbols = new HashMap<String,BS3tinycSymbol>();
public BS3tinycStringData stringdata = new BS3tinycStringData();
public boolean addSymbol(String name, BS3tinycType aType, boolean isExtern, String size)
{
   if (symbols.containsKey(name)) return false;
   BS3tinycSymbol s = new  BS3tinycSymbol(name,aType, isExtern,size);
   symbols.put(name, s);
   return true;
}

public boolean addSymbol(String name, BS3tinycType aType, boolean isExtern)
{
  return addSymbol(name, aType, isExtern, "1");
}
}

program
   : statement + EOF 
   ;

statement
   : declare ';'                                      
   | label                                            
   | jump ';'                                         
   | returnfct ';'                                    
   | 'for' '(' expr ';' expr ';' expr ')' statement  
   | 'if' paren_expr statement                       
   | 'if' paren_expr statement 'else' statement      
   | 'while' paren_expr statement                    
   | 'do' statement 'while' paren_expr ';'           
   | '{' statement* '}'                               
   | expr ';'                                         
   | 'asm' Bs3asm ';'                                 
   | 'asm' '{' bs3asmblock '}'                        
   | ';'                                              
   ;

declare
   : ext='extern'? type id_ ('[' integer ']')? {addSymbol($id_.text, $type.aType, $ext!=null, $integer.text)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}>  
     (
      ',' id_ ('[' integer ']')? {addSymbol($id_.text, $type.aType, $ext!=null, $integer.text)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}> 
     )*    
   ;

label
   : id_ ':' {addSymbol($id_.text, BS3tinycType._void(), false)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}>
   ;

jump
   : ('jump' | 'goto') id_
   ;

returnfct
   : 'return'              
   | 'return' expr         
   ;

type
   returns [ BS3tinycType aType ]
   : BYTE                  
   | WORD                 
   | 'signed' BYTE       
   | 'signed' WORD        
   | 'unsigned' BYTE      
   | 'unsigned' WORD      
   | VOID                 
   ;

Bs3asm
   : (('{' [0-9]+ '}')[a-zA-Z0-9._[\]+$ \t",']+ ('{' [0-9]+ '}') )+
   ;

Bs3nl
   : [\r\n]+
   ;

Bs3comment
   : ';' ~[\r\n]*
   ;

bs3asmblock
   : Bs3asm Bs3comment? (Bs3nl bs3asmblock? )?  
   | Bs3comment (Bs3nl bs3asmblock? )?          
   | Bs3nl bs3asmblock                          
   ;

//BEGIN of  expression rules
paren_expr
   returns [ BS3tinycType aType ]
   : '(' expr ')'                            { $aType = $expr.aType; }
   ;

expr
   returns [ BS3tinycType aType ]
   : ase1=assignexpr                         { $aType = $ase1.aType; }
   ( ',' asen=assignexpr                     { $aType = $asen.aType; }      
   )*
   ;

assignexpr
   returns [ BS3tinycType aType ]
   : condexpr                                { $aType = $condexpr.aType; }
   | targetvar '=' expr                      { $aType = $targetvar.aType; }
   ;

condexpr
   returns [ BS3tinycType aType ]
   : logicORexpr                             { $aType = $logicORexpr.aType; }
   ('?' expr ':' condexpr                    { $aType = BS3tinycType.cond3Type($expr.aType, $condexpr.aType); }
   )?
   ;

logicORexpr
   returns [ BS3tinycType aType ]
   : land1=logicANDexpr                      { $aType = $land1.aType; }
   ( '||' logicANDexpr                       { $aType = BS3tinycType.unsignedbyte(); }
   )*
   ;

logicANDexpr
   returns [ BS3tinycType aType ]
   : ioe1=incluORexpr                        { $aType = $ioe1.aType; } 
   ( '&&' ioen=incluORexpr                   { $aType = BS3tinycType.unsignedbyte(); }
   )*
   ;

incluORexpr
   returns [ BS3tinycType aType ]
   : eoe1=excluORexpr                        { $aType = $eoe1.aType; }
   ( '|' eoen=excluORexpr                    { $aType = BS3tinycType.iorType($aType, $eoen.aType); }      
   )*
   ;

excluORexpr
   returns [ BS3tinycType aType ]
   : ae1=andexpr                             { $aType = $ae1.aType; }
   ('^' aen=andexpr                          { $aType = BS3tinycType.eorType($aType, $aen.aType); }
   )*
   ;

andexpr
   returns [ BS3tinycType aType ]
   : ee1=equaexpr                            { $aType = $ee1.aType; }
   ('&' een=equaexpr                         { $aType = BS3tinycType.andType($aType, $een.aType); }
   )*
   ;

equaexpr
   returns [ BS3tinycType aType ]
   : re1=relatexpr                           { $aType = $re1.aType; }
   ( ope=('==' | '!=' ) relatexpr            { $aType = BS3tinycType.unsignedbyte(); }
   ) *
   ;   

relatexpr
   returns [ BS3tinycType aType ]
   : se1=shiftexpr                           { $aType = $se1.aType; }
   ( ope=('<' | '>' | '<=' | '>=') shiftexpr { $aType = BS3tinycType.unsignedbyte(); }
   )*
   ;

shiftexpr
   returns [ BS3tinycType aType ]
   : ae1=addexpr                             { $aType = $ae1.aType; }
   (ope=('<<' | '>>') aen=addexpr            { $aType = BS3tinycType.shiftType($aType, $aen.aType); }
   )* 
   ;

addexpr
   returns [ BS3tinycType aType ]
   : me1=mulexpr                             { $aType = $me1.aType; }
   (ope=('+' | '-') men=mulexpr              { $aType = BS3tinycType.addType($aType, $men.aType); }
   )*
   ;

mulexpr
   returns [ BS3tinycType aType ]
   : ce1=castexpr                            { $aType = $ce1.aType; }
   (ope=('*' | '/' | '%') cen=castexpr       { $aType = BS3tinycType.mulType($aType, $cen.aType); }
   )*
   ;

castexpr
   returns [ BS3tinycType aType ]
   :  '(' type ')' ce=castexpr               { $aType = $type.aType; }
   | unaryexpr                               { $aType = $unaryexpr.aType; }
   ;

unaryexpr
   returns [ BS3tinycType aType ]
   : ope=('+' | '-' | '~' | '!') ? tr=term   { $aType = $term.aType;
                                                if ($ope != null) {
                                                   if ($ope.text.charAt(0) == '!') {
                                                      $aType = BS3tinycType.unsignedbyte();
                                                   }
                                                   if ($ope.text.charAt(0) == '-') {
                                                      $aType.setSigned();
                                                   }
                                                }
                                             }
   ;

term
   returns [ BS3tinycType aType ]
   : sourcevar                                     { $aType = $sourcevar.aType; }
   | integer                                       { $aType = $integer.aType; }
   | paren_expr                                    { $aType = $paren_expr.aType; }
   ;

targetvar
   returns [ BS3tinycType aType ]
   : id_ ('[' er=expr ']') ?                       { symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { BS3tinycSymbol s = symbols.get($id_.text);
                                                     $aType = s.type;
                                                   }
   | '*' ('(' tr=type ')')? er=expr                { if ($type.text == null) {
                                                       $aType = BS3tinycType.unsignedword();
                                                     } else {
                                                       $aType = $type.aType;
                                                     }
                                                   }
   | regWord                                       { $aType = $regWord.aType;}
   | regByte                                       { $aType = $regByte.aType;}
   ;

sourcevar
   returns [ BS3tinycType aType ]
   : (ope='&' | ope='*' ('(' tr=type ')')? ) ? 
     ir=id_ ( '[' er=expr ']') ?                   {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { if ($ope ==  null) {
                                                      BS3tinycSymbol s = symbols.get($id_.text);
                                                      $aType = s.type;
                                                     } else {
                                                      if ($ope.text.charAt(0) == '&') {
                                                         $aType = BS3tinycType.unsignedword(); 
                                                      } else { // '*'
                                                         if ($type.text == null) {
                                                            $aType = BS3tinycType.unsignedword();
                                                         } else {
                                                            $aType = $type.aType;
                                                         }
                                                      }
                                                     }
                                                   }
   | id_ '(' ')'                                   {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { $aType = BS3tinycType.unsignedword(); }
   | id_ '(' expr  ')'                             {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { $aType = BS3tinycType.unsignedword(); } // one param W0=expr
   | id_ '(' expr ',' expr ')'                     {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { $aType = BS3tinycType.unsignedword(); } // two params W0=1st expr, W1=2nd expr
   | id_ '(' expr ',' expr ',' expr ')'            {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { $aType = BS3tinycType.unsignedword(); } // 3 aprams W0=1st expr, W1=2nd expr, W2=3rd expr
   | id_ '(' expr ',' expr ',' expr ',' expr ')'   {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { $aType = BS3tinycType.unsignedword(); } // 4 params W0=1st expr, W1=2nd expr, W2=3rd expr and W3=4th expr 
   | regWord                                       { $aType = $regWord.aType;}
   | regByte                                       { $aType = $regByte.aType;}
   | stringdata                                    { $aType = $stringdata.aType; }
   ;

regWord
   returns [ BS3tinycType aType ]
   locals [ String strValue ]
   : REGW                                          {$aType = BS3tinycType.unsignedword(); $strValue = $REGW.text; }
   ;

regByte
   returns [ BS3tinycType aType ]
   locals [String strValue ]
   : REGB                                          {$aType = BS3tinycType.unsignedbyte(); $strValue  = $REGB.text; }
   ;

id_
   : IDSTRING
   ;

integer
   returns [ BS3tinycType aType ]
   locals [String strValue ]
   : INT                                           { $aType = ($INT.text.charAt(0) == '+' || $INT.text.charAt(0) == '-')?BS3tinycType.signedword():BS3tinycType.unsignedword();
                                                     if ($INT.text.charAt(0) == '+') {
                                                       $strValue = $INT.text.substring(1);
                                                     } else {
                                                       $strValue = $INT.text;
                                                     }
                                                    }
   ;

stringdata
   returns [ BS3tinycType aType ]
   locals [ String strValue ]
   : '"' stringdataraw '"'                                   { $aType = BS3tinycType.unsignedword(); stringdata.addString($stringdataraw.text); $strValue = $stringdataraw.text; }
   ;

//END of  expression rules

stringdataraw
   : STRDATA*
   ;

STRDATA
   : (~[\r\n"])
   ;

BYTE
   : 'char'
   | 'byte'
   ;

WORD
   : 'int'
   | 'short'
   | 'word'
   ;

VOID
   : 'void'
   | 'label'
   ;

REGW
   : [wW] [0-3]         { setText(getText().toLowerCase()); }
   ;

REGB
   : [bB] [0-7]         { setText(getText().toLowerCase()); }
   ;

IDSTRING
   : [a-z._][a-z0-9_]*  { setText(getText().toLowerCase()); }
   ;

INT
   : [+\-]?[0-9] +
   | '0x' [0-9A-Fa-f]+  { setText("$" + getText().substring(2)); }
   | '\'' ~['\r\n] '\'' { setText(String.valueOf((int)getText().charAt(1))); }
   ;

WS
   : [ \r\n\t] -> skip
   ;

BlockComment
    :   '/*' .*? '*/'
        -> skip
    ;

LineComment
    :   '//' ~[\r\n]*
        -> skip
    ;
