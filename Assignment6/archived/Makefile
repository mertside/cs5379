NVCC=nvcc

all: solution_v5
	$(NVCC) $(^.cu) -o $^.out

v4: solution_v4.cu
	$(NVCC) $^ -o $^.out

v3: solution_v3.cu
	$(NVCC) $^ -o $^.out

test: solution_v5.out
	./$^

clean:
	rm -rf *.out
