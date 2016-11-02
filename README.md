# MedianBitonicMPI

In the beginning of the program there is a definition of a size `N` corresponding to
the data of the table and the number of processes via the command line and also
the maximum buffer size for data transfer -`MAX_SIZE`.

After data `N` are divided into a number of elements (proportional to the processes given
as input) and initialized randomly through the `rand` function with a range from `0` to `N`. 
The initialization of each packet in done in parallel, by each process, for
time saving, but also space in case where the data are too large to
fit on a machine. Now each process selects a partner which through comparisons
exchanges elements until in a process ends up with a collection of the smaller data and the other
process has the larger data. This is constantly repeated for all
processes-couples. 

After repetitions each process has an unsorted
list which is not at all related to the others, since the higher value of
process `i` will be less than or (at most) equal to the smallest value of process `i + 1`.

Each process then calls the serial quicksort and classifies locally (on each machine
if the algorithm is running in a cluster) its own data only. So each process will have a 
part of the registered table. Now median finding is easy as the right median is found in the
first element of the process with rank `{process / 2}` (element `array [0]`) while the left median
is the last element of the process with rank `{(number process) / 2 - 1}` (item
`array [elements -1]`). Their average is the median . As the program is
running locally the median is printed with a simple `printf`(x2) of the 
specific processes. For a cluster it can be easily implemented with two
`MPI_Send commands`. Finally, the program displays the execution time of the algorithm.
