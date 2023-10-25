lexer grammar asmLexer;

OPEN_BRACKET:             '[';
CLOSE_BRACKET:            ']';
OPEN_PARENS:              '(';
CLOSE_PARENS:             ')';
COLON:                    ':';
COMMA:                    ',';

Bs5_org:  'org';
Bs5_dw:   'dw';
//Bs5_equ:  'equ';

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

Bs5_reg0 : [rR] '0';
Bs5_reg15: [rR] '15';

Bs5_reg_1_14
    : [rR]('1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'10'|'11'|'12'|'13'|'14');

Bs5_num_hexa_word // if signed, then encode as complementary of two
    : '#0x'[1-9A-Fa-f][0-9A-Fa-f]{2,3}
    | '#0x0'[1-9A-Fa-f][0-9A-Fa-f]{2};

Bs5_num_hexa_byte // if signed, then encode as complementary of two
    : '#0x'[0]{0,2}[1-9A-Fa-f][0-9A-Fa-f];

Bs5_num_hexa_quad
    : '#0x'[0]{0,3}[2-9A-Fa-f];

Bs5_num_hexa_bit
    : '#0x'[0]{0,3}[01];

Bs5_num_decimal_signed
    : ('#'('+'|'-')[0-9]{1,5}) | ('+'|'-')[0-9]{1,5}; // TODO : missing check value from -32768 to 32767

Bs5_num_decimal_unsigned
    :'#'?([1-9][0-9]{0,3}|[1-5][0-9]{4}|'6'[0-4][0-9]{3}|'65'[0-4][0-9]{2}|'655'[0-2][0-9]|'6553'[0-5]);


Bs5_num_decimal_bit
    : '#'?'0'{0,4}[01];

Bs5_num_char
    : '#\''[^\r\n]'\'';

Bs5_identifier
    : (('.'[a-zA-Z_0-9]+)|([a-zA-Z_][a-zA-Z_0-9]*))('.'[a-zA-Z_0-9]+)*;    

Bs5nl
   : [\r\n]+
   ;

Bs5comment
   : ';' [^\r\n]*
   ;

Bs5WS
   : [ \t]+ -> skip;
