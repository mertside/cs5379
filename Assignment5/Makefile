all: program1

program1: particle_v1.c
	mpicc -lm $^ -o particle_v1.out

test:
	mpirun -np 4 ./particle_v1.out

clean:
	rm -rf *.out
