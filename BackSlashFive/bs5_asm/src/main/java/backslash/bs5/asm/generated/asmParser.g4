parser grammar asmParser;
options {tokenVocab=asmLexer; contextSuperClass=org.antlr.v4.runtime.RuleContextWithAltNum;}
 
@header {
package backslash.bs5.asm.generated;
import java.util.*;
import backslash.bs5.asm.*;
}

@parser::members {
public BS5program prg = new BS5program();

}

program: statement+ EOF;

statement: 
    label? instruction? Bs5comment? Bs5nl                                           { prg.addSourceLine($text).incrementLine(); }
    ;

label:
    Bs5_identifier COLON                                                            { prg.addLabel($Bs5_identifier.text); }
    ;

instruction
    : directive
    | bs5_core_instruction
    | bs5_corex_instruction
    | bs5_imm16_instruction
    | bs5_stack_instruction
    ;

directive
    : origin
    | dataword;   

origin
    : Bs5_org number16bitsNumeral                                                   { prg.org($number16bitsNumeral.immediat);}
    ;

dataword
    : Bs5_dw wordlist                                                               { prg.dw($wordlist.lstWord); }
    ;

wordlist
    returns [ List<String> lstWord ]
    : (number16bits|string) (COMMA wordlist)?                                       { $lstWord = new ArrayList<String>(); 
                                                                                      if ($string.text !=  null)  $lstWord.addAll($string.lstWord);
                                                                                      else $lstWord.add($number16bits.immediat); 
                                                                                      if ($wordlist.text != null) {
                                                                                        try {
                                                                                          $lstWord.addAll($wordlist.lstWord);
                                                                                        } catch (Exception e) {
                                                                                          /* ignore the exception, because this issue should be reported by the parser */ 
                                                                                        }
                                                                                      }
                                                                                    }
    ;

string
    returns [ List<String> lstWord ]
    : DBLQUOTE STRDATA DBLQUOTEEND                                                  { $lstWord = new ArrayList<String>(); for (char i: $STRDATA.text.toCharArray()) $lstWord.add(""+((int)i));}
    ;

