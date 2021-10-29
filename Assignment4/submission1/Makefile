all: program1

program1: partition_v1.c
	mpicc -lm $^ -o partition_v1.out

test:
	mpirun -np 25 ./partition_v1.out

clean:
	rm -rf *.out
