#!/bin/bash
echo "Running..."

srcFile="barrier_v3"
execFile="barrier_v3"
resFile="output"

echo "Compiling..."
mpicc ${srcFile}.c -o ${execFile}.out

echo "Removing previous txt file..."
rm ${resFile}.txt

for i in {1..100}; do
  echo "Run $i..."
  
  printf "\nRun $i \n" >> ${resFile}.txt
  
  mpirun -np 8 ${execFile}.out >> ${resFile}.txt
  
  printf "\n=========================================================\n" >> ${resFile}.txt
done
echo "Finished."
