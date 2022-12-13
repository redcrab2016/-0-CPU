#include "bs3.h"

struct bs3_cpu_instr bs3Instr[]={
[BS3_INSTR_NOP] = { .fullName = "NOP", .name = "NOP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_INB] = { .fullName = "INB", .name = "IN", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTB] = { .fullName = "OUTB", .name = "OUT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTB2] = { .fullName = "OUTB2", .name = "OUT2", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = 'B', .operandFormat = "o BX"},
[BS3_INSTR_OUTBI] = { .fullName = "OUTBI", .name = "OUT", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o b"},
[BS3_INSTR_OUTB2I] = { .fullName = "OUTB2I", .name = "OUT2", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o b"},
[0x06] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x07] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x08] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x09] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0A] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0B] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0C] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0D] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0E] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x0F] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_LEAW0] = { .fullName = "LEAW0", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W0,@"},
[BS3_INSTR_LEAW1] = { .fullName = "LEAW1", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W1,@"},
[BS3_INSTR_LEAW2] = { .fullName = "LEAW2", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W2,@"},
[BS3_INSTR_LEAW3] = { .fullName = "LEAW3", .name = "LEA", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o W3,@"},
[BS3_INSTR_LEAB0] = { .fullName = "LEAB0", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W0,@"},
[BS3_INSTR_LEAB1] = { .fullName = "LEAB1", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W1,@"},
[BS3_INSTR_LEAB2] = { .fullName = "LEAB2", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W2,@"},
[BS3_INSTR_LEAB3] = { .fullName = "LEAB3", .name = "LEA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o W3,@"},
[BS3_INSTR_CLC] = { .fullName = "CLC", .name = "CLC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLZ] = { .fullName = "CLZ", .name = "CLZ", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLV] = { .fullName = "CLV", .name = "CLV", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLN] = { .fullName = "CLN", .name = "CLN", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STC] = { .fullName = "STC", .name = "STC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STZ] = { .fullName = "STZ", .name = "STZ", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STV] = { .fullName = "STV", .name = "STV", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_STN] = { .fullName = "STN", .name = "STN", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_INT0] = { .fullName = "INT0", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 0"},
[BS3_INSTR_INT1] = { .fullName = "INT1", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 1"},
[BS3_INSTR_INT2] = { .fullName = "INT2", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 2"},
[BS3_INSTR_INT3] = { .fullName = "INT3", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 3"},
[BS3_INSTR_INT4] = { .fullName = "INT4", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 4"},
[BS3_INSTR_INT5] = { .fullName = "INT5", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 5"},
[BS3_INSTR_INT6] = { .fullName = "INT6", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 6"},
[BS3_INSTR_INT7] = { .fullName = "INT7", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 7"},
[BS3_INSTR_INT8] = { .fullName = "INT8", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 8"},
[BS3_INSTR_INT9] = { .fullName = "INT9", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 9"},
[BS3_INSTR_INT10] = { .fullName = "INT10", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 10"},
[BS3_INSTR_INT11] = { .fullName = "INT11", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 11"},
[BS3_INSTR_INT12] = { .fullName = "INT12", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 12"},
[BS3_INSTR_INT13] = { .fullName = "INT13", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 13"},
[BS3_INSTR_INT14] = { .fullName = "INT14", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 14"},
[BS3_INSTR_INT15] = { .fullName = "INT15", .name = "INT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o 15"},
[BS3_INSTR_J_W0] = { .fullName = "J_W0", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_J_W1] = { .fullName = "J_W1", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_J_W2] = { .fullName = "J_W2", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_J_W3] = { .fullName = "J_W3", .name = "J", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_C_W0] = { .fullName = "C_W0", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_C_W1] = { .fullName = "C_W1", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_C_W2] = { .fullName = "C_W2", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_C_W3] = { .fullName = "C_W3", .name = "C", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[0x38] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x39] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x3A] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0x3B] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_RET] = { .fullName = "RET", .name = "RET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_IRET] = { .fullName = "IRET", .name = "IRET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_J_A] = { .fullName = "J_A", .name = "J", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o cb"},
[BS3_INSTR_C_A] = { .fullName = "C_A", .name = "C", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o cb"},
[BS3_INSTR_JZ] = { .fullName = "JZ", .name = "JZ", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNZ] = { .fullName = "JNZ", .name = "JNZ", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JC] = { .fullName = "JC", .name = "JC", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNC] = { .fullName = "JNC", .name = "JNC", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JN] = { .fullName = "JN", .name = "JN", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNN] = { .fullName = "JNN", .name = "JNN", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JV] = { .fullName = "JV", .name = "JV", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JNV] = { .fullName = "JNV", .name = "JNV", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JA] = { .fullName = "JA", .name = "JA", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JBE] = { .fullName = "JBE", .name = "JBE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JGE] = { .fullName = "JGE", .name = "JGE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JL] = { .fullName = "JL", .name = "JL", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JG] = { .fullName = "JG", .name = "JG", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_JLE] = { .fullName = "JLE", .name = "JLE", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_J_R] = { .fullName = "J_R", .name = "J", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_C_R] = { .fullName = "C_R", .name = "C", .size = 2, .hasParam = 0, .hasImmediate = 1, .immType = 'B', .operandFormat = "o @"},
[BS3_INSTR_LDBm0] = { .fullName = "LDBm0", .name = "LD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[dc]"},
[BS3_INSTR_LDBm1] = { .fullName = "LDBm1", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy]"},
[BS3_INSTR_LDBm2] = { .fullName = "LDBm2", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + c]"},
[BS3_INSTR_LDBm3] = { .fullName = "LDBm3", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[SP + Wy]"},
[BS3_INSTR_LDBm4] = { .fullName = "LDBm4", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + c]"},
[BS3_INSTR_LDBm5] = { .fullName = "LDBm5", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy + Wz]"},
[BS3_INSTR_LDBm6] = { .fullName = "LDBm6", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + Wy + c]"},
[BS3_INSTR_LDBm7] = { .fullName = "LDBm7", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + Wz + c]"},
[BS3_INSTR_POPB0] = { .fullName = "POPB0", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B0"},
[BS3_INSTR_POPB1] = { .fullName = "POPB1", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B1"},
[BS3_INSTR_POPB2] = { .fullName = "POPB2", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B2"},
[BS3_INSTR_POPB3] = { .fullName = "POPB3", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B3"},
[BS3_INSTR_POPB4] = { .fullName = "POPB4", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B4"},
[BS3_INSTR_POPB5] = { .fullName = "POPB5", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B5"},
[BS3_INSTR_POPB6] = { .fullName = "POPB6", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B6"},
[BS3_INSTR_POPB7] = { .fullName = "POPB7", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B7"},
[BS3_INSTR_LDWm0] = { .fullName = "LDWm0", .name = "LD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,[dc]"},
[BS3_INSTR_LDWm1] = { .fullName = "LDWm1", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy]"},
[BS3_INSTR_LDWm2] = { .fullName = "LDWm2", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + c]"},
[BS3_INSTR_LDWm3] = { .fullName = "LDWm3", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[SP + Wy]"},
[BS3_INSTR_LDWm4] = { .fullName = "LDWm4", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + c]"},
[BS3_INSTR_LDWm5] = { .fullName = "LDWm5", .name = "LD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy + Wz]"},
[BS3_INSTR_LDWm6] = { .fullName = "LDWm6", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + Wy + c]"},
[BS3_INSTR_LDWm7] = { .fullName = "LDWm7", .name = "LD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + Wz + c]"},
[BS3_INSTR_POPW0] = { .fullName = "POPW0", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_POPW1] = { .fullName = "POPW1", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_POPW2] = { .fullName = "POPW2", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_POPW3] = { .fullName = "POPW3", .name = "POP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_POPA] = { .fullName = "POPA", .name = "POPA", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_POPF] = { .fullName = "POPF", .name = "POPF", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_POPPC] = { .fullName = "POPPC", .name = "POPPC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_DROP] = { .fullName = "DROP", .name = "DROP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_SRBm0] = { .fullName = "SRBm0", .name = "SR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[dc]"},
[BS3_INSTR_SRBm1] = { .fullName = "SRBm1", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy]"},
[BS3_INSTR_SRBm2] = { .fullName = "SRBm2", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + c]"},
[BS3_INSTR_SRBm3] = { .fullName = "SRBm3", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[SP + Wy]"},
[BS3_INSTR_SRBm4] = { .fullName = "SRBm4", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + c]"},
[BS3_INSTR_SRBm5] = { .fullName = "SRBm5", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,[Wy + Wz]"},
[BS3_INSTR_SRBm6] = { .fullName = "SRBm6", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[SP + Wy + c]"},
[BS3_INSTR_SRBm7] = { .fullName = "SRBm7", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,[Wy + Wz + c]"},
[BS3_INSTR_PUSHB0] = { .fullName = "PUSHB0", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B0"},
[BS3_INSTR_PUSHB1] = { .fullName = "PUSHB1", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B1"},
[BS3_INSTR_PUSHB2] = { .fullName = "PUSHB2", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B2"},
[BS3_INSTR_PUSHB3] = { .fullName = "PUSHB3", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B3"},
[BS3_INSTR_PUSHB4] = { .fullName = "PUSHB4", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B4"},
[BS3_INSTR_PUSHB5] = { .fullName = "PUSHB5", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B5"},
[BS3_INSTR_PUSHB6] = { .fullName = "PUSHB6", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B6"},
[BS3_INSTR_PUSHB7] = { .fullName = "PUSHB7", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o B7"},
[BS3_INSTR_SRWm0] = { .fullName = "SRWm0", .name = "SR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,[dc]"},
[BS3_INSTR_SRWm1] = { .fullName = "SRWm1", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy]"},
[BS3_INSTR_SRWm2] = { .fullName = "SRWm2", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + c]"},
[BS3_INSTR_SRWm3] = { .fullName = "SRWm3", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[SP + Wy]"},
[BS3_INSTR_SRWm4] = { .fullName = "SRWm4", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + c]"},
[BS3_INSTR_SRWm5] = { .fullName = "SRWm5", .name = "SR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,[Wy + Wz]"},
[BS3_INSTR_SRWm6] = { .fullName = "SRWm6", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[SP + Wy + c]"},
[BS3_INSTR_SRWm7] = { .fullName = "SRWm7", .name = "SR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o Wx,[Wy + Wz + c]"},
[BS3_INSTR_PUSHW0] = { .fullName = "PUSHW0", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W0"},
[BS3_INSTR_PUSHW1] = { .fullName = "PUSHW1", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W1"},
[BS3_INSTR_PUSHW2] = { .fullName = "PUSHW2", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W2"},
[BS3_INSTR_PUSHW3] = { .fullName = "PUSHW3", .name = "PUSH", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o W3"},
[BS3_INSTR_PUSHA] = { .fullName = "PUSHA", .name = "PUSHA", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_PUSHF] = { .fullName = "PUSHF", .name = "PUSHF", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_PUSHPC] = { .fullName = "PUSHPC", .name = "PUSHPC", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_DUP] = { .fullName = "DUP", .name = "DUP", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_ANDB] = { .fullName = "ANDB", .name = "AND", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ORB] = { .fullName = "ORB", .name = "OB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_EORB] = { .fullName = "EORB", .name = "EOR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_BICB] = { .fullName = "BICB", .name = "BIC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_TSTB] = { .fullName = "TSTB", .name = "TST", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SHLB] = { .fullName = "SHLB", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SHRB] = { .fullName = "SHRB", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SARB] = { .fullName = "SARB", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_RORB] = { .fullName = "RORB", .name = "ROR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ROLB] = { .fullName = "ROLB", .name = "ROL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_NOTB] = { .fullName = "NOTB", .name = "NOT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ADDB] = { .fullName = "ADDB", .name = "ADD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ADCB] = { .fullName = "ADCB", .name = "ADC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SUBB] = { .fullName = "SUBB", .name = "SUB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SBBB] = { .fullName = "SBBB", .name = "SBB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_CMPB] = { .fullName = "CMPB", .name = "CMP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_ANDW] = { .fullName = "ANDW", .name = "AND", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ORW] = { .fullName = "ORW", .name = "OR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_EORW] = { .fullName = "EORW", .name = "EOR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_BICW] = { .fullName = "BICW", .name = "BIC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_TSTW] = { .fullName = "TSTW", .name = "TST", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SHLW] = { .fullName = "SHLW", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SHRW] = { .fullName = "SHRW", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SARW] = { .fullName = "SARW", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_RORW] = { .fullName = "RORW", .name = "ROR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ROLW] = { .fullName = "ROLW", .name = "ROL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_NOTW] = { .fullName = "NOTW", .name = "NOT", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ADDW] = { .fullName = "ADDW", .name = "ADD", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ADCW] = { .fullName = "ADCW", .name = "ADC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SUBW] = { .fullName = "SUBW", .name = "SUB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_SBBW] = { .fullName = "SBBW", .name = "SBB", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_CMPW] = { .fullName = "CMPW", .name = "CMP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_MULB] = { .fullName = "MULB", .name = "MUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_IMULB] = { .fullName = "IMULB", .name = "IMUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_DIVB] = { .fullName = "DIVB", .name = "DIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_IDIVB] = { .fullName = "IDIVB", .name = "IDIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[0xB4] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xB5] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_MOVB] = { .fullName = "MOVB", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_SWPB] = { .fullName = "SWPB", .name = "SWP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,BY"},
[BS3_INSTR_MULW] = { .fullName = "MULW", .name = "MUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_IMULW] = { .fullName = "IMULW", .name = "IMUL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_DIVW] = { .fullName = "DIVW", .name = "DIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy,Wz"},
[BS3_INSTR_IDIVW] = { .fullName = "IDIVW", .name = "IDIV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy,Wz"},
[BS3_INSTR_MOVW] = { .fullName = "MOVW", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_MOVSW] = { .fullName = "MOVSW", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o SP,Wx"},
[BS3_INSTR_MOVWS] = { .fullName = "MOVWS", .name = "MOV", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,SP"},
[BS3_INSTR_SWPW] = { .fullName = "SWPW", .name = "SWP", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,Wy"},
[BS3_INSTR_ANDBI] = { .fullName = "ANDBI", .name = "AND", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ORBI] = { .fullName = "ORBI", .name = "OR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_EORBI] = { .fullName = "EORBI", .name = "EOR", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_BICBI] = { .fullName = "BICBI", .name = "BIC", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_TSTBI] = { .fullName = "TSTBI", .name = "TST", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SHLB1] = { .fullName = "SHLB1", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,1"},
[BS3_INSTR_SHRB1] = { .fullName = "SHRB1", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,1"},
[BS3_INSTR_SARB1] = { .fullName = "SARB1", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX,1"},
[BS3_INSTR_INCB] = { .fullName = "INCB", .name = "INC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_DECB] = { .fullName = "DECB", .name = "DEC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_NEGB] = { .fullName = "NEGB", .name = "NEG", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o BX"},
[BS3_INSTR_ADDBI] = { .fullName = "ADDBI", .name = "ADD", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ADCBI] = { .fullName = "ADCBI", .name = "ADC", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SUBBI] = { .fullName = "SUBBI", .name = "SUB", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_SBBBI] = { .fullName = "SBBBI", .name = "SBB", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_CMPBI] = { .fullName = "CMPBI", .name = "CMP", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[BS3_INSTR_ANDWI] = { .fullName = "ANDWI", .name = "AND", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_ORWI] = { .fullName = "ORWI", .name = "OR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_EORWI] = { .fullName = "EORWI", .name = "EOR", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_BICWI] = { .fullName = "BICWI", .name = "BIC", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_TSTWI] = { .fullName = "TSTWI", .name = "TST", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SHLW1] = { .fullName = "SHLW1", .name = "SHL", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,1"},
[BS3_INSTR_SHRW1] = { .fullName = "SHRW1", .name = "SHR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,1"},
[BS3_INSTR_SARW1] = { .fullName = "SARW1", .name = "SAR", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx,1"},
[BS3_INSTR_INCW] = { .fullName = "INCW", .name = "INC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_DECW] = { .fullName = "DECW", .name = "DEC", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_NEGW] = { .fullName = "NEGW", .name = "NEG", .size = 2, .hasParam = 1, .hasImmediate = 0, .immType = '_', .operandFormat = "o Wx"},
[BS3_INSTR_ADDWI] = { .fullName = "ADDWI", .name = "ADD", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_ADCWI] = { .fullName = "ADCWI", .name = "ADC", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SUBWI] = { .fullName = "SUBWI", .name = "SUB", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_SBBWI] = { .fullName = "SBBWI", .name = "SBB", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_CMPWI] = { .fullName = "CMPWI", .name = "CMP", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_MULBI] = { .fullName = "MULBI", .name = "MUL", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_IMULBI] = { .fullName = "IMULBI", .name = "IMUL", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_DIVBI] = { .fullName = "DIVBI", .name = "DIV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[BS3_INSTR_IDIVBI] = { .fullName = "IDIVBI", .name = "IDIV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c,BY"},
[0xE4] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xE5] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_MOVBI] = { .fullName = "MOVBI", .name = "MOV", .size = 3, .hasParam = 1, .hasImmediate = 1, .immType = 'B', .operandFormat = "o BX,c"},
[0xE7] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_MULWI] = { .fullName = "MULWI", .name = "MUL", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_IMULWI] = { .fullName = "IMULWI", .name = "IMUL", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_DIVWI] = { .fullName = "DIVWI", .name = "DIV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_IDIVWI] = { .fullName = "IDIVWI", .name = "IDIV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc,Wy"},
[BS3_INSTR_MOVWI] = { .fullName = "MOVWI", .name = "MOV", .size = 4, .hasParam = 1, .hasImmediate = 1, .immType = 'W', .operandFormat = "o Wx,dc"},
[BS3_INSTR_MOVSI] = { .fullName = "MOVSI", .name = "MOV", .size = 3, .hasParam = 0, .hasImmediate = 1, .immType = 'W', .operandFormat = "o SP,cb"},
[0XEE] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xEF] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF0] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF1] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF2] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF3] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF4] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF5] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF6] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF7] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF8] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[0xF9] = { .fullName = "DB", .name = "DB", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o a"},
[BS3_INSTR_STI] = { .fullName = "STI", .name = "STI", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_CLI] = { .fullName = "CLI", .name = "CLI", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_HEVT] = { .fullName = "HEVT", .name = "HEVT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_WAIT] = { .fullName = "WAIT", .name = "WAIT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_RESET] = { .fullName = "RESET", .name = "RESET", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"},
[BS3_INSTR_HLT] = { .fullName = "HLT", .name = "HLT", .size = 1, .hasParam = 0, .hasImmediate = 0, .immType = '_', .operandFormat = "o"}
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
