parser grammar asmParser;
options {tokenVocab=asmLexer; contextSuperClass=org.antlr.v4.runtime.RuleContextWithAltNum;}

@header {
import java.util.*;
import backslash.bs5.asm.*;
}

program: statement + EOF;

statement: EOF;