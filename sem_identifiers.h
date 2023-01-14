#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/stat.h>

/*
    Initializing the semaphores, the requested shared memory buffer of 128 Bytes, the number of buffers which is 100.
    Total Bytes in the Buffers is 12800 By
    Semaphore S: Performs Mutual Exclusion
    Semaphore N: Performs synchronization on Producer and Consumer for the number, N, of filled buffers in shared memory
    Semaphore E: PeRforms synchronization on Producer and Consumer for the number, E, of free buffers in shared memory
    Semaphore G: Ensures that Producer accesses file before Consumer reads File
*/
// First define the bytes for the shared memory buffer (128 Bytes)
// Define also the number of buffers of 100
#define SHARED_MEMORY_BUFFER 128        // 128 Bytes Per Buffer in Shared Memory 
#define NUMBER_OF_BUFFERS 100           // 100 Buffers in Shared Memory
#define O_BUFSIZ SHARED_MEMORY_BUFFER  // Setting up an identifier

// Shared Memory 
typedef struct {
    int count;
    char buffer[SHARED_MEMORY_BUFFER];
}shared_memory_st;

// Finite Circular Buffer
typedef struct {
    int file_size;
    shared_memory_st shared_mem[NUMBER_OF_BUFFERS];
}finite_buffer_st;

// Semaphore Identifiers
#define S_SEMAPHORE_VALUE 1                         
#define N_SEMAPHORE_VALUE 0                         
#define E_SEMAPHORE_VALUE (NUMBER_OF_BUFFERS - 1) // (0,99) Since we start at 0, we end at 100 - 1 = 99 buffers 
#define G_SEMAPHORE_VALUE 0                       // If consumer runs first, then it waits until producer increments it

#define S_KEY_ID 1234   // Semaphore S Key Identifier
#define E_KEY_ID 5678   // Semaphore E Key Identifier
#define N_KEY_ID 9012   // Semaphore N Key Identifier
#define G_KEY_ID 3456   // Ensures that Producer accesses file size before Consumer Reads File
#define SHM_KEY 1101    // Shared memory key 

// Input/Output string file path  
#define OUTPUT_FILE "output_text.txt"
#define INPUT_FILE "text.txt"




