CS5379: Parallel Processing - Assignment 1

by Tyler JOHNSON and Mert SIDE
on 20210907 

Result:
Half of the calculation on pid == 1 overlaps with half of the communication.

Procedure:
pid == 1 sends the row sums between 0 and 25 as soon as they are calculated.
pid == 0 receives these row sums between 0 and 25 right after its data 
generation is completed. pid == 0 then continues in the meantime.
pid == 1 sends the row sums between 25 and 50 after they are calculated. 
pid == 0 receives the sums and prints everything.

NOTE: implementation was deemed correct by Dr. Zhuang as of SEP 7th, 2021.
