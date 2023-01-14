This is the classic PC problem that was done for the SYSC4001 course.
Most of the code blocks and algorithms were taken from the Beginnning Linux Programming Book from Chapter 14 and Chapter 3.
Chapter 14 and Lab 7 were used to implement the PC problem using semaphores, shared memory and buffers. 
Chapter 3 was used to learn how to work with text files, especially the input and output text files. Operations such as open, write, and read. 

For this assignment the process goes like this:
We read a input text file, then producer writes a section of it onto shared memory using buffers. 
Consumer then reads that section from shared memory, using buffers. And then writes it onto a output text file.

The file "sem_identifiers.h" contains all the variable identifiers used thoughtout "producer.c" and "consumer.c" scripts. 
The file "sem_functions.h" contains all the semaphore functions used thoughtout "producer.c" and "consumer.c" scripts. Derived from Lab 7. 
 
NOTE: that only 1 producer and 1 consumer can run at a time.

Use cd to get to the directory where the files are located. Running run.sh will compile and create the executables.
You will have the producer and consumer executables being generated in the same directory.

You can either run:

CASE A
1 Terminal Window:
	$ ./producer & ./consumer &

CASE B
1 Terminal Window:
	$ ./consumer & ./producer &

CASE C
2 Terminal Windows:Run Terminal 1 first then run Terminal 2 second
	Terminal 1: $./producer
	Terminal 2: $./consumer

CASE D
2 Terminal Windows:Run Terminal 1 first then run Terminal 2 second
	Terminal 1: $./consumer
	Terminal 2: $./producer

In any case, running producer first then consumer will give you the same output as with running consumer first 
then producer second. The order of which you run each c script does not matter.

The output by Terminal 1 from CASE C looks like this:
Producer's PID: 11638
Producer 11638 shared memory successfully established. 
Producer produced 36750 bytes to shared memory, read those bytes from the input file named 'text.txt'.
Done.


The output by Terminal 2 from CASE C looks like this:
Consumer's PID: 11639
Consumer 11639 shared memory successfully established. 
Consumer consumed 36750 bytes from shared memory, wrote those bytes to output file named 'output_text'.
Done.



Thank you,
Georgios