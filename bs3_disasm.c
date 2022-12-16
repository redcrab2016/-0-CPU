#include "bs3.h"

struct bs3_cpu_instr bs3Instr[]={
[BS3_INSTR_NOP] = { .fullName = "NOP", .opeType = BS3_ASM_OPETYPE_CPU, .name = "NOP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_INB] = { .fullName = "INB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IN", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTB] = { .fullName = "OUTB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OUT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTB2] = { .fullName = "OUTB2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OUT2", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTBI] = { .fullName = "OUTBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OUT", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o b"},
[BS3_INSTR_OUTB2I] = { .fullName = "OUTB2I", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OUT2", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o b"},
[0x06] = { .fullName = "DB_06", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_06", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x07] = { .fullName = "DB_07", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_07", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x08] = { .fullName = "DB_08", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_08", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x09] = { .fullName = "DB_09", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_09", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x0A] = { .fullName = "DB_0A", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_0A", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x0B] = { .fullName = "DB_0B", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_0B", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x0C] = { .fullName = "DB_0C", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_0C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x0D] = { .fullName = "DB_0D", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_0D", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x0E] = { .fullName = "DB_0E", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_0E", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x0F] = { .fullName = "DB_0F", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_0F", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_LEAW0] = { .fullName = "LEAW0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W0,@"},
[BS3_INSTR_LEAW1] = { .fullName = "LEAW1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W1,@"},
[BS3_INSTR_LEAW2] = { .fullName = "LEAW2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W2,@"},
[BS3_INSTR_LEAW3] = { .fullName = "LEAW3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W3,@"},
[BS3_INSTR_LEAB0] = { .fullName = "LEAB0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W0,@"},
[BS3_INSTR_LEAB1] = { .fullName = "LEAB1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W1,@"},
[BS3_INSTR_LEAB2] = { .fullName = "LEAB2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W2,@"},
[BS3_INSTR_LEAB3] = { .fullName = "LEAB3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W3,@"},
[BS3_INSTR_CLC] = { .fullName = "CLC", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CLC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLZ] = { .fullName = "CLZ", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CLZ", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLV] = { .fullName = "CLV", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CLV", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLN] = { .fullName = "CLN", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CLN", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STC] = { .fullName = "STC", .opeType = BS3_ASM_OPETYPE_CPU, .name = "STC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STZ] = { .fullName = "STZ", .opeType = BS3_ASM_OPETYPE_CPU, .name = "STZ", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STV] = { .fullName = "STV", .opeType = BS3_ASM_OPETYPE_CPU, .name = "STV", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STN] = { .fullName = "STN", .opeType = BS3_ASM_OPETYPE_CPU, .name = "STN", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_INT0] = { .fullName = "INT0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 0"},
[BS3_INSTR_INT1] = { .fullName = "INT1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 1"},
[BS3_INSTR_INT2] = { .fullName = "INT2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 2"},
[BS3_INSTR_INT3] = { .fullName = "INT3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 3"},
[BS3_INSTR_INT4] = { .fullName = "INT4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 4"},
[BS3_INSTR_INT5] = { .fullName = "INT5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 5"},
[BS3_INSTR_INT6] = { .fullName = "INT6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 6"},
[BS3_INSTR_INT7] = { .fullName = "INT7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 7"},
[BS3_INSTR_INT8] = { .fullName = "INT8", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 8"},
[BS3_INSTR_INT9] = { .fullName = "INT9", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 9"},
[BS3_INSTR_INT10] = { .fullName = "INT10", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 10"},
[BS3_INSTR_INT11] = { .fullName = "INT11", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 11"},
[BS3_INSTR_INT12] = { .fullName = "INT12", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 12"},
[BS3_INSTR_INT13] = { .fullName = "INT13", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 13"},
[BS3_INSTR_INT14] = { .fullName = "INT14", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 14"},
[BS3_INSTR_INT15] = { .fullName = "INT15", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 15"},
[BS3_INSTR_J_W0] = { .fullName = "J_W0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_J_W1] = { .fullName = "J_W1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_J_W2] = { .fullName = "J_W2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_J_W3] = { .fullName = "J_W3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_C_W0] = { .fullName = "C_W0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_C_W1] = { .fullName = "C_W1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_C_W2] = { .fullName = "C_W2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_C_W3] = { .fullName = "C_W3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[0x38] = { .fullName = "DB_38", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_38", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x39] = { .fullName = "DB_39", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_39", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x3A] = { .fullName = "DB_3A", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_3A", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x3B] = { .fullName = "DB_3B", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_3B", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_RET] = { .fullName = "RET", .opeType = BS3_ASM_OPETYPE_CPU, .name = "RET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_IRET] = { .fullName = "IRET", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IRET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_J_A] = { .fullName = "J_A", .opeType = BS3_ASM_OPETYPE_CPU, .name = "J", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o cb"},
[BS3_INSTR_C_A] = { .fullName = "C_A", .opeType = BS3_ASM_OPETYPE_CPU, .name = "C", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o cb"},
[BS3_INSTR_JZ] = { .fullName = "JZ", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JZ", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNZ] = { .fullName = "JNZ", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JNZ", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JC] = { .fullName = "JC", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JC", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNC] = { .fullName = "JNC", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JNC", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JN] = { .fullName = "JN", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JN", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNN] = { .fullName = "JNN", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JNN", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JV] = { .fullName = "JV", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JV", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNV] = { .fullName = "JNV", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JNV", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JA] = { .fullName = "JA", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JBE] = { .fullName = "JBE", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JBE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JGE] = { .fullName = "JGE", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JGE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JL] = { .fullName = "JL", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JL", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JG] = { .fullName = "JG", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JG", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JLE] = { .fullName = "JLE", .opeType = BS3_ASM_OPETYPE_CPU, .name = "JLE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_J_R] = { .fullName = "J_R", .opeType = BS3_ASM_OPETYPE_CPU, .name = "J", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_C_R] = { .fullName = "C_R", .opeType = BS3_ASM_OPETYPE_CPU, .name = "C", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_LDBm0] = { .fullName = "LDBm0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[dc]"},
[BS3_INSTR_LDBm1] = { .fullName = "LDBm1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy]"},
[BS3_INSTR_LDBm2] = { .fullName = "LDBm2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + c]"},
[BS3_INSTR_LDBm3] = { .fullName = "LDBm3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[SP + Wy]"},
[BS3_INSTR_LDBm4] = { .fullName = "LDBm4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + c]"},
[BS3_INSTR_LDBm5] = { .fullName = "LDBm5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy + Wz]"},
[BS3_INSTR_LDBm6] = { .fullName = "LDBm6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + Wy + c]"},
[BS3_INSTR_LDBm7] = { .fullName = "LDBm7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + Wz + c]"},
[BS3_INSTR_POPB0] = { .fullName = "POPB0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B0"},
[BS3_INSTR_POPB1] = { .fullName = "POPB1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B1"},
[BS3_INSTR_POPB2] = { .fullName = "POPB2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B2"},
[BS3_INSTR_POPB3] = { .fullName = "POPB3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B3"},
[BS3_INSTR_POPB4] = { .fullName = "POPB4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B4"},
[BS3_INSTR_POPB5] = { .fullName = "POPB5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B5"},
[BS3_INSTR_POPB6] = { .fullName = "POPB6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B6"},
[BS3_INSTR_POPB7] = { .fullName = "POPB7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B7"},
[BS3_INSTR_LDWm0] = { .fullName = "LDWm0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,[dc]"},
[BS3_INSTR_LDWm1] = { .fullName = "LDWm1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy]"},
[BS3_INSTR_LDWm2] = { .fullName = "LDWm2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + c]"},
[BS3_INSTR_LDWm3] = { .fullName = "LDWm3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[SP + Wy]"},
[BS3_INSTR_LDWm4] = { .fullName = "LDWm4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + c]"},
[BS3_INSTR_LDWm5] = { .fullName = "LDWm5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy + Wz]"},
[BS3_INSTR_LDWm6] = { .fullName = "LDWm6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + Wy + c]"},
[BS3_INSTR_LDWm7] = { .fullName = "LDWm7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + Wz + c]"},
[BS3_INSTR_POPW0] = { .fullName = "POPW0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_POPW1] = { .fullName = "POPW1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_POPW2] = { .fullName = "POPW2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_POPW3] = { .fullName = "POPW3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_POPA] = { .fullName = "POPA", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POPA", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_POPF] = { .fullName = "POPF", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POPF", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_POPPC] = { .fullName = "POPPC", .opeType = BS3_ASM_OPETYPE_CPU, .name = "POPPC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_DROP] = { .fullName = "DROP", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DROP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_SRBm0] = { .fullName = "SRBm0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[dc]"},
[BS3_INSTR_SRBm1] = { .fullName = "SRBm1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy]"},
[BS3_INSTR_SRBm2] = { .fullName = "SRBm2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + c]"},
[BS3_INSTR_SRBm3] = { .fullName = "SRBm3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[SP + Wy]"},
[BS3_INSTR_SRBm4] = { .fullName = "SRBm4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + c]"},
[BS3_INSTR_SRBm5] = { .fullName = "SRBm5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy + Wz]"},
[BS3_INSTR_SRBm6] = { .fullName = "SRBm6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + Wy + c]"},
[BS3_INSTR_SRBm7] = { .fullName = "SRBm7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + Wz + c]"},
[BS3_INSTR_PUSHB0] = { .fullName = "PUSHB0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B0"},
[BS3_INSTR_PUSHB1] = { .fullName = "PUSHB1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B1"},
[BS3_INSTR_PUSHB2] = { .fullName = "PUSHB2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B2"},
[BS3_INSTR_PUSHB3] = { .fullName = "PUSHB3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B3"},
[BS3_INSTR_PUSHB4] = { .fullName = "PUSHB4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B4"},
[BS3_INSTR_PUSHB5] = { .fullName = "PUSHB5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B5"},
[BS3_INSTR_PUSHB6] = { .fullName = "PUSHB6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B6"},
[BS3_INSTR_PUSHB7] = { .fullName = "PUSHB7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B7"},
[BS3_INSTR_SRWm0] = { .fullName = "SRWm0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,[dc]"},
[BS3_INSTR_SRWm1] = { .fullName = "SRWm1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy]"},
[BS3_INSTR_SRWm2] = { .fullName = "SRWm2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + c]"},
[BS3_INSTR_SRWm3] = { .fullName = "SRWm3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[SP + Wy]"},
[BS3_INSTR_SRWm4] = { .fullName = "SRWm4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + c]"},
[BS3_INSTR_SRWm5] = { .fullName = "SRWm5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy + Wz]"},
[BS3_INSTR_SRWm6] = { .fullName = "SRWm6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + Wy + c]"},
[BS3_INSTR_SRWm7] = { .fullName = "SRWm7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + Wz + c]"},
[BS3_INSTR_PUSHW0] = { .fullName = "PUSHW0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_PUSHW1] = { .fullName = "PUSHW1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_PUSHW2] = { .fullName = "PUSHW2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_PUSHW3] = { .fullName = "PUSHW3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_PUSHA] = { .fullName = "PUSHA", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSHA", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_PUSHF] = { .fullName = "PUSHF", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSHF", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_PUSHPC] = { .fullName = "PUSHPC", .opeType = BS3_ASM_OPETYPE_CPU, .name = "PUSHPC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_DUP] = { .fullName = "DUP", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DUP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_ANDB] = { .fullName = "ANDB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "AND", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ORB] = { .fullName = "ORB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_EORB] = { .fullName = "EORB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "EOR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_BICB] = { .fullName = "BICB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "BIC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_TSTB] = { .fullName = "TSTB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "TST", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SHLB] = { .fullName = "SHLB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SHRB] = { .fullName = "SHRB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SARB] = { .fullName = "SARB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_RORB] = { .fullName = "RORB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ROR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ROLB] = { .fullName = "ROLB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ROL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_NOTB] = { .fullName = "NOTB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "NOT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ADDB] = { .fullName = "ADDB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ADCB] = { .fullName = "ADCB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SUBB] = { .fullName = "SUBB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SUB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SBBB] = { .fullName = "SBBB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SBB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_CMPB] = { .fullName = "CMPB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CMP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ANDW] = { .fullName = "ANDW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "AND", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ORW] = { .fullName = "ORW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_EORW] = { .fullName = "EORW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "EOR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_BICW] = { .fullName = "BICW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "BIC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_TSTW] = { .fullName = "TSTW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "TST", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SHLW] = { .fullName = "SHLW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SHRW] = { .fullName = "SHRW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SARW] = { .fullName = "SARW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_RORW] = { .fullName = "RORW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ROR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ROLW] = { .fullName = "ROLW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ROL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_NOTW] = { .fullName = "NOTW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "NOT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ADDW] = { .fullName = "ADDW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ADCW] = { .fullName = "ADCW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SUBW] = { .fullName = "SUBW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SUB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SBBW] = { .fullName = "SBBW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SBB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_CMPW] = { .fullName = "CMPW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CMP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_MULB] = { .fullName = "MULB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_IMULB] = { .fullName = "IMULB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IMUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_DIVB] = { .fullName = "DIVB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_IDIVB] = { .fullName = "IDIVB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IDIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[0xB4] = { .fullName = "DB_B4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_B4", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xB5] = { .fullName = "DB_B5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_B5", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_MOVB] = { .fullName = "MOVB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SWPB] = { .fullName = "SWPB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SWP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_MULW] = { .fullName = "MULW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_IMULW] = { .fullName = "IMULW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IMUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_DIVW] = { .fullName = "DIVW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy,Wz"},
[BS3_INSTR_IDIVW] = { .fullName = "IDIVW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IDIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy,Wz"},
[BS3_INSTR_MOVW] = { .fullName = "MOVW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_MOVSW] = { .fullName = "MOVSW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o SP,Wx"},
[BS3_INSTR_MOVWS] = { .fullName = "MOVWS", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,SP"},
[BS3_INSTR_SWPW] = { .fullName = "SWPW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SWP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ANDBI] = { .fullName = "ANDBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "AND", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ORBI] = { .fullName = "ORBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_EORBI] = { .fullName = "EORBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "EOR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_BICBI] = { .fullName = "BICBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "BIC", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_TSTBI] = { .fullName = "TSTBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "TST", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SHLB1] = { .fullName = "SHLB1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_SHRB1] = { .fullName = "SHRB1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_SARB1] = { .fullName = "SARB1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_INCB] = { .fullName = "INCB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_DECB] = { .fullName = "DECB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DEC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_NEGB] = { .fullName = "NEGB", .opeType = BS3_ASM_OPETYPE_CPU, .name = "NEG", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_ADDBI] = { .fullName = "ADDBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ADCBI] = { .fullName = "ADCBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADC", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SUBBI] = { .fullName = "SUBBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SUB", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SBBBI] = { .fullName = "SBBBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SBB", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_CMPBI] = { .fullName = "CMPBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CMP", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ANDWI] = { .fullName = "ANDWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "AND", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_ORWI] = { .fullName = "ORWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "OR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_EORWI] = { .fullName = "EORWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "EOR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_BICWI] = { .fullName = "BICWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "BIC", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_TSTWI] = { .fullName = "TSTWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "TST", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SHLW1] = { .fullName = "SHLW1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_SHRW1] = { .fullName = "SHRW1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_SARW1] = { .fullName = "SARW1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_INCW] = { .fullName = "INCW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "INC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_DECW] = { .fullName = "DECW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DEC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_NEGW] = { .fullName = "NEGW", .opeType = BS3_ASM_OPETYPE_CPU, .name = "NEG", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_ADDWI] = { .fullName = "ADDWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_ADCWI] = { .fullName = "ADCWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "ADC", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SUBWI] = { .fullName = "SUBWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SUB", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SBBWI] = { .fullName = "SBBWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "SBB", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_CMPWI] = { .fullName = "CMPWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CMP", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_MULBI] = { .fullName = "MULBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MUL", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_IMULBI] = { .fullName = "IMULBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IMUL", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_DIVBI] = { .fullName = "DIVBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DIV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_IDIVBI] = { .fullName = "IDIVBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IDIV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[0xE4] = { .fullName = "DB_E4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_E4", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xE5] = { .fullName = "DB_E5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_E5", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_MOVBI] = { .fullName = "MOVBI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MOV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[0xE7] = { .fullName = "DB_E7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_E7", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_MULWI] = { .fullName = "MULWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MUL", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_IMULWI] = { .fullName = "IMULWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IMUL", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_DIVWI] = { .fullName = "DIVWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DIV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_IDIVWI] = { .fullName = "IDIVWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "IDIV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_MOVWI] = { .fullName = "MOVWI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MOV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_MOVSI] = { .fullName = "MOVSI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "MOV", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o SP,cb"},
[0XEE] = { .fullName = "DB_EE", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_EE", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xEF] = { .fullName = "DB_EF", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_EF", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF0] = { .fullName = "DB_F0", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F0", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF1] = { .fullName = "DB_F1", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F1", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF2] = { .fullName = "DB_F2", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F2", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF3] = { .fullName = "DB_F3", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F3", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF4] = { .fullName = "DB_F4", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F4", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF5] = { .fullName = "DB_F5", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F5", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF6] = { .fullName = "DB_F6", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F6", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF7] = { .fullName = "DB_F7", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F7", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF8] = { .fullName = "DB_F8", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F8", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0xF9] = { .fullName = "DB_F9", .opeType = BS3_ASM_OPETYPE_CPU, .name = "DB_F9", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STI] = { .fullName = "STI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "STI", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLI] = { .fullName = "CLI", .opeType = BS3_ASM_OPETYPE_CPU, .name = "CLI", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_HEVT] = { .fullName = "HEVT", .opeType = BS3_ASM_OPETYPE_CPU, .name = "HEVT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_WAIT] = { .fullName = "WAIT", .opeType = BS3_ASM_OPETYPE_CPU, .name = "WAIT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_RESET] = { .fullName = "RESET", .opeType = BS3_ASM_OPETYPE_CPU, .name = "RESET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_HLT] = { .fullName = "HLT", .opeType = BS3_ASM_OPETYPE_CPU, .name = "HLT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[0x100] = { .fullName = "DB", .opeType = BS3_ASM_OPETYPE_META, .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"},
[0x100] = { .fullName = "DW", .opeType = BS3_ASM_OPETYPE_META, .name = "DW", .size = 2, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"},
[0x101] = { .fullName = "EQU", .opeType = BS3_ASM_OPETYPE_ALIAS, .name = "EQU", .size = 0, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"},
[0x102] = { .fullName = "DIST", .opeType = BS3_ASM_OPETYPE_ALIAS, .name = "DIST", .size = 0, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"},
[0x103] = { .fullName = "ORG", .opeType = BS3_ASM_OPETYPE_DIRECTIVE, .name = "ORG", .size = 0, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"},
[0x104] = { .fullName = "MACRO", .opeType = BS3_ASM_OPETYPE_DIRECTIVE, .name = "MACRO", .size = 0, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"},
[0x105] = { .fullName = "ENDM", .opeType = BS3_ASM_OPETYPE_DIRECTIVE, .name = "ENDM", .size = 0, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"},
[0x106] = { .fullName = "INCLUDE", .opeType = BS3_ASM_OPETYPE_DIRECTIVE, .name = "INCLUDE", .size = 0, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "_"}
};

static const char * hexa_digit="0123456789ABCDEF";
 
/* disassemble instruction thanks to the 4 bytes (a, b, c and d) located at particular PC address */
/* return the location of the next instruction */  
WORD bs3_cpu_disassemble(WORD PC, BYTE a, BYTE b, BYTE c, BYTE d, char * result) {
  struct bs3_cpu_instr * instr;
  instr = &bs3Instr[a];
  int idx = 0; /* index in 'result' char array */
  /* first part : address */
  result[idx++] = hexa_digit[(PC & 0xF000) >> 12];
  result[idx++] = hexa_digit[(PC & 0x0F00) >> 8];
  result[idx++] = hexa_digit[(PC & 0x00F0) >> 4];
  result[idx++] = hexa_digit[PC & 0x000F];
  result[idx++] = ':';
  result[idx++] = ' ';
  /* second part : instruction byte dump */
  if (instr->size >= 1) { /* for operator : a */
    result[idx++] = hexa_digit[(a & 0x00F0) >> 4];
    result[idx++] = hexa_digit[a & 0x000F];
    result[idx++] = ' ';
  }
  if (instr->size >= 2) { /* for operator param/immediate : b */
    result[idx++] = hexa_digit[(b & 0x00F0) >> 4];
    result[idx++] = hexa_digit[b & 0x000F];
    result[idx++] = ' ';
  }
  else
  {
    result[idx++] = ' ';
    result[idx++] = ' ';
    result[idx++] = ' ';
  } 
  if (instr->size >= 3) { /* for immediate : c */
    result[idx++] = hexa_digit[(c & 0x00F0) >> 4];
    result[idx++] = hexa_digit[c & 0x000F];
    result[idx++] = ' ';
  }
  else
  {
    result[idx++] = ' ';
    result[idx++] = ' ';
    result[idx++] = ' ';
  } 
  if (instr->size == 4) { /* for immediate : d */
    result[idx++] = hexa_digit[(d & 0x00F0) >> 4];
    result[idx++] = hexa_digit[d & 0x000F];
    result[idx++] = ' ';
  }
  else
  {
    result[idx++] = ' ';
    result[idx++] = ' ';
    result[idx++] = ' ';
  } 
  result[idx++] = ' '; /* separator between dump and disassembled operation */
  /* third part : instruction disassembled */
  union opParam oparam;
  oparam.param = b;
  int opfmtidx=0;
  int opidx=0;
  WORD dest=0;
  while (instr->operandFormat[opfmtidx]) 
  {
    switch (instr->operandFormat[opfmtidx])
    {
      case 'a':
        result[idx++] = hexa_digit[(a & 0x00F0) >> 4];
        result[idx++] = hexa_digit[a & 0x000F];
        break;
      case 'b':
        result[idx++] = hexa_digit[(b & 0x00F0) >> 4];
        result[idx++] = hexa_digit[b & 0x000F];
        break;
      case 'c':
        result[idx++] = hexa_digit[(c & 0x00F0) >> 4];
        result[idx++] = hexa_digit[c & 0x000F];
        break;
      case 'd':
        result[idx++] = hexa_digit[(d & 0x00F0) >> 4];
        result[idx++] = hexa_digit[d & 0x000F];
        break;
      case 'x':
        result[idx++] = hexa_digit[oparam.x2 & 0x000F];
        break;
      case 'X':
        result[idx++] = hexa_digit[oparam.x3 & 0x000F];
        break;
      case 'y':
        result[idx++] = hexa_digit[oparam.y2 & 0x000F];
        break;
      case 'Y':
        result[idx++] = hexa_digit[oparam.y3 & 0x000F];
        break;
      case 'z':
        result[idx++] = hexa_digit[oparam.z2 & 0x000F];
        break;
      case 'o':
        opidx = 0;
        while (instr->name[opidx])
        {
          result[idx++] = instr->name[opidx++];
        }
        result[idx++] = '\t';
        break;
      case '@':
        
        if (instr->size > 1 && instr->hasImmediate && (instr->immType == 'B' || instr->immType == 'W' ))
        {
          if (instr->hasParam) /* immediate on c or (c and d) */
          {
            if (instr->immType = 'B') /* immediate on  c */
            {
              dest = PC + instr->size + (SBYTE)c;
            }
            else /* immediate on dc  (d high byte , c low byte) */
            {
              dest = PC + instr->size + (SWORD)((WORD)c | (((WORD)d )<<8));
            }
          }
          else /* immediate on b or (b and c) */
          {
            if (instr->immType = 'B') /* immediate on b */
            {
              dest = PC + instr->size + (SBYTE)b;
            }
            else /* immediate on cb (c high byte, b low byte) */
            {
              dest = PC + instr->size + (SWORD)((WORD)b | (((WORD)c )<<8));
            }
          }
          result[idx++] = hexa_digit[(dest & 0xF000) >> 12];
          result[idx++] = hexa_digit[(dest & 0x0F00) >> 8];
          result[idx++] = hexa_digit[(dest & 0x00F0) >> 4];
          result[idx++] = hexa_digit[dest & 0x000F];
        }
        else /* Something wrong in the definition */
        {
          result[idx++] = '?';
          result[idx++] = '?';
          result[idx++] = '?';
          result[idx++] = '?';
        }
        break;
      default:
        result[idx++] = instr->operandFormat[opfmtidx];
    } /* end of switch case the operand format character interpretation */
    opfmtidx++;
  } /* end for each character in operandFormat */

  result[idx] = 0; /* ASCIIZ null terminator. */
  
  return PC + instr->size; 
}
/*
I0|N0|V0|Z0|C0|W3  |W2  |W1  |W0  |
PC  |SP  |[SP]|B7B6|B5B4|B3B2|B1B0|
0000|0400|3F08|03A2|02B5|C0A3|0000|
*/
/*                                       01  4  7  10 13                                                           72   77   82   87   92   97   102    */
static const char * bs3_cpu_state_model="I_|N_|V_|Z_|C_|W3  |W2  |W1  |W0  |\nPC  |SP  |[SP]|B7B6|B5B4|B3B2|B1B0|\n____|____|____|____|____|____|____|\n\0";

void bs3_cpu_state(struct bs3_cpu_data * pbs3, char * result)
{
  int it = 0;
  /* copy template */
  while (bs3_cpu_state_model[it])
  {
    result[it] = bs3_cpu_state_model[it];
    it++;
  }
  result[it]=0;
  /* set the flags */
  result[1]  = hexa_digit[pbs3->r.I]; /* I flag */
  result[4]  = hexa_digit[pbs3->r.N]; /* N flag */
  result[7]  = hexa_digit[pbs3->r.V]; /* V flag */
  result[10] = hexa_digit[pbs3->r.Z]; /* Z flag */
  result[13] = hexa_digit[pbs3->r.C]; /* C flag */
  /* PC register */
  result[72] = hexa_digit[(pbs3->r.PC & 0xF000) >> 12]; 
  result[73] = hexa_digit[(pbs3->r.PC & 0x0F00) >> 8];
  result[74] = hexa_digit[(pbs3->r.PC & 0x00F0) >> 4]; 
  result[75] = hexa_digit[pbs3->r.PC & 0x000F];
  /* SP register */
  result[77] = hexa_digit[(pbs3->r.SP & 0xF000) >> 12]; 
  result[78] = hexa_digit[(pbs3->r.SP & 0x0F00) >> 8];
  result[79] = hexa_digit[(pbs3->r.SP & 0x00F0) >> 4]; 
  result[80] = hexa_digit[pbs3->r.SP & 0x000F];
  /* Word at SP register : [SP] */
  result[82] = hexa_digit[(pbs3->m[pbs3->r.SP+1] & 0xF0) >> 4]; 
  result[83] = hexa_digit[pbs3->m[pbs3->r.SP+1] & 0x0F];
  result[84] = hexa_digit[(pbs3->m[pbs3->r.SP] & 0xF0) >> 4]; 
  result[85] = hexa_digit[pbs3->m[pbs3->r.SP] & 0x0F];
  /* W3 register | B7 and B6 registers */
  result[87] = hexa_digit[(pbs3->r.W[3] & 0xF000) >> 12]; 
  result[88] = hexa_digit[(pbs3->r.W[3] & 0x0F00) >> 8];
  result[89] = hexa_digit[(pbs3->r.W[3] & 0x00F0) >> 4]; 
  result[90] = hexa_digit[pbs3->r.W[3] & 0x000F];
  /* W2 register | B5 and B4 registers */
  result[92] = hexa_digit[(pbs3->r.W[2] & 0xF000) >> 12]; 
  result[93] = hexa_digit[(pbs3->r.W[2] & 0x0F00) >> 8];
  result[94] = hexa_digit[(pbs3->r.W[2] & 0x00F0) >> 4]; 
  result[95] = hexa_digit[pbs3->r.W[2] & 0x000F];
  /* W1 register | B3 and B2 registers */
  result[97] = hexa_digit[(pbs3->r.W[1] & 0xF000) >> 12]; 
  result[98] = hexa_digit[(pbs3->r.W[1] & 0x0F00) >> 8];
  result[99] = hexa_digit[(pbs3->r.W[1] & 0x00F0) >> 4]; 
  result[100] = hexa_digit[pbs3->r.W[1] & 0x000F];
  /* W0 register | B1 and B0 registers */
  result[102] = hexa_digit[(pbs3->r.W[0] & 0xF000) >> 12]; 
  result[103] = hexa_digit[(pbs3->r.W[0] & 0x0F00) >> 8];
  result[104] = hexa_digit[(pbs3->r.W[0] & 0x00F0) >> 4]; 
  result[105] = hexa_digit[pbs3->r.W[0] & 0x000F];
}
