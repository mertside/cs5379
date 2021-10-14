CS5379: Parallel Processing - Assignment 2 
  by Tyler JOHNSON and Mert SIDE
  on 20211014

IMPLEMENTATION: 
  Find the chunk that the process is responsible for.
  FOR every node:
    Find the minimum distance vertex (same as before).
    Send to pid 0 least and leastPos.
    Receive least of all leasts and leastPos from pid 0.
    Update the distances for all nodes.
  Combine calculated distances.
  IF pid 0:
    Create the correct global distance array in pid 0.
    Receive the correct result from other pids at pid 0.
    Send back the full distance array to all other pids.
  ELSE: 
    Send to pid 0 calculated values for its chunk.
    Receive the entire correct array from pid 0.


Files: 
  question.txt         : Dr. Zhuangs Question.
  Makefile             : To compile, test, and clean.
  automateTest_v2.sh   : To run the code repeatedly and save the results.
  output.txt           : Results saved for 8 processes ran 10 times.
  dijkstra_v3.c        : Implementation C file / source code

.
