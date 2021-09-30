#!/bin/bash
echo "Running..."

resFile="output"

make clean
echo "Compiling..."
make

echo "Removing previous txt file..."
rm ${resFile}.txt

for i in {1..10}; do
  echo "Run $i..."
  
  printf "\nRun $i \n" >> ${resFile}.txt
  
  make test >> ${resFile}.txt
  
  printf "\n==============================================\n" >> ${resFile}.txt
done
echo "Finished."
