lexer grammar asmLexer;
@header {
package backslash.bs5.asm.generated;
import java.util.*;
import backslash.bs5.asm.*;
}
 
OPEN_BRACKET:             '[';
CLOSE_BRACKET:            ']';
OPEN_PARENS:              '('; 
CLOSE_PARENS:             ')';
COLON:                    ':';
COMMA:                    ','     { setText(", "); };
DBLQUOTE:                 '"' -> pushMode(DBLQUOTESTRING) ; 

Bs5_org:  ('org' | 'ORG')         { setText(getText().toLowerCase() + " ");};  
Bs5_dw:   ('dw' | 'DW')           { setText(getText().toLowerCase() + " ");};
 
Bs5_low:  ('low' | 'LOW')         { setText(getText().toLowerCase() + " ");};
Bs5_high: ('high' | 'HIGH')       { setText(getText().toLowerCase() + " ");};

Bs5_flag_x: [Xx]                  { setText(getText().toUpperCase());};

// basic Mnemonic
Bs5_mov:  ('mov' | 'MOV')         { setText(getText().toLowerCase() + " ");}; 
Bs5_add:  ('add' | 'ADD')         { setText(getText().toLowerCase() + " ");};
Bs5_sub:  ('sub' | 'SUB')         { setText(getText().toLowerCase() + " ");};
Bs5_and:  ('and' | 'AND')         { setText(getText().toLowerCase() + " ");};
Bs5_or:   ('or'  | 'OR')          { setText(getText().toLowerCase() + "  ");};
Bs5_not:  ('not' | 'NOT')         { setText(getText().toLowerCase() + " ");};
Bs5_shl:  ('shl' | 'SHL')         { setText(getText().toLowerCase() + " ");};
Bs5_shr:  ('shr' | 'SHR')         { setText(getText().toLowerCase() + " ");};

// core condition
Bs5_cond_always: ('al' | 'AL')                  { setText("al ");};
Bs5_cond_Cset: ('cs' | 'CS' | 'hs' | 'HS')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Cclr: ('cc' | 'CC' | 'lo' | 'LO')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Zset: ('zs' | 'ZS' | 'eq' | 'EQ')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Zclr: ('zc' | 'ZC' | 'ne' | 'NE')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Xset: ('xs' | 'XS')                    { setText("xs ");};
Bs5_cond_Xclr: ('xc' | 'XC')                    { setText("xc ");};
Bs5_cond_never: ('no' | 'NO')                   { setText("no ");};

// assembler condition
Bs5_cond_Sset: ('ss' | 'SS' | 'mi' | 'MI')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Sclr: ('sc' | 'SC' | 'pl' | 'PL')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Vset: ('vs' | 'VS')                    { setText(getText().toLowerCase() + " ");};
Bs5_cond_Vclr: ('vc' | 'VC')                    { setText(getText().toLowerCase() + " ");};
Bs5_cond_Aset: ('hi' | 'HI' | 'as' | 'AS')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Aclr: ('ls' | 'LS' | 'ac' | 'AC')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Lset: ('lt' | 'LT' | 'll' | 'LL')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Lclr: ('ge' | 'GE' | 'lc' | 'LC')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Gset: ('gt' | 'GT' | 'gs' | 'GS')      { setText(getText().toLowerCase() + " ");};
Bs5_cond_Gclr: ('le' | 'LE' | 'gc' | 'GC')      { setText(getText().toLowerCase() + " ");};

Bs5_flag_unchanged: ('nf' | 'NF') { setText(getText().toLowerCase() + " ");};
Bs5_flag_changed: ('fl' | 'FL')   { setText(getText().toLowerCase() + " ");};

Bs5_label_ptr: ('ptr' | 'PTR' | '@') { setText("@");};
Bs5_label_loffset: 'loffset' | 'LOFFSET' { setText(getText().toLowerCase() + " ");};
// alias for register R15
Bs5_regPC: ('PC' | 'pc') { setText("R15"); } -> type(Bs5_reg15);
// aliases for register R14
Bs5_regFG: ('FLAG' | 'flag' | 'FG' | 'fg') { setText("R14"); } -> type(Bs5_reg_1_14);
// alias for register R13 (alias for stack related microprogram)
Bs5_regSP: ('SP' | 'sp') { setText("R13"); } -> type(Bs5_reg_1_14);
// aliases for register R12 (aliases for stack related microprogram)
Bs5_regLP: ('BP' | 'bp' | 'LP' | 'lp') { setText("R12"); } -> type(Bs5_reg_1_14);

Bs5_stack: ('STACK' | 'stack')     { setText(getText().toLowerCase());};
Bs5_local: ('LOCAL' | 'local')     { setText(getText().toLowerCase());};

Bs5_reg0 : [rR] '0' { setText(getText().toUpperCase());};
Bs5_reg15: [rR] '15' { setText(getText().toUpperCase());};

Bs5_reg_1_14
    : [rR]('1'|'2'|'3'|'4'|'5'|'6'|'7'|'8'|'9'|'10'|'11'|'12'|'13'|'14') { setText(getText().toUpperCase());};



