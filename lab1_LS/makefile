all: da

da: main.o
	gcc main.o -o da

main.o: main.c
	gcc -c main.c

clean:
	rm -rf *.o da