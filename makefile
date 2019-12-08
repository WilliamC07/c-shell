ifeq ($(DEBUG), true)
	CC = gcc -g
else
	CC = gcc
endif

all: main.o parse_input.o runner.o
	$(CC) -o output main.o parse_input.o runner.o

main.o: main.c parse_input.h
	$(CC) -c main.c

parse_input.o: parse_input.c
	$(CC) -c parse_input.c

runner.o: runner.c parse_input.h
	$(CC) -c runner.c

run:
	./output

clean:
	rm *.o