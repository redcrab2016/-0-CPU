lexer grammar tinycLexer;
@header {
package backslash.bs3.tinyc.generated;   
}

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
   : [0-9] +
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

Bs3ope_directive
   : 'align'|'db'|'dw'|'embed'|'equ'|'include'|'space' | 'dist' | 'org'
   ;

Bs3ope_je  : 'je'  {setText("jz"); } -> type(Bs3ope_branch) ;
Bs3ope_jne : 'jne' {setText("jnz");} -> type(Bs3ope_branch) ;
Bs3ope_jae : 'jae' {setText("jc"); } -> type(Bs3ope_branch) ;
Bs3ope_jb  : 'jb'  {setText("jnc");} -> type(Bs3ope_branch) ;

Bs3ope_branch
   : 'c'|'call'|'c_w0'|'c_w1'|'c_w2'|'c_w3' |'ret'
   |'int_0'|'int_1'|'int_10'|'int_11'|'int_12'|'int_13'|'int_14'|'int_15'|'int_2'|'int_3'|'int_4'|'int_5'|'int_6'|'int_7'|'int_8'|'int_9'|'iret'
   |'j'|'ja'|'jbe'|'jc'|'jg'|'jge'|'jl'|'jle'|'jn'|'jnc'|'jnn'|'jnv'|'jnz'|'jump'|'jv'|'jz'|'j_w0'|'j_w1'|'j_w2'|'j_w3' 
   ;

Bs3ope_UAL
   : 'adc'|'add'|'and'|'bic'|'cmp'|'dec'|'div'|'eor'|'idiv'|'imul'|'inc'|'mul'|'neg'|'not'|'or'|'rol'|'ror'|'sar'|'sbb'|'shl'|'shr'|'sub'|'tst'
   ;

Bs3ope_stack
   : 'drop' | 'dup'
   | 'popa'|'popf'|'poppc'|'pop_b0'|'pop_b1'|'pop_b2'|'pop_b3'|'pop_b4'|'pop_b5'|'pop_b6'|'pop_b7'|'pop_w0'|'pop_w1'|'pop_w2'|'pop_w3'
   |'pusha'|'pushf'|'pushpc'|'push_b0'|'push_b1'|'push_b2'|'push_b3'|'push_b4'|'push_b5'|'push_b6'|'push_b7'|'push_w0'|'push_w1'|'push_w2'|'push_w3'
   ;

Bs3ope_transfer
   : 'ld'|'leaf_w0'|'leaf_w1'|'leaf_w2'|'leaf_w3'|'lean_w0'|'lean_w1'|'lean_w2'|'lean_w3'|'mov'|'sr'|'swp'
   ;

Bs3ope_misc
   : 'clc'|'cli'|'cln'|'clv'|'clz'|'hevt'|'hlt'|'nop'|'reset'|'stc'|'sti'|'stn'|'stv'|'stz'|'wait'
   ;

Bs3ope_inout
   : 'in'| 'out' | 'out2'
   ;

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
   : '"' (~[\r\n"]*)?  '"'
   ;

Bs3nl
   : [\r\n]+
   ;

Bs3comment
   : ';' (~[\r\n]+)? ~[\r\n}] | ';' (~[\r\n}]*)
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
   : (Bs3macrosubst+ ~[\r\n]*)* [\r\n]+
   ;


mode DBLQUOTESTRING;

STRDATA
   : (~[\r\n"])
   ;

DBLQUOTEEND: '"' -> popMode;