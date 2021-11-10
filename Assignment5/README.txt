CS5379: Parallel Processing - Assignment 5
  by Tyler JOHNSON and Mert SIDE
  on 20211104

IMPLEMENTATION: 
  Further up in the array, more time it takes to calculate, 
    therefore, to achive proper load balancing we seperate the array to
    smaller chunks using the method described below
  Till you reach the middle of the array:
    Get a chunk from the beginning and another chunk from the end of the array 
      for each pid. This is done for load balancing.
      Chunks look like they are mirrored from the midpoint of the array.

  n / 2P      = the amount to handle above and below theb midpoint
  pid * n/ 2P = lowest index to handle

e.g., for a N of 32 and a pid of 2:

  0      8     16     24     32      
  +------+------+------+------+
  | pid0 | pid1 | pid1 | pid0 |   
  +------+------+------+------+

Files: 
  Makefile             : To compile, test, and clean.
  automateTest_v2.sh   : To run the code repeatedly and save the results.
  output.txt           : Results saved from previous run.
  particle_v1.c        : Implementation C file / source code

.
