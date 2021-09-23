#!/bin/bash

echo "Running..."
mpicc barrier_v2.c -o barrier_v2.out

rm output.txt

for i in {1..50}; do
  echo "Run $i..."
  printf "\nRun $i \n" >> output.txt
  mpirun -np 8 barrier_v2.out >> output.txt
  printf "\n=========================================================\n" >> output.txt
done
echo "Finished."
