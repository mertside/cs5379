CS5379: Parallel Processing - Assignment 2 
  by Tyler JOHNSON and Mert SIDE
  on 20210930

Description of the solution:
  Implemented a tree barrier that consists of two phases:
    1. trapping phase, for p = 2^k
    2. release phase, for p = 2^k 

Based on the tree barier idea in: 
  https://homepages.math.uic.edu/~jan/mcs572/barriers.pdf

Solved in [2*log_2(n)]

Files: 
  question.txt         : Dr. Zhuangs Question.
  Makefile             : To compile, test, and clean.
  automateTest_v2.sh   : To run the code repeatedly and save the results.
  output_np8_run10.txt : Results saved for 8 processes ran 10 times.
  barrier_v4.c         : Implementation C file / source code

.
