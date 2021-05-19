build: main.c lock
	gcc main.c lock.o -lpthread -Wall -o main

run: build nthreads
	@cat nthreads | xargs ./main 

run_test: build_test nthreads
	@cat nthreads | xargs ./testes/teste_lock 

build_test: testes/teste_lock.c lock
	gcc testes/teste_lock.c lock.o -lpthread -Wall -o testes/teste_lock

lock: lock.c
	gcc lock.c -lpthread -Wall -c -o lock.o

clean:
	@rm *.o main testes/*.o testes/teste_lock nthreads