// 31 CPU core instructions
bs5_core_instruction 
    // ccc f mov Rx, 0
    : bs5_cond? bs5_flag? 
      Bs5_mov bs5_reg COMMA numberZero                                              { prg.asm_mov_Rx_0($bs5_cond.text, $bs5_flag.text, $bs5_reg.text); } 
    // ccc f mov low Rx, 0
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_low bs5_reg COMMA numberZero                                      { prg.asm_mov_low_Rx_0($bs5_cond.text, $bs5_flag.text, $bs5_reg.text); } 
    // ccc f mov high Rx, 0
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_high bs5_reg COMMA numberZero                                     { prg.asm_mov_high_Rx_0($bs5_cond.text, $bs5_flag.text, $bs5_reg.text); }  
    // ccc f mov Rx, Ry
    | bs5_cond? bs5_flag? 
      Bs5_mov rx=bs5_reg COMMA ry=bs5_reg                                           { prg.asm_mov_Rx_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text); }
    // ccc f mov Rx, [Ry]
    | bs5_cond? bs5_flag? 
      Bs5_mov rx=bs5_reg COMMA OPEN_BRACKET ry=bs5_reg CLOSE_BRACKET                { prg.asm_mov_Rx_atRy($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text); }
    // ccc f mov [Rx], Ry
    | bs5_cond? bs5_flag? 
      Bs5_mov OPEN_BRACKET rx=bs5_reg CLOSE_BRACKET COMMA ry=bs5_reg                { prg.asm_mov_atRx_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text); }
    // ccc f mov low R0, imm8 (imm8 != 0)
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_low Bs5_reg0 COMMA numberUnsignedByteNotZero                      { prg.asm_mov_low_R0_imm8($bs5_cond.text, $bs5_flag.text, $numberUnsignedByteNotZero.immediat); }
    // ccc f mov high R0, imm8 (imm8 != 0)
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_high Bs5_reg0 COMMA numberUnsignedByteNotZero                     { prg.asm_mov_high_R0_imm8($bs5_cond.text, $bs5_flag.text, $numberUnsignedByteNotZero.immediat); }
    // ccc f mov low R0, low Rx
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_low Bs5_reg0 COMMA Bs5_low bs5_reg                                { prg.asm_mov_low_R0_low_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text); }
    // ccc f mov high R0, low Rx
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_high Bs5_reg0 COMMA Bs5_low bs5_reg                               { prg.asm_mov_high_R0_low_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text); }
    // ccc f mov low R0, high Rx
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_low Bs5_reg0 COMMA Bs5_high bs5_reg                               { prg.asm_mov_low_R0_high_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text); }
    // ccc f mov high R0, high Rx
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_high Bs5_reg0 COMMA Bs5_high bs5_reg                              { prg.asm_mov_high_R0_high_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text); }
    // ccc f mov X, Rx:imm4
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_flag_x COMMA bs5_reg COLON numberUnsignedQuad                     { prg.asm_mov_X_Rx_imm4($bs5_cond.text, $bs5_flag.text, $bs5_reg.text, $numberUnsignedQuad.immediat); }
    // ccc f mov Rx:imm4, X
    | bs5_cond? bs5_flag? 
      Bs5_mov bs5_reg COLON numberUnsignedQuad COMMA Bs5_flag_x                     { prg.asm_mov_Rx_imm4_X($bs5_cond.text, $bs5_flag.text, $bs5_reg.text, $numberUnsignedQuad.immediat); }
    // ccc f mov Rx:imm4, 0
    | bs5_cond? bs5_flag? 
      Bs5_mov bs5_reg COLON numberUnsignedQuad COMMA numberZero                     { prg.asm_mov_Rx_imm4_0($bs5_cond.text, $bs5_flag.text, $bs5_reg.text, $numberUnsignedQuad.immediat); }
    // ccc f mov Rx:imm4, 1    
    | bs5_cond? bs5_flag?
      Bs5_mov bs5_reg COLON numberUnsignedQuad COMMA numberOne                      { prg.asm_mov_Rx_imm4_1($bs5_cond.text, $bs5_flag.text, $bs5_reg.text, $numberUnsignedQuad.immediat); }

    // ccc f add Rx, Ry
    | bs5_cond? bs5_flag? 
      Bs5_add rx=bs5_reg COMMA ry=bs5_reg                                           { prg.asm_add_Rx_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text); }
    // ccc f add Rx, [Ry]
    | bs5_cond? bs5_flag? 
      Bs5_add rx=bs5_reg COMMA OPEN_BRACKET ry=bs5_reg CLOSE_BRACKET                { prg.asm_add_Rx_atRy($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text); }
    // ccc f add R0, imm4
    | bs5_cond? bs5_flag? 
      Bs5_add Bs5_reg0 COMMA numberUnsignedQuad                                     { prg.asm_add_R0_imm4($bs5_cond.text, $bs5_flag.text, $numberUnsignedQuad.immediat); }
    // ccc f add Rx, 1  (Rx != R0 preferably)
    | bs5_cond? bs5_flag? 
      Bs5_add bs5_reg_1_15 COMMA numberOne                                          { prg.asm_add_Rx_1($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text);}
    // ccc f add R15, simm8
    | bs5_cond? bs5_flag? 
      Bs5_add Bs5_reg15 COMMA numberSignedByte                                      { prg.asm_add_R15_simm8($bs5_cond.text, $bs5_flag.text, $numberSignedByte.immediat);  }

    // ccc f sub Rx, Ry
    | bs5_cond? bs5_flag? 
      Bs5_sub rx=bs5_reg COMMA ry=bs5_reg                                           { prg.asm_add_Rx_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text); }
    // ccc f sub Rx, [Ry]
    | bs5_cond? bs5_flag? 
      Bs5_sub rx=bs5_reg COMMA OPEN_BRACKET ry=bs5_reg CLOSE_BRACKET                { prg.asm_sub_Rx_atRy($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text); }
    // ccc f sub R0, imm4
    | bs5_cond? bs5_flag? 
      Bs5_sub Bs5_reg0 COMMA numberUnsignedQuad                                     { prg.asm_sub_R0_imm4($bs5_cond.text, $bs5_flag.text, $numberUnsignedQuad.immediat); }
    // ccc f sub Rx, 1  (Rx != R0 preferably)
    | bs5_cond? bs5_flag? 
      Bs5_sub bs5_reg_1_15 COMMA numberOne                                          { prg.asm_sub_Rx_1($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text);}

    // ccc f shl R0, imm4
    | bs5_cond? bs5_flag? 
      Bs5_shl Bs5_reg0 COMMA numberUnsignedQuad                                     { prg.asm_shl_R0_imm4($bs5_cond.text, $bs5_flag.text, $numberUnsignedQuad.immediat);}
    // ccc f shr R0, imm4
    | bs5_cond? bs5_flag? 
      Bs5_shr Bs5_reg0 COMMA numberUnsignedQuad                                     { prg.asm_shr_R0_imm4($bs5_cond.text, $bs5_flag.text, $numberUnsignedQuad.immediat);}
    // ccc f and R0, Rx
    | bs5_cond? bs5_flag? 
      Bs5_and Bs5_reg0 COMMA bs5_reg                                                { prg.asm_and_R0_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text);}
    // ccc f or R0, Rx
    | bs5_cond? bs5_flag? 
      Bs5_or Bs5_reg0 COMMA bs5_reg                                                 { prg.asm_or_R0_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text);}
    // ccc f not Rx:imm4
    | bs5_cond? bs5_flag? 
      Bs5_not bs5_reg COLON numberUnsignedQuad                                      { prg.asm_not_Rx_imm4($bs5_cond.text, $bs5_flag.text, $bs5_reg.text, $numberUnsignedQuad.immediat);}
    // ccc f not Rx    
    | bs5_cond? bs5_flag? 
      Bs5_not bs5_reg                                                               { prg.asm_not_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text);}
    ;

