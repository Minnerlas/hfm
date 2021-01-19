CC     = gcc
SRCS   = $(wildcard *.c)
OBJS   = $(patsubst %.c,bin/%.o,$(SRCS))
# PROF   = -pg
CFLAGS = ${PROF} -g -Wall -Wextra -Wpedantic
LFLAGS = ${PROF}
OUT    = hfm
TESTF  = test.txt

test4: all
	./$(OUT) -x izlaz1.hfm

test: all
	./$(OUT) $(TESTF)

test2: all
	./$(OUT) ~/Downloads/Projekat_ORT2_2020_2021.zip.zip

test3: all
	./$(OUT) ~/Downloads/*.mp4

all: $(OUT)

run: all
	./$(OUT)

dbg: all
	gdb ./$(OUT)

val: all
	valgrind ./$(OUT) $(TESTF)

clean:
	rm -rvf bin/*

$(OUT): $(OBJS)
	${CC} ${LFLAGS} $^ -o $@

bin/%.o: %.c
	${CC} ${CFLAGS} -c $< -o $@

.PHONEY: clean
