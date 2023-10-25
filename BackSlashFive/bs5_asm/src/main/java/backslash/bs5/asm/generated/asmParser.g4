parser grammar asmParser;
options {tokenVocab=asmLexer; contextSuperClass=org.antlr.v4.runtime.RuleContextWithAltNum;}

@header {
import java.util.*;
import backslash.bs5.asm.*;
}

program: statement+ EOF;

statement: 
    label? instruction? comment? endofline ;

endofline:
    Bs5nl;

label:
    Bs5_identifier COLON;

comment: 
    Bs5comment;

instruction
    : directive
    | bs5_basic_instruction
    | bs5_extend_instruction;

directive
    : origin
    | dataword;   

origin
    : Bs5_org number16bits;

dataword
    : Bs5_dw wordlist;

wordlist
    : number16bits (COMMA wordlist)*;

number16bits
    :  Bs5_num_hexa_word
    |  Bs5_num_hexa_byte
    |  Bs5_num_hexa_quad
    |  Bs5_num_hexa_one
    |  Bs5_num_hexa_zero
    |  Bs5_num_decimal_unsigned_word
    |  Bs5_num_decimal_unsigned_byte
    |  Bs5_num_decimal_unsigned_quad
    |  Bs5_num_decimal_unsigned_one
    |  Bs5_num_decimal_unsigned_zero
    |  Bs5_num_decimal_signed_word
    |  Bs5_num_decimal_signed_byte
    |  Bs5_num_decimal_signed_one
    |  Bs5_num_decimal_signed_zero
    |  Bs5_num_char
    |  Bs5_label_ptr Bs5_identifier
    |  Bs5_low Bs5_label_ptr Bs5_identifier
    |  Bs5_high Bs5_label_ptr Bs5_identifier;      

numberZero
    :  Bs5_num_hexa_zero
    |  Bs5_num_decimal_unsigned_zero
    |  Bs5_num_decimal_signed_zero; 

numberOne
    :  Bs5_num_hexa_one
    |  Bs5_num_decimal_unsigned_one
    |  Bs5_num_decimal_signed_one;

numberUnsignedByte
    :  Bs5_num_hexa_byte
    |  Bs5_num_hexa_quad
    |  Bs5_num_hexa_one
    |  Bs5_num_hexa_zero
    |  Bs5_num_decimal_unsigned_byte
    |  Bs5_num_decimal_unsigned_quad
    |  Bs5_num_decimal_unsigned_one
    |  Bs5_num_decimal_unsigned_zero
    |  Bs5_num_char
    |  Bs5_low Bs5_label_ptr Bs5_identifier
    |  Bs5_high Bs5_label_ptr Bs5_identifier;

numberSignedByte
    :  Bs5_num_hexa_byte
    |  Bs5_num_hexa_quad
    |  Bs5_num_hexa_one
    |  Bs5_num_hexa_zero
    |  Bs5_num_decimal_signed_byte
    |  Bs5_num_decimal_unsigned_quad
    |  Bs5_num_decimal_unsigned_one
    |  Bs5_num_decimal_unsigned_zero
    |  Bs5_num_decimal_signed_one
    |  Bs5_num_decimal_signed_zero
    |  Bs5_label_loffset Bs5_identifier;

numberUnsignedQuad
    :  Bs5_num_hexa_quad
    |  Bs5_num_hexa_one
    |  Bs5_num_hexa_zero
    |  Bs5_num_decimal_unsigned_quad
    |  Bs5_num_decimal_unsigned_one
    |  Bs5_num_decimal_unsigned_zero;