// 12 CPU micro programs for register versatility (R0 is modified and can't be used as operand)
bs5_corex_instruction 
    // ccc f mov low Rx, imm8 
    : bs5_cond? bs5_flag? 
      Bs5_mov Bs5_low bs5_reg_1_15 COMMA numberUnsignedByte                         { prg.asm_mov_low_Rx_imm8($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $numberUnsignedByte.immediat);}
    // ccc f  mov high Rx, imm8
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_high bs5_reg_1_15 COMMA numberUnsignedByte                        { prg.asm_mov_high_Rx_imm8($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $numberUnsignedByte.immediat);}
    // ccc f add Rx, imm4 (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_add bs5_reg_1_15 COMMA numberUnsignedQuadNotOne                           { prg.asm_add_Rx_imm4($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $numberUnsignedQuadNotOne.immediat ); }
    // ccc f sub Rx, imm4 (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_sub bs5_reg_1_15 COMMA numberUnsignedQuadNotOne                           { prg.asm_sub_Rx_imm4($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $numberUnsignedQuadNotOne.immediat ); }
    // ccc f shl Rx, imm4 (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_shl bs5_reg_1_15 COMMA numberUnsignedQuad                                 { prg.asm_shl_Rx_imm4($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $numberUnsignedQuad.immediat ); }
    // ccc f shr Rx, imm4 (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_shr bs5_reg_1_15 COMMA numberUnsignedQuad                                 { prg.asm_shr_Rx_imm4($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $numberUnsignedQuad.immediat ); }
    // ccc f mov low Rx, low Ry (Rx != R0 and Ry != R0)
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_low rx=bs5_reg_1_15 COMMA Bs5_low ry=bs5_reg_1_15                 { prg.asm_mov_low_Rx_low_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text ); }
    // ccc f mov low Rx, high Ry (Rx != R0 and Ry != R0)
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_low rx=bs5_reg_1_15 COMMA Bs5_high ry=bs5_reg_1_15                { prg.asm_mov_low_Rx_high_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text ); }
    // ccc f mov high Rx, low Ry (Rx != R0 and Ry != R0)
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_high rx=bs5_reg_1_15 COMMA Bs5_low ry=bs5_reg_1_15                { prg.asm_mov_high_Rx_low_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text ); }
    // ccc f mov high Rx, high Ry (Rx != R0 and Ry != R0)
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_high rx=bs5_reg_1_15 COMMA Bs5_high ry=bs5_reg_1_15               { prg.asm_mov_high_Rx_high_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text ); }
    // ccc f and Rx, Ry (Rx != R0 and Ry != R0)
    | bs5_cond? bs5_flag? 
      Bs5_and rx=bs5_reg_1_15 COMMA ry=bs5_reg_1_15                                 { prg.asm_and_Rx_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text ); }
    // ccc f or  Rx, Ry (Rx != R0 and Ry != R0)
    | bs5_cond? bs5_flag? 
      Bs5_or rx=bs5_reg_1_15 COMMA ry=bs5_reg_1_15                                  { prg.asm_or_Rx_Ry($bs5_cond.text, $bs5_flag.text, $rx.text, $ry.text ); }
    ;

