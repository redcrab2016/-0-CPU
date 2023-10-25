parser grammar asmParser;
options {tokenVocab=asmLexer; contextSuperClass=org.antlr.v4.runtime.RuleContextWithAltNum;}

@header {
import java.util.*;
import backslash.bs5.asm.*;
}

program: statement + EOF;

statement: 
    label? instruction? comment? endofline;

endofline:
    Bs5nl | EOF;

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

number16bits
    :  Bs5_num_hexa_word
    |  Bs5_num_hexa_byte
    |  Bs5_num_hexa_quad
    |  Bs5_num_hexa_bit
    |  Bs5_num_decimal_unsigned
    |  Bs5_num_decimal_signed
    |  Bs5_num_char;   