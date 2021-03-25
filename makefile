LIBS= -pthread

all: problem1 problem2

problem1: problem1.c
	gcc -o problem1 problem1.c -lpthread

problem2: problem2.c
	gcc -o problem2 problem2.c -lpthread

clean:
	rm -f problem1 problem2