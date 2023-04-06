lexer grammar tinycLexer;

OPEN_BRACE:               '{';
CLOSE_BRACE:              '}';
OPEN_BRACKET:             '[';
CLOSE_BRACKET:            ']';
OPEN_PARENS:              '(';
CLOSE_PARENS:             ')';
DOT:                      '.';
COMMA:                    ',';
COLON:                    ':';
SEMICOLON:                ';';
PLUS:                     '+';
MINUS:                    '-';
STAR:                     '*';
DIV:                      '/';
PERCENT:                  '%';
AMP:                      '&';
BITWISE_OR:               '|';
CARET:                    '^';
BANG:                     '!';
TILDE:                    '~';
ASSIGNMENT:               '=';
LT:                       '<';
GT:                       '>';
INTERR:                   '?';
OP_AND:                   '&&';
OP_OR:                    '||';
OP_EQ:                    '==';
OP_NE:                    '!=';
OP_LE:                    '<=';
OP_GE:                    '>=';
OP_LEFT_SHIFT:            '<<';
OP_RIGHT_SHIFT:           '>>';
DBLQUOTE:                 '"' -> pushMode(DBLQUOTESTRING) ;

// Kewords
IF:                        'if';
ELSE:                      'else';
DO:                        'do';
WHILE:                     'while';
FOR:                       'for';
EXTERN:                    'extern';
JUMP:                      'jump';
GOTO:                      'goto';
RETURN:                    'return';
UNSIGNED:                  'unsigned';
SIGNED:                    'signed';
ASM:                       'asm' -> pushMode(ASSEMBLER);


SBYTE
   : 'char'
   ;
BYTE   
   : 'byte'
   ;

SWORD
   : 'int'
   | 'short'
   ;

WORD   
   : 'word'
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

mode ASSEMBLER;

ASMBEGIN:   '{';

ASMEND:     '}' -> popMode;

Bs3macro:   'macro' -> pushMode(ASMMACRO); 

Bs3paramsep:   ',' ; 


Bs3reg
   : ([bB][0-7]) | ([wW][0-3])
   ;

Bs3id
   : [a-zA-Z_][a-zA-Z_0-9]*
   ;

Bs3labeldot:   '.';
Bs3labelcolon: ':';

Bs3accessmode
   : '[' ~[[\n\r\]]+ ']'
   ;

Bs3char
   : '\'' ~['\r\n] '\''
   ;

Bs3decimal
   : ('+' | '-')? [0-9]+
   ;

Bs3hexa
   : '$' [0-9A-Fa-f]+
   ;

Bs3string
   : '"' ((~[\r\n"])* Bs3macrosubst?)+  '"'
   ;

Bs3nl
   : [\r\n]+
   ;

Bs3comment
   : ';' ~[\r\n]*
   ;

Bs3WS
   : [ \t]+
   ;

mode ASMMACRO;

Bs3macrosubst
   : '{' [0-9]+ '}'
   ;

Bs3macroend
   : [\t ]+'endm' -> popMode
   ;

Bs3macroline
   : (Bs3macrosubst* ~[\r\n]*)* [\r\n]+
   ;


mode DBLQUOTESTRING;

STRDATA
   : (~[\r\n"])
   ;

DBLQUOTEEND: '"' -> popMode;