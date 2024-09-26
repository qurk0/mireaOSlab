all: run

run: myLs
	./myLs

myLs: main.o
	gcc main.o -o myLs

main.o: main.c
	gcc -c main.c

clean:
	rm -rf *.o myLs