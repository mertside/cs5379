CS5379: Parallel Processing - Assignment 2 
  by Tyler JOHNSON and Mert SIDE
  on 20210930

Description of the solution:
  Implemented a tree barrier that consists of two phases:
    1. trapping phase
    2. release phase 

  * * * * * * * *    ----------
  |/  |/  |/  |/
  *   *   *   *
  | /     | /           TRAP
  *       *
  |   /
  *                 
  |                  -----------
  *                 
  |   \  
  *       *
  | \     | \          RELEASE
  *   *   *   *
  |\  |\  |\  |\
  * * * * * * * *    -----------

IMPLEMENTATION: 
  The trapping phase, for p = 2^k (recall the fan in gather):
    for i from k − 1 down to 0 do 
      for j from 2^i to 2^(i+1) do
        node j sends to node j − 2^i ; 
        node j − 2^i receives from node j.

  The release phase, for p = 2^k (recall the fan out scatter):
    for i from 0 to k − 1 do 
      for j from 0 to 2^i − 1 do
        node j sends to j + 2^i ;
        node j + 2^i receives from node j.

Solved in [2*log_2(n)]

Based on the tree barier idea in: 
  https://homepages.math.uic.edu/~jan/mcs572/barriers.pdf

Files: 
  question.txt         : Dr. Zhuangs Question.
  Makefile             : To compile, test, and clean.
  automateTest_v2.sh   : To run the code repeatedly and save the results.
  output_np8_run10.txt : Results saved for 8 processes ran 10 times.
  barrier_v4.c         : Implementation C file / source code

.
