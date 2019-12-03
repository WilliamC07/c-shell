all: main.o
	gcc -o output main.o

main.o: main.c
	gcc -c main.c

run:
	./output

clean:
	rm *.o