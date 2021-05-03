run: build
	./a.out

build: main.c lock
	gcc main.c lock.o -lpthread -Wall

lock: lock.c
	gcc lock.c -lpthread -Wall -c -o lock.o

clean:
	rm *.o *.out