// 12 CPU micro programs for immediate 16 bits value 
// ( R0 is modified and can't be used as operand, except for "mov R0, imm16" and "mov Rx, [imm16]" )
bs5_imm16_instruction
    // ccc f mov Rx, imm16 (Rx != R0 and imm16 != 0)
    : bs5_cond? bs5_flag? 
      Bs5_mov bs5_reg_1_15 COMMA number16bitsNotZero                                { prg.asm_mov_Rx_imm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bitsNotZero.immediat ); }
    // ccc f mov R0, imm16 ( imm16 != 0 )
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_reg0 COMMA number16bitsNotZero                                    { prg.asm_mov_R0_imm16($bs5_cond.text, $bs5_flag.text, $number16bitsNotZero.immediat ); }
    // ccc f add Rx, imm16 (Rx != R0 and imm16 > 15)
    | bs5_cond? bs5_flag? 
      Bs5_add bs5_reg_1_15 COMMA number16bitsNotQuad                                { prg.asm_add_Rx_imm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bitsNotQuad.immediat ); }
    // ccc f sub Rx, imm16 (Rx != R0 and imm16 > 15)
    | bs5_cond? bs5_flag? 
      Bs5_sub bs5_reg_1_15 COMMA number16bitsNotQuad                                { prg.asm_sub_Rx_imm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bitsNotQuad.immediat ); }
    // ccc f mov Rx, [imm16] 
    | bs5_cond? bs5_flag? 
      Bs5_mov bs5_reg COMMA OPEN_BRACKET number16bits CLOSE_BRACKET                 { prg.asm_mov_Rx_atImm16($bs5_cond.text, $bs5_flag.text, $bs5_reg.text, $number16bits.immediat ); }
    // ccc f mov [imm16], Rx ( Rx != R0 )
    | bs5_cond? bs5_flag? 
      Bs5_mov OPEN_BRACKET number16bits CLOSE_BRACKET COMMA bs5_reg_1_15            { prg.asm_mov_atImm16_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bits.immediat );}
    // ccc f add Rx, [imm16] ( Rx != R0 )
    | bs5_cond? bs5_flag? 
      Bs5_add bs5_reg_1_15 COMMA OPEN_BRACKET number16bits CLOSE_BRACKET            { prg.asm_add_Rx_atImm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bits.immediat ); }      
    // ccc f sub Rx, [imm16] ( Rx != R0 )
    | bs5_cond? bs5_flag? 
      Bs5_sub bs5_reg_1_15 COMMA OPEN_BRACKET number16bits CLOSE_BRACKET            { prg.asm_sub_Rx_atImm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bits.immediat ); }
    // ccc f and Rx, imm16 (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_and bs5_reg_1_15 COMMA number16bits                                       { prg.asm_add_Rx_imm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bits.immediat ); }
    // ccc f or Rx, imm16 (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_or bs5_reg_1_15 COMMA number16bits                                        { prg.asm_or_Rx_imm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bits.immediat ); }
    // ccc f and Rx, [imm16] (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_and bs5_reg_1_15 COMMA OPEN_BRACKET number16bits CLOSE_BRACKET            { prg.asm_and_Rx_atImm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bits.immediat ); }
    // ccc f or Rx, [imm16] (Rx != R0)
    | bs5_cond? bs5_flag? 
      Bs5_or bs5_reg_1_15 COMMA OPEN_BRACKET number16bits CLOSE_BRACKET             { prg.asm_or_Rx_atImm16($bs5_cond.text, $bs5_flag.text, $bs5_reg_1_15.text, $number16bits.immediat ); }
    ;

//63 Microprograms to handle stack (backward memory stacking, stack addr in R13, local context in stack addr in R12)
bs5_stack_instruction
//  Push value into stack (4 microprograms)
    //ccc f mov STACK, Rx
    : bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COMMA bs5_reg
        { prg.asm_mov_stack_Rx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text);}
    //ccc f mov STACK, [Rx]  (R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COMMA OPEN_BRACKET bs5_reg CLOSE_BRACKET
        { prg.asm_mov_stack_atRx($bs5_cond.text, $bs5_flag.text, $bs5_reg.text);}
    //ccc f mov STACK, imm16 (R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COMMA number16bits
        { prg.asm_mov_stack_imm16($bs5_cond.text, $bs5_flag.text, $number16bits.immediat);}
    //ccc f mov STACK, [imm16] ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COMMA OPEN_BRACKET number16bits CLOSE_BRACKET
        { prg.asm_mov_stack_atImm16($bs5_cond.text, $bs5_flag.text, $number16bits.immediat);}
