# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean bs3rt bs3asm

CC = gcc      # compiler to use

LINKERFLAG = -lm

SRCS := $(wildcard *.c)
OBJS := $(SRCS:%.c=%.o)
BINS := bs3asm bs3rt

all: ${BINS}

bs3rt: bs3m bs3.o bs3_asm_code_map.o
	@echo "BS3 Runtime link"
	${CC} -s bs3_main.o bs3.o bs3_asm_code_map.o -o $@ ${LINKERFLAG}


bs3asm: ${OBJS}
	@echo "BS3 Assembler/Debugger link"
	${CC}  $^ -o $@ ${LINKERFLAG}

bs3m: 
	echo "BS3 Runtime compile"
	rm -rvf bs3_main.o
	${CC} -D BS3RUNTIME -c bs3_main.c

%.o: %.c
	@echo "Creating object.." $@
	${CC} -Os -c $<

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}