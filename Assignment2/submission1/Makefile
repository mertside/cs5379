all: program1

program1: barrier_v4.c
	mpicc -lm $^ -o barrier_v4.out

test:
	mpirun -np 8 ./barrier_v4.out

clean:
	rm -rf *.out
