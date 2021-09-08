all: program1

program1: nBlockingCommV5.c
	mpicc $^ -o nBlockingCommV5.out

test:
	mpirun -np 2 ./nBlockingCommV5.out

clean:
	rm -rf *.out
