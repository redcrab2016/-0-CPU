# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean bs3rt bs3asm

CC = gcc      # compiler to use

LINKERFLAG = -lm
#DEBUG = -Os
DEBUG = -g
BINS := bs3asm bs3rt

all: ${BINS}

bs3rt: bs3_mainrt.o bs3.o bs3_asm_code_map.o
	@echo "BS3 Runtime link"
	${CC} bs3_mainrt.o bs3.o bs3_asm_code_map.o -o $@ ${LINKERFLAG}


bs3asm: bs3_asm_code_map.o  bs3_asm_line.o  bs3_asm_message.o  bs3_asm.o  bs3_asm_pass1.o  bs3_asm_pass2.o  bs3_debug.o  bs3_disasm.o  bs3_instr.o  bs3_main.o bs3.o
	@echo "BS3 Assembler/Debugger link"
	${CC}  $^ -o $@ ${LINKERFLAG}

bs3m: 
	echo "BS3 Runtime compile"
	rm -rvf bs3_main.o
	${CC} -D BS3RUNTIME -c bs3_main.c

bs3_mainrt.o : bs3_main.c
	@echo "Creating bs3_mainrt.o"
	${CC} ${DEBUG} -D BS3RUNTIME -c bs3_main.c -o bs3_mainrt.o

%.o: %.c
	@echo "Creating object.." $@
	${CC} ${DEBUG} -c $<

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}