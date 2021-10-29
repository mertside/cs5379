CS5379: Parallel Processing - Assignment 4
  by Tyler JOHNSON and Mert SIDE
  on 20211028

IMPLEMENTATION: 
  Each pid gets its own row and column index.
    Works on its submatrix which is their respective row and column they belong to.
  Each k loop:
    Calculate a local minimum
    Gather on pid 0
    Send to all other pids
  Return

Files: 
  Makefile             : To compile, test, and clean.
  automateTest_v2.sh   : To run the code repeatedly and save the results.
  output.txt           : Results saved from previous run.
  partition_v1.c       : Implementation C file / source code

.
