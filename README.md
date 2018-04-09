This is an implementation of a feeder consumer model in which a feeder process is passing an integer array to N consumers.
Each number is placed in a shared memory along with a timestamp which indicates the time that it is placed. Then, every consumer has to get this number an place it in an array of his own. At the end, all consumers must have the exact same array with the feeder, in the same order. Before exiting, all the consumers write their in a file the array they got, along with the average of the time it took for them to take the numbers.

The implementation is done using 3 mutex semaphores, one for the activation/deactivasion of the feeder and 2 for all the consumers.
The idea is that initially all the consumers are locked in semaphore 0, and each one that reads the first element is locked in semaphore 1. Afterwards, each one that reads the second element is locked in semaphore 0 and so on. This strategy is followed in order to avoid conflicts. 

Compilation: make
Execution: "./programm <arg1> <arg2>"

<arg1> = how many the consumers are
<arg2> = size of integer array
