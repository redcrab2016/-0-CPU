lexer grammar asmLexer;

OPEN_BRACKET:             '[';
CLOSE_BRACKET:            ']';
OPEN_PARENS:              '(';
CLOSE_PARENS:             ')';
COLON:                    ':';
COMMA:                    ',';

Bs5_org:  'org';
Bs5_dw:   'dw';
Bs5_equ:  'equ';

Bs5_mov:  'mov';
Bs5_add:  'add';
Bs5_sub:  'sub';
Bs5_and:  'and';
Bs5_or:   'or';
Bs5_not:  'not';
Bs5_shl:  'shl';
Bs5_shr:  'shr';

Bs5_cond_always: 'al';
Bs5_cond_Cset: 'cs';
Bs5_cond_Cclr: 'cc';
Bs5_cond_Zset: 'zs';
Bs5_cond_Zclr: 'zc';
Bs5_cond_Xset: 'xs';
Bs5_cond_Xclr: 'xc';
Bs5_cond_never: 'ne';

Bs5_flag_unchanged: 'nf';
Bs5_flag_changed: 'fl';

Bs5_label_ptr: 'ptr';
Bs5_label_lptr: 'lptr';
Bs5_label_hptr: 'hptr';
Bs5_label_loffset: 'loffset';

Bs5_reg
    : [rR]('0'|'1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'10'|'11'|'12'|'13'|'14'|'15');

Bs5_num_hexa_word
    : '#0x'[0123456789ABCDEFabcdef]{3,4}; // sign by setting a complementary of two

Bs5_num_hexa_byte
    : '#0x'[0123456789ABCDEFabcdef]{2}; // sign by setting a complementary of two

Bs5_num_hexa_quad
    : '#0x'[0123456789ABCDEFabcdef]{1};

Bs5_num_decimal_signed
    : ('#'('+'|'-')[0-9]{1,5}) | ('+'|'-')[0-9]{1,5}; // TODO : missing check value from -32768 to 32767

Bs5_num_decimal_unsigned
    : ('#'[0-9]{1,5}) | [0-9]{1,5}; // TODO : missing check value below or equal to 65535

Bs5_num_char
    : '#\''[^\r\n]'\'';

Bs5_identifier
    : (('.'[a-zA-Z_0-9]+)|([a-zA-Z_][a-zA-Z_0-9]*))('.'[a-zA-Z_0-9]+)*;    

Bs5nl
   : [\r\n]+
   ;

Bs5comment
   : ';' (~[\r\n]+)? ~[\r\n}] | ';' (~[\r\n}]*)
   ;

Bs5WS
   : [ \t]+
   ;
