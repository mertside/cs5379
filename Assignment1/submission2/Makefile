all: program1

program1: nBlockingCommV7.c
	mpicc $^ -o nBlockingComm.out

test:
	mpirun -np 2 ./nBlockingComm.out

clean:
	rm -rf *.out