//  Pop value from stack (3 microprograms)
    //ccc f mov Rx, STACK (if R15 then it is like a return from call procedure)
    | bs5_cond? bs5_flag?
      Bs5_mov bs5_reg COMMA Bs5_stack
        { prg.asm_mov_Rx_stack($bs5_cond.text, $bs5_flag.text, $bs5_reg.text);}
    //ccc f mov [Rx], STACK ( Rx != R0, R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET COMMA Bs5_stack
    //ccc f mov [imm16], STACK (R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET number16bits CLOSE_BRACKET COMMA Bs5_stack
//  Call procedure  (5 microprograms)
    //ccc f mov STACK:R15, Rx (Rx != R0 & Rx != R15)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON Bs5_reg15 COMMA Bs5_reg_1_14
    //ccc f mov STACK:R15, [Rx] (Rx != R0 & Rx != R15)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON Bs5_reg15 COMMA OPEN_BRACKET Bs5_reg_1_14 CLOSE_BRACKET
    //ccc f mov STACK:R15, imm16 (R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON Bs5_reg15 COMMA number16bits 
    //ccc f mov STACK:R15, [imm16] (R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON Bs5_reg15 COMMA OPEN_BRACKET number16bits CLOSE_BRACKET
    //ccc f add STACK:R15, simm8  (near address, R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON Bs5_reg15 COMMA numberSignedByte 
//   Write to stack/local context (24 microprograms)
    //ccc f mov STACK:imm4, Rx (Rx != R0, R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON numberUnsignedQuad COMMA bs5_reg_1_15
    //ccc f mov STACK:imm4, imm16 ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON numberUnsignedQuad COMMA number16bits
    //ccc f mov STACK:imm4, [Rx] (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON numberUnsignedQuad COMMA OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET
    //ccc f mov STACK:imm4, [imm16] (R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON numberUnsignedQuad COMMA OPEN_BRACKET number16bits CLOSE_BRACKET
    //ccc f mov STACK:imm16, Rx (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON number16bitsNotQuad COMMA bs5_reg_1_15
    //ccc f mov STACK:imm16a, imm16b ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON numa1=number16bitsNotQuad COMMA numb1=number16bits
    //ccc f mov STACK:imm16, [Rx] (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON number16bitsNotQuad COMMA OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET
    //ccc f mov STACK:imm16a, [imm16b] ( R0 modified) 
    | bs5_cond? bs5_flag? 
      Bs5_mov Bs5_stack COLON numa2=number16bitsNotQuad COMMA OPEN_BRACKET numb2=number16bits CLOSE_BRACKET 
        { prg.asm_mov_stackImm16_atImm16($bs5_cond.text, $bs5_flag.text,$numa2.immediat, $numb2.immediat);}
    //ccc f mov STACK:Rx, Ry  (Rx and Ry != R15 and R0) 
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON rx1=Bs5_reg_1_14 COMMA ry1=Bs5_reg_1_14
    //ccc f mov STACK:Rx, imm16 (Rx != R0 and R15, R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON Bs5_reg_1_14 COMMA number16bits
    //ccc f mov STACK:Rx, [Ry] (Rx,Ry != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON rx2=Bs5_reg_1_14 COMMA OPEN_BRACKET r2y=Bs5_reg_1_14 CLOSE_BRACKET
    //ccc f mov STACK:Rx, [imm16] (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COLON Bs5_reg_1_14 COMMA OPEN_BRACKET number16bits CLOSE_BRACKET
    //ccc f mov LOCAL:imm4, Rx (Rx != R0, R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON numberUnsignedQuad COMMA bs5_reg_1_15
    //ccc f mov LOCAL:imm4, imm16 ( R0 modified)
      | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON numberUnsignedQuad COMMA number16bits
    //ccc f mov LOCAL:imm4, [Rx] (Rx != R0 , R0 modified)
      | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON numberUnsignedQuad COMMA OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET
    //ccc f mov LOCAL:imm4, [imm16] ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON numberUnsignedQuad COMMA OPEN_BRACKET number16bits CLOSE_BRACKET
    //ccc f mov LOCAL:imm16, Rx (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON number16bitsNotQuad COMMA bs5_reg_1_15
    //ccc f mov LOCAL:imm16a, imm16b ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON numa3=number16bitsNotQuad COMMA numb3=number16bits
    //ccc f mov LOCAL:imm16, [Rx] (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON number16bitsNotQuad COMMA OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET
    //ccc f mov LOCAL:imm16a, [imm16b] ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON numa4=number16bitsNotQuad COMMA OPEN_BRACKET numb4=number16bits CLOSE_BRACKET
    //ccc f mov LOCAL:Rx, Ry  (Rx and Ry != R15 and R0)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON rx3=Bs5_reg_1_14 COMMA ry3=Bs5_reg_1_14
    //ccc f mov LOCAL:Rx, imm16 (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON Bs5_reg_1_14 COMMA number16bits
    //ccc f mov LOCAL:Rx, [Ry] (Rx,Ry != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON rx4=Bs5_reg_1_14 COMMA OPEN_BRACKET ry4=Bs5_reg_1_14 CLOSE_BRACKET
    //ccc f mov LOCAL:Rx, [imm16] (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COLON Bs5_reg_1_14 COMMA OPEN_BRACKET number16bits CLOSE_BRACKET
