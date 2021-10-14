all: program1

program1: dijkstra_v3.c
	mpicc -lm $^ -o dijkstra_v3.out

test:
	mpirun -np 8 ./dijkstra_v3.out

clean:
	rm -rf *.out
