grammar tinyc;


@header {
import java.util.*;
import backslash.bs3.tinyc.*;
}

@parser::members {
Map<String,BS3tinycSymbol> symbols = new HashMap<String,BS3tinycSymbol>();
public boolean addSymbol(String name, BS3tinycType aType, boolean isExtern)
{
   if (symbols.containsKey(name)) return false;
   BS3tinycSymbol s = new  BS3tinycSymbol(name,aType, isExtern);
   symbols.put(name, s);
   return true;
}
}

program
   : statement + EOF 
   ;


statement
   locals [int subctx]
   : declare ';'                                      { $subctx = 0; }
   | label                                            { $subctx = 1; }
   | jump ';'                                         { $subctx = 2; }
   | returnfct ';'                                    { $subctx = 3; }
   | 'for' '(' expr ';' expr ';' expr ')' statement   { $subctx = 4; }
   | 'if' paren_expr statement                        { $subctx = 5; }                  
   | 'if' paren_expr statement 'else' statement       { $subctx = 6; } 
   | 'while' paren_expr statement                     { $subctx = 7; } 
   | 'do' statement 'while' paren_expr ';'            { $subctx = 8; } 
   | '{' statement* '}'                               { $subctx = 9; } 
   | expr ';'                                         { $subctx = 10; } 
   | 'asm' Bs3asm ';'                                 { $subctx = 11; }
   | 'asm' '{' bs3asmblock '}'                        { $subctx = 12; }
   | ';'                                              { $subctx = 13; } 
   ;

declare
   : ext='extern'? type id_ {addSymbol($id_.text, $type.aType, $ext!=null)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}>  
     (
      ',' id_ {addSymbol($id_.text, $type.aType, $ext!=null)}?<fail={"duplicate identifier declaration '"+$id_.text+"'"}> 
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
   : BYTE                  { $aType = BS3tinycType._byte(); }
   | WORD                  { $aType = BS3tinycType.word(); }
   | 'signed' BYTE         { $aType = BS3tinycType.signedbyte(); }
   | 'signed' WORD         { $aType = BS3tinycType.signedword(); }
   | 'unsigned' BYTE       { $aType = BS3tinycType.unsignedbyte(); }
   | 'unsigned' WORD       { $aType = BS3tinycType.unsignedword(); }
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
   : '(' expr ')'
   ;

expr
   : assignexpr ( ',' assignexpr)*
   ;

assignexpr
   : condexpr
   | targetvar '=' expr
   ;

condexpr
   : logicORexpr ('?' expr ':' condexpr)?
   ;

logicORexpr
   : logicANDexpr ( '||' logicANDexpr)*
   ;

logicANDexpr
   : incluORexpr ( '&&' incluORexpr)*
   ;

incluORexpr
   : excluORexpr ( '||' excluORexpr)*
   ;

excluORexpr
   : andexpr ('^' andexpr)*
   ;

andexpr
   : equaexpr ('&' equaexpr)*
   ;

equaexpr
   : relatexpr ( ope=('==' | '!=' ) relatexpr) *
   ;   

relatexpr
   : shiftexpr ( ope=('<' | '>' | '<=' | '>=') shiftexpr )*
   ;

shiftexpr
   : addexpr (ope=('<<' | '>>') addexpr)*
   ;

addexpr
   : mulexpr (ope=('+' | '-') mulexpr)*
   ;

mulexpr
   : castexpr (ope=('*' | '/' | '%') castexpr)
   ;

castexpr
   :  '(' type ')' castexpr
   | unaryexpr
   ;

unaryexpr
   : ope=('+' | '-' | '~' | '!') ? term
   ;

term
   : sourcevar
   | integer
   | paren_expr
   ;

targetvar
   : id_ ('[' expr ']') ?
   | '*' ('(' type ')')? expr
   | regWord
   | regByte
   ;

sourcevar
   : ope=('&' | '*') ? id_ ( '[' expr ']') ?
   | id_ '(' ')'
   | id_ '(' expr  ')' // one param W0=expr
   | id_ '(' expr ',' expr ')' // two params W0=1st expr, W1=2nd expr
   | id_ '(' expr ',' expr ',' expr ')' // 3 aprams W0=1st expr, W1=2nd expr, W2=3rd expr
   | id_ '(' expr ',' expr ',' expr ',' expr ')' // 4 params W0=1st expr, W1=2nd expr, W2=3rd expr and W3=4th expr
   | regWord
   | regByte
   ;

regWord
   : REGW
   ;

regByte
   : REGB
   ;

id_
   : STRING
   ;

integer
   : INT
   ;

//END of  expression rules

BYTE
   : 'char'
   | 'byte'
   ;

WORD
   : 'int'
   | 'short'
   | 'word'
   ;   

REGW
   : [wW] [0-3]
   ;

REGB
   : [bB] [0-7]
   ;

STRING
   : [a-z._][a-z0-9_]*
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