// need 16 bits to be encoded
Bs5_num_hexa_word // if signed, then encode as complementary of two
    : '#0x'[1-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f]?
    | '#0x0'[1-9A-Fa-f][0-9A-Fa-f][0-9A-Fa-f];

// need 8 bits to be encoded
Bs5_num_hexa_byte // if signed, then encode as complementary of two
    : '#0x'[0]?[0]?[1-9A-Fa-f][0-9A-Fa-f];

// need 4 bits to be encoded
Bs5_num_hexa_quad
    : '#0x'[0]?[0]?[0]?[2-9A-Fa-f];

// need 1 bit to be encoded
Bs5_num_hexa_one
    : '#0x'[0]?[0]?[0]?[1];


Bs5_num_hexa_zero
    : '#0x'[0]?[0]?[0]?'0';

// need 16 bits to be encoded
Bs5_num_decimal_signed_word
    : '#'?'-25'[6-9]                    // -256  to -259
    | '#'?'-2'[6-9][0-9]                // -260  to -299
    | '#'?'-'[3-9][0-9][0-9]            // -300  to -999
    | '#'?'-'[1-9][0-9][0-9][0-9]       // -1000 to -9999
    | '#'?'-'[1-2][0-9][0-9][0-9][0-9]  // -10000 to -29999
    | '#'?'-3'[0-1][0-9][0-9][0-9]      // -30000 to -31999
    | '#'?'-32'[0-6][0-9][0-9]          // -32000 to -32699
    | '#'?'-327'[0-5][0-9]              // -32700 to -32759
    | '#'?'-3276'[0-8]                  // -32760 to -32768
    | '#'?'+25'[6-9]                    // +256  to +259
    | '#'?'+2'[6-9][0-9]                // +260  to +299
    | '#'?'+'[3-9][0-9][0-9]            // +300  to +999
    | '#'?'+'[1-9][0-9][0-9][0-9]       // +1000 to +9999
    | '#'?'+'[1-2][0-9][0-9][0-9][0-9]  // +10000 to +29999
    | '#'?'+3'[0-1][0-9][0-9][0-9]      // +30000 to +31999
    | '#'?'+32'[0-6][0-9][0-9]          // +32000 to +32699
    | '#'?'+327'[0-5][0-9]              // +32700 to +32759
    | '#'?'+3276'[0-7];                 // +32760 to +32767

// need 8 bits to be encoded 

Bs5_num_decimal_signed_byte 
    : '#'?'-'[1-9]
    | '#'?'-'[1-9][0-9]
    | '#'?'-1'[0-1][0-9]
    | '#'?'-12'[0-8]
    | '#'?'+'[2-9]
    | '#'?'+'[1-9][0-9]
    | '#'?'+1'[0-1][0-9]
    | '#'?'+12'[0-7];

Bs5_num_decimal_signed_one
    : '#'?'+1';

Bs5_num_decimal_signed_zero
    : '#'?'+0'
    | '#'?'-0';

// need 16 bits to be encoded
Bs5_num_decimal_unsigned_word
    : '#'?'25'[6-9]                 // 256   - 259
    | '#'?'2'[6-9][0-9]             // 260   - 299
    | '#'?[3-9][0-9][0-9]           // 300   - 999
    | '#'?[1-9][0-9][0-9][0-9]      // 1000  - 9999
    | '#'?[1-5][0-9][0-9][0-9][0-9] // 10000 - 59999
    | '#'?'6'[0-4][0-9][0-9][0-9]   // 60000 - 64999
    | '#'?'65'[0-4][0-9][0-9]       // 65000 - 65499
    | '#'?'655'[0-2][0-9]           // 65500 - 65529
    | '#'?'6553'[0-5] ;             // 65530 - 65535

// need 8 bits to be encoded
Bs5_num_decimal_unsigned_byte
    : '#'?'1'[6-9]              // 16 - 19
    | '#'?[2-9][0-9]            // 20 - 99
    | '#'?'1'[0-9][0-9]         // 100 - 199
    | '#'?'2'[0-4][0-9]         // 200 - 249
    | '#'?'25'[0-5];            // 250 - 255 

// need 4 bits to be encoded
Bs5_num_decimal_unsigned_quad
    : '#'?[2-9]                 // 2 - 9
    | '#'?'1'[0-5];             // 10 - 15

Bs5_num_decimal_unsigned_one
    : '#'?'1';                  // 1

Bs5_num_decimal_unsigned_zero
    : '#'?'0';                  // 0

Bs5_num_char
    : '#'?'\''~[\u0000-\u001f\u0100-\uffff\u007F]'\'';

Bs5_identifier
    : ('.'[a-zA-Z_0-9]+)
    | ([a-zA-Z_][a-zA-Z_0-9]*('.'[a-zA-Z_0-9]+)?);    

Bs5nl
   : [\r\n]
   ;

// single line comment starts with ';' or '//'
Bs5comment
   : (';'|'//') (~[\r\n])*
   ;

Bs5WS
   : [ \t]+ -> skip;

mode DBLQUOTESTRING;

STRDATA
   : (~[\r\n"])+
   ;

DBLQUOTEEND: '"' -> popMode;