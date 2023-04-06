parser grammar tinycParser;

options {tokenVocab=tinycLexer; contextSuperClass=org.antlr.v4.runtime.RuleContextWithAltNum;}

@header {
import java.util.*;
import backslash.bs3.tinyc.*;
}

@parser::members {
public BS3tinycSymbolSet symbols = new BS3tinycSymbolSet();
public BS3tinycStringData stringdata = new BS3tinycStringData();
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
   | OPEN_BRACE statement* CLOSE_BRACE
   | expr ';'
   | 'asm' ASMBEGIN bs3asmblock ASMEND                        
   | ';'                               
   ;

declare
   : ext='extern'? type id_ ('[' integer ']')?  {!symbols.has($id_.text)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}>  
                                                {symbols.add($id_.text, $type.aType, $ext!=null, $integer.text);}
     (
      COMMA id_ ('[' integer ']')?              {!symbols.has($id_.text)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}> 
                                                {symbols.add($id_.text, $type.aType, $ext!=null, $integer.text);}
     )*    
   ;

label
   : id_ COLON                                  {!symbols.has($id_.text)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}>
                                                {symbols.add($id_.text,  BS3tinycType._void(), false);}
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
   : SBYTE                          { $aType = BS3tinycType.signedbyte(); }
   | BYTE                           { $aType = BS3tinycType.unsignedbyte(); }
   | SWORD                          { $aType = BS3tinycType.signedword(); }
   | WORD                           { $aType = BS3tinycType.unsignedword(); }
   | 'signed' (BYTE | SBYTE)        { $aType = BS3tinycType.signedbyte(); }
   | 'signed' (WORD | SWORD)        { $aType = BS3tinycType.signedword(); }
   | 'unsigned' (BYTE | SBYTE)      { $aType = BS3tinycType.unsignedbyte(); }
   | 'unsigned' (WORD | SWORD)      { $aType = BS3tinycType.unsignedword(); }
   | VOID                           { $aType = BS3tinycType._void(); }
   ;

bs3asmblock
   : (Bs3nl | Bs3WS Bs3nl)* bs3asmline ( (Bs3nl | Bs3WS Bs3nl)+ bs3asmline )*  (Bs3nl | Bs3WS)*
   ;

bs3asmline
   : (bs3asmlabel (bs3ope (bs3params)?)?)? bs3comment ? 
   | bs3macrodef Bs3nl?
   ;


bs3macrodef
   : bs3asmlabel bs3macro Bs3macroline+ Bs3macroend
   ;


bs3asmlabel
   : (Bs3labeldot ? Bs3id Bs3labelcolon ?)? Bs3WS
   ;

bs3ope
   : Bs3id
   ;

bs3params
   : Bs3WS bs3param ( Bs3WS* Bs3paramsep Bs3WS* bs3param )*
   ;

bs3param
   : Bs3reg | bs3labelref | bs3value | Bs3accessmode | Bs3string
   ;

bs3labelref
   : Bs3id ( Bs3labeldot Bs3id )?
   | Bs3labeldot Bs3id
   ;

bs3value
   : Bs3decimal
   | Bs3hexa
   | Bs3char
   ;

bs3comment
   : Bs3WS? Bs3comment
   ;

bs3macro
   : Bs3macro
   ;

//BEGIN of  expression rules
paren_expr
   returns [ BS3tinycType aType ]
   : '(' expr ')'                            { $aType = $expr.aType; }
   ;

expr
   returns [ BS3tinycType aType ]
   : ase1=assignexpr                         { $aType = $ase1.aType; }
   ( COMMA asen=assignexpr                   { $aType = $asen.aType; }      
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
   ('?' expr COLON condexpr                  { $aType = BS3tinycType.cond3Type($expr.aType, $condexpr.aType); }
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
   : id_ ('[' er=expr ']') ?                       { symbols.has($id_.text)}?<fail={"Undefined identifier '"+$id_+"'"}>
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
     ir=id_ ( '[' er=expr ']') ?                   {symbols.has($id_.text)}?<fail={"Undefined identifier '"+$id_+"'"}>
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
   | id_ '(' expr COMMA expr ')'                     {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { $aType = BS3tinycType.unsignedword(); } // two params W0=1st expr, W1=2nd expr
   | id_ '(' expr COMMA expr COMMA expr ')'            {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
                                                   { $aType = BS3tinycType.unsignedword(); } // 3 aprams W0=1st expr, W1=2nd expr, W2=3rd expr
   | id_ '(' expr COMMA expr COMMA expr COMMA expr ')'   {symbols.get($id_.text) != null}?<fail={"Undefined identifier '"+$id_+"'"}>
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
   : DBLQUOTE stringdataraw DBLQUOTEEND                                   { $aType = BS3tinycType.unsignedword(); stringdata.addString($stringdataraw.text); $strValue = $stringdataraw.text; }
   ;

//END of  expression rules

stringdataraw
   : STRDATA*
   ;