//	Read from stack/local context (18 microprograms)
    //ccc f mov Rx, STACK:imm4 (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov bs5_reg_1_15 COMMA Bs5_stack COLON numberUnsignedQuad
    //ccc f mov [Rx], STACK:imm4 (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET COMMA Bs5_stack COLON numberUnsignedQuad
    //ccc f mov [imm16], STACK:imm4 ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET number16bits CLOSE_BRACKET COMMA Bs5_stack COLON numberUnsignedQuad
    //ccc f mov Rx, STACK:imm16 (Rx != R0 , R0 modified)
     | bs5_cond? bs5_flag?
      Bs5_mov bs5_reg_1_15 COMMA Bs5_stack COLON number16bitsNotQuad
    //ccc f mov [Rx], STACK:imm16 (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET COMMA Bs5_stack COLON number16bitsNotQuad
    //ccc f mov [imm16a], STACK:imm16b ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET numa5=number16bits CLOSE_BRACKET COMMA Bs5_stack COLON numb5=number16bitsNotQuad
    //ccc f mov Rx, STACK:Ry (Rx,Ry != R0 , R0 modified)
     | bs5_cond? bs5_flag?
      Bs5_mov rx5=bs5_reg_1_15 COMMA Bs5_stack COLON ry5=bs5_reg_1_15
    //ccc f mov [Rx], STACK:Ry (Rx,Ry != R0 , R0 modified)
     | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET rx6=bs5_reg_1_15 CLOSE_BRACKET COMMA Bs5_stack COLON ry6=bs5_reg_1_15
    //ccc f mov [imm16], STACK:Rx (R0 usable but modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET number16bits CLOSE_BRACKET COMMA Bs5_stack COLON bs5_reg
    //ccc f mov Rx, LOCAL:imm4 (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov bs5_reg_1_15 COMMA Bs5_local COLON numberUnsignedQuad
    //ccc f mov [Rx], LOCAL:imm4 (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET COMMA Bs5_local COLON numberUnsignedQuad
    //ccc f mov [imm16], LOCAL:imm4 (R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET number16bits CLOSE_BRACKET COMMA Bs5_local COLON numberUnsignedQuad
    //ccc f mov Rx, LOCAL:imm16 (Rx != R0 , R0 modified)
     | bs5_cond? bs5_flag?
      Bs5_mov bs5_reg_1_15 COMMA Bs5_local COLON number16bitsNotQuad
    //ccc f mov [Rx], LOCAL:imm16 (Rx != R0 , R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET bs5_reg_1_15 CLOSE_BRACKET COMMA Bs5_local COLON number16bitsNotQuad
    //ccc f mov [imm16a], LOCAL:imm16b ( R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET numa6=number16bits CLOSE_BRACKET COMMA Bs5_local COLON numb6=number16bitsNotQuad
    //ccc f mov Rx, LOCAL:Ry (Rx,Ry != R0 , R0 modified)
     | bs5_cond? bs5_flag?
      Bs5_mov rx7=bs5_reg_1_15 COMMA Bs5_local COLON ry7=bs5_reg_1_15
    //ccc f mov [Rx], LOCAL:Ry (Rx,Ry != R0 , R0 modified)
     | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET rx8=bs5_reg_1_15 CLOSE_BRACKET COMMA Bs5_local COLON ry8=bs5_reg_1_15
    //ccc f mov [imm16], LOCAL:Rx  (R0 usable but R0 modified)
    | bs5_cond? bs5_flag?
      Bs5_mov OPEN_BRACKET number16bits CLOSE_BRACKET COMMA Bs5_local COLON bs5_reg
