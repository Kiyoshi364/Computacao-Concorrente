build: main.c lock
	gcc main.c lock.o -lpthread -Wall -o main

run: build nthreads
	cat nthreads | xargs ./main 

lock: lock.c
	gcc lock.c -lpthread -Wall -c -o lock.o

clean:
	rm *.o main