bs5_basic_instruction // 31 CPU core instructions
    // ccc f mov Rx, 0
    : bs5_cond? bs5_flag? Bs5_mov bs5_reg COMMA numberZero 
    // ccc f mov low Rx, 0
    | bs5_cond? bs5_flag? Bs5_mov Bs5_low bs5_reg COMMA numberZero 
    // ccc f mov high Rx, 0
    | bs5_cond? bs5_flag? Bs5_mov Bs5_high bs5_reg COMMA numberZero 
    // ccc f mov Rx, Ry
    | bs5_cond? bs5_flag? Bs5_mov bs5_reg COMMA bs5_reg 
    // ccc f mov Rx, [Ry]
    | bs5_cond? bs5_flag? Bs5_mov bs5_reg COMMA OPEN_BRACKET bs5_reg CLOSE_BRACKET
    // ccc f mov [Rx], Ry
    | bs5_cond? bs5_flag? Bs5_mov OPEN_BRACKET bs5_reg CLOSE_BRACKET COMMA bs5_reg 
    // ccc f mov low R0, imm8
    | bs5_cond? bs5_flag? Bs5_mov Bs5_low Bs5_reg0 COMMA numberUnsignedByte
    // ccc f mov high R0, imm8
    | bs5_cond? bs5_flag? Bs5_mov Bs5_high Bs5_reg0 COMMA numberUnsignedByte
    // ccc f mov low R0, low Rx
    | bs5_cond? bs5_flag? Bs5_mov Bs5_low Bs5_reg0 COMMA Bs5_low bs5_reg
    // ccc f mov high R0, low Rx
    | bs5_cond? bs5_flag? Bs5_mov Bs5_high Bs5_reg0 COMMA Bs5_low bs5_reg
    // ccc f mov low R0, high Rx
    | bs5_cond? bs5_flag? Bs5_mov Bs5_low Bs5_reg0 COMMA Bs5_high bs5_reg
    // ccc f mov high R0, high Rx
    | bs5_cond? bs5_flag? Bs5_mov Bs5_high Bs5_reg0 COMMA Bs5_high bs5_reg
    // ccc f mov C, Rx:imm4
    | bs5_cond? bs5_flag? Bs5_mov Bs5_flag_c COMMA bs5_reg COLON numberUnsignedQuad
    // ccc f mov Rx:imm4, C
    | bs5_cond? bs5_flag? Bs5_mov bs5_reg COLON numberUnsignedQuad COMMA Bs5_flag_c
    // ccc f mov Rx:imm4, 0
    | bs5_cond? bs5_flag? Bs5_mov bs5_reg COLON numberUnsignedQuad COMMA numberZero
    // ccc f mov Rx:imm4, 1    
    | bs5_cond? bs5_flag? Bs5_mov bs5_reg COLON numberUnsignedQuad COMMA numberOne

    // ccc f add Rx, Ry
    | bs5_cond? bs5_flag? Bs5_add bs5_reg COMMA bs5_reg
    // ccc f add Rx, [Ry]
    | bs5_cond? bs5_flag? Bs5_add bs5_reg COMMA OPEN_BRACKET bs5_reg CLOSE_BRACKET
    // ccc f add R0, imm4
    | bs5_cond? bs5_flag? Bs5_add Bs5_reg0 COMMA numberUnsignedQuad
    // ccc f add Rx, 1
    | bs5_cond? bs5_flag? Bs5_add bs5_reg COMMA numberOne
    // ccc f add R15, simm8
    | bs5_cond? bs5_flag? Bs5_add Bs5_reg15 COMMA numberSignedByte

    // ccc f sub Rx, Ry
    | bs5_cond? bs5_flag? Bs5_sub bs5_reg COMMA bs5_reg
    // ccc f sub Rx, [Ry]
    | bs5_cond? bs5_flag? Bs5_sub bs5_reg COMMA OPEN_BRACKET bs5_reg CLOSE_BRACKET
    // ccc f sub R0, imm4
    | bs5_cond? bs5_flag? Bs5_sub Bs5_reg0 COMMA numberUnsignedQuad
    // ccc f sub Rx, 1
    | bs5_cond? bs5_flag? Bs5_sub bs5_reg COMMA numberOne

    // ccc f shl R0, imm4
    | bs5_cond? bs5_flag? Bs5_shl Bs5_reg0 COMMA numberUnsignedQuad
    // ccc f shr R0, imm4
    | bs5_cond? bs5_flag? Bs5_shr Bs5_reg0 COMMA numberUnsignedQuad
    // ccc f and R0, Rx
    | bs5_cond? bs5_flag? Bs5_and Bs5_reg0 COMMA bs5_reg
    // ccc f or R0, Rx
    | bs5_cond? bs5_flag? Bs5_or Bs5_reg0 COMMA bs5_reg
    // ccc f not Rx:imm4
    | bs5_cond? bs5_flag? Bs5_not bs5_reg COLON numberUnsignedQuad
    // ccc f not Rx    
    | bs5_cond? bs5_flag? Bs5_not bs5_reg
    ;


bs5_reg
    : Bs5_reg0
    | Bs5_reg_1_14
    | Bs5_reg15;

bs5_cond
    : Bs5_cond_always
    | Bs5_cond_Zset
    | Bs5_cond_Zclr
    | Bs5_cond_Cset
    | Bs5_cond_Cclr
    | Bs5_cond_Xset
    | Bs5_cond_Xclr
    | Bs5_cond_never;

bs5_flag
    : Bs5_flag_changed
    | Bs5_flag_unchanged;

bs5_extend_instruction
    : COLON; // dummy : TODO the real stuff