//	Local context / Stack context setting (2 microprograms)
    //ccc f mov STACK, LOCAL 
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_local COMMA Bs5_stack
    //ccc f mov LOCAL, STACK 
    | bs5_cond? bs5_flag?
      Bs5_mov Bs5_stack COMMA Bs5_local
//	Standard instructions (add, sub, shl, shr, and, or , not) by stack (7 microprograms)
    //ccc f add STACK
    | bs5_cond? bs5_flag?
      Bs5_add Bs5_stack
    //ccc f sub STACK // stack head = stack head - stack+1
    | bs5_cond? bs5_flag?
      Bs5_sub Bs5_stack
    //ccc f shl STACK, imm4
    | bs5_cond? bs5_flag?
      Bs5_shl Bs5_stack COMMA numberUnsignedQuad
    //ccc f shr STACK, imm4
    | bs5_cond? bs5_flag?
      Bs5_shr Bs5_stack COMMA numberUnsignedQuad
    //ccc f and STACK
    | bs5_cond? bs5_flag?
      Bs5_and Bs5_stack
    //ccc f or STACK
    | bs5_cond? bs5_flag?
      Bs5_or Bs5_stack
    //ccc f not STACK
    | bs5_cond? bs5_flag?
      Bs5_not Bs5_stack
;

// any register reference
bs5_reg
    : Bs5_reg0
    | Bs5_reg_1_14
    | Bs5_reg15;

// any register reference except R0
bs5_reg_1_15
    : Bs5_reg_1_14
    | Bs5_reg15;

bs5_cond
    // Core condition
    : Bs5_cond_always
    | Bs5_cond_Zset
    | Bs5_cond_Zclr
    | Bs5_cond_Cset
    | Bs5_cond_Cclr
    | Bs5_cond_Xset
    | Bs5_cond_Xclr
    | Bs5_cond_never
    // Assembler condition
    | Bs5_cond_Sset
    | Bs5_cond_Sclr
    | Bs5_cond_Vset
    | Bs5_cond_Vclr
    | Bs5_cond_Aset
    | Bs5_cond_Aclr
    | Bs5_cond_Lset
    | Bs5_cond_Lclr
    | Bs5_cond_Gset
    | Bs5_cond_Gclr;

bs5_flag
    : Bs5_flag_changed
    | Bs5_flag_unchanged;

