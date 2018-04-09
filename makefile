CC = gcc # Compiler

all:src/program

src/program: src/main.o src/routine.o
	$(CC) -o program main.o routine.o

src/main.o:
	$(CC) -c src/main.c

src/routine.o:
	$(CC) -c src/routine.c

.Phony: clean

clean:
	rm -r program main.o routine.o
