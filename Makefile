CC     = gcc
SRCS   = $(wildcard *.c)
OBJS   = $(patsubst %.c,bin/%.o,$(SRCS))
PROF   =
CFLAGS = ${PROF} -g -Wall -Wextra -Wpedantic
LFLAGS = ${PROF}
OUT    = hfm

test: all
	./$(OUT) test.txt

run: all
	./$(OUT)

dbg: all
	gdb ./$(OUT)

val: all
	valgrind ./$(OUT)

all: $(OUT)

clean:
	rm -rvf bin/*

$(OUT): $(OBJS)
	${CC} $^ -o $@

bin/%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

.PHONEY: clean