number16bits
    returns [ String immediat ]
    :  Bs5_num_hexa_word                        { $immediat = $text; }
    |  Bs5_num_hexa_byte                        { $immediat = $text; }
    |  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_one                         { $immediat = $text; }
    |  Bs5_num_hexa_zero                        { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_word            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_byte            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_one             { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_zero            { $immediat = $text; }
    |  Bs5_num_decimal_signed_word              { $immediat = $text; }
    |  Bs5_num_decimal_signed_byte              { $immediat = $text; }
    |  Bs5_num_decimal_signed_one               { $immediat = $text; }
    |  Bs5_num_decimal_signed_zero              { $immediat = $text; }
    |  Bs5_num_char                             { $immediat = $text; }
    |  Bs5_label_ptr Bs5_identifier             { $immediat = $Bs5_identifier.text; }
    |  Bs5_low Bs5_label_ptr Bs5_identifier     { $immediat = "L8:" + $Bs5_identifier.text; }
    |  Bs5_high Bs5_label_ptr Bs5_identifier    { $immediat = "H8:" + $Bs5_identifier.text; }
    |  Bs5_label_loffset Bs5_identifier         { $immediat = "O8:" + $Bs5_identifier.text; }
    ;

number16bitsNumeral
    returns [ String immediat ]
    :  Bs5_num_hexa_word                        { $immediat = $text; }
    |  Bs5_num_hexa_byte                        { $immediat = $text; }
    |  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_one                         { $immediat = $text; }
    |  Bs5_num_hexa_zero                        { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_word            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_byte            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_one             { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_zero            { $immediat = $text; }
    |  Bs5_num_decimal_signed_word              { $immediat = $text; }
    |  Bs5_num_decimal_signed_byte              { $immediat = $text; }
    |  Bs5_num_decimal_signed_one               { $immediat = $text; }
    |  Bs5_num_decimal_signed_zero              { $immediat = $text; }
    ;


number16bitsNotQuad
    returns [ String immediat ]
    :  Bs5_num_hexa_word                        { $immediat = $text; }
    |  Bs5_num_hexa_byte                        { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_word            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_byte            { $immediat = $text; }
    |  Bs5_num_decimal_signed_word              { $immediat = $text; }
    |  Bs5_num_decimal_signed_byte              { $immediat = $text; }
    |  Bs5_num_char                             { $immediat = $text; }
    |  Bs5_label_ptr Bs5_identifier             { $immediat = $Bs5_identifier.text; }
    |  Bs5_low Bs5_label_ptr Bs5_identifier     { $immediat = "L8:" + $Bs5_identifier.text; }
    |  Bs5_high Bs5_label_ptr Bs5_identifier    { $immediat = "H8:" + $Bs5_identifier.text; }
    ;

number16bitsNotZero
    returns [ String immediat ]
    :  Bs5_num_hexa_word                        { $immediat = $text; }
    |  Bs5_num_hexa_byte                        { $immediat = $text; }
    |  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_one                         { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_word            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_byte            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_one             { $immediat = $text; }
    |  Bs5_num_decimal_signed_word              { $immediat = $text; }
    |  Bs5_num_decimal_signed_byte              { $immediat = $text; }
    |  Bs5_num_decimal_signed_one               { $immediat = $text; }
    |  Bs5_num_char                             { $immediat = $text; }
    |  Bs5_label_ptr Bs5_identifier             { $immediat = $Bs5_identifier.text; }
    |  Bs5_low Bs5_label_ptr Bs5_identifier     { $immediat = "L8:" + $Bs5_identifier.text; }
    |  Bs5_high Bs5_label_ptr Bs5_identifier    { $immediat = "H8:" + $Bs5_identifier.text; }
    ;

numberZero
    :  Bs5_num_hexa_zero
    |  Bs5_num_decimal_unsigned_zero
    |  Bs5_num_decimal_signed_zero
    ;

numberOne
    :  Bs5_num_hexa_one                         
    |  Bs5_num_decimal_unsigned_one             
    |  Bs5_num_decimal_signed_one               
    ;

numberUnsignedByte
    returns [ String immediat ]
    :  Bs5_num_hexa_byte                        { $immediat = $text; }
    |  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_one                         { $immediat = $text; }
    |  Bs5_num_hexa_zero                        { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_byte            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_one             { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_zero            { $immediat = $text; }
    |  Bs5_num_char                             { $immediat = $text; }
    |  Bs5_low Bs5_label_ptr Bs5_identifier     { $immediat = "L8:" + $Bs5_identifier.text; }
    |  Bs5_high Bs5_label_ptr Bs5_identifier    { $immediat = "H8:" + $Bs5_identifier.text; }
    ;

numberUnsignedByteNotZero
    returns [ String immediat ]
    :  Bs5_num_hexa_byte                        { $immediat = $text; }
    |  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_one                         { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_byte            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_one             { $immediat = $text; }
    |  Bs5_num_char                             { $immediat = $text; }
    |  Bs5_low Bs5_label_ptr Bs5_identifier     { $immediat = "L8:" + $Bs5_identifier.text; }
    |  Bs5_high Bs5_label_ptr Bs5_identifier    { $immediat = "H8:" + $Bs5_identifier.text; }
    ;


numberSignedByte
    returns [ String immediat ]
    :  Bs5_num_hexa_byte                        { $immediat = $text; }
    |  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_one                         { $immediat = $text; }
    |  Bs5_num_hexa_zero                        { $immediat = $text; }
    |  Bs5_num_decimal_signed_byte              { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_one             { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_zero            { $immediat = $text; }
    |  Bs5_num_decimal_signed_one               { $immediat = $text; }
    |  Bs5_num_decimal_signed_zero              { $immediat = $text; }
    |  Bs5_label_loffset Bs5_identifier         { $immediat = "O8:" + $Bs5_identifier.text; }
    ;

numberUnsignedQuad
returns [ String immediat ]
    :  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_one                         { $immediat = $text; }
    |  Bs5_num_hexa_zero                        { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_one             { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_zero            { $immediat = $text; }
    ;

numberUnsignedQuadNotOne
returns [ String immediat ]
    :  Bs5_num_hexa_quad                        { $immediat = $text; }
    |  Bs5_num_hexa_zero                        { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_quad            { $immediat = $text; }
    |  Bs5_num_decimal_unsigned_zero            { $immediat = $text; }
    ;
