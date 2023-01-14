// #include <unistd.h>
// #include <stdlib.h>
// #include <stdio.h>
// #include <string.h>
// #include <sys/sem.h>
// #include <sys/types.h>
// #include <sys/stat.h>

#include "sem_identifiers.h"
#include "sem_functions.h"

void main(int argc, char *argv[]) 
{
    pid_t pid = getpid();   
    int total_bytes_copied = 0;			// total bytes copied initializing
    int bytes_copied, buffer_index = 0;	// bytes_copied and buffer_index variables 
    char data[O_BUFSIZ + 1];            

    int running = 1;                    // Reference from Linux Book, Chapter 14, Page 591
    void *shared_memory = (void *)0;   
    finite_buffer_st *shared_buffer;
    int shmid;

    // Printing info about Producer's process
    printf("Consumer's PID: %d\n", pid);

    // ----------------------------------------------------------------------------------------
    //                           Semaphores Initialization: S,N,E,G
    /*    
        Setting up the Semaphore variables                               
    */
    int sem_s_id = semget(S_KEY_ID, 1, 0666 | IPC_CREAT);
    int sem_n_id = semget(N_KEY_ID, 1, 0666 | IPC_CREAT);
    int sem_e_id = semget(E_KEY_ID, 1, 0666 | IPC_CREAT);
    int sem_g_id = semget(G_KEY_ID, 1, 0666 | IPC_CREAT);
    // --------------------------------------///////-------------------------------------------
    
    // ----------------------------------------------------------------------------------------
    //                             Shared Memory - Initializing
    /*                                      
            Creating shared memory using shmget 
            Reference from Linux Book, Chapter 14, Page 588
            SHM_KEY is 1101, defined in sem_identifiers  
    */
    if ( (shmid = shmget((key_t)SHM_KEY, sizeof(finite_buffer_st), 0666 | IPC_CREAT)) == -1 ) {
        fprintf(stderr, "shmget failed .. shared memory was not created for this process\n");
        exit(EXIT_FAILURE);
    }
    /*      
            Attaching shared memeory to the address space of the process using shmat
            Reference from Linux Book, Chapter 14, Page 588-589
    */
    shared_memory = shmat(shmid, (void *)0, 0);
    if ( shared_memory == ((void *)-1) ) {
        fprintf(stderr, "shmat failed .. shared memory did not attach to the address space of this process\n");
        exit(EXIT_FAILURE);
    }  

    //printf("Memory attached at %p\n", shared_memory);
    printf("Consumer %d shared memory successfully established. \n", pid);

    shared_buffer = (finite_buffer_st *) shared_memory;
    // --------------------------------------///////-------------------------------------------


    // ----------------------------------------------------------------------------------------
    /*
                                            Consumer
    */
    // open establishes an access path to output file
    // Output text file output_text.txt  
    int output_file = open(OUTPUT_FILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
    if ( output_file == -1 )
    {                      
        // if output_file returns -1, it fails 
        fprintf(stderr,"Failed to establish access path to output file ..\n");
        exit(EXIT_FAILURE);
    }

    // Consumer waits until it is available 
    wait(sem_g_id);

    // struct stat st;
    // stat(OUTPUT_FILE, &st);
    int file_size = shared_buffer -> file_size; // 
    // int file_output_size = st.st_size;
    // printf("The file size of '%s' is %d bytes, original\n", OUTPUT_FILE, file_size);
    // printf("The file size of '%s' is %d bytes\n", OUTPUT_FILE, file_output_size);

    while(file_size > total_bytes_copied){
        wait(sem_n_id);
        wait(sem_s_id);
		
		// CRITICAL SECTION
	    // Reading from the shared memory, refer to Linux Book, Ch14, Page 592-593
        // this is the take method, critical section of consumer
        strcpy(data, shared_buffer -> shared_mem[buffer_index].buffer);
        bytes_copied = shared_buffer -> shared_mem[buffer_index].count;
		// END OF CRITICAL SECTION

        signal(sem_s_id);
        signal(sem_e_id);
        
         // Increment buffer index
        if (++buffer_index == NUMBER_OF_BUFFERS) buffer_index = 0;   
        if(bytes_copied != write(output_file, data, bytes_copied)){
            fprintf(stderr, "Size mismatch error when copying from Buffer to Output File\n");
        }
		
        // Updating the total_bytes_copied
        total_bytes_copied += bytes_copied;
    }
    // --------------------------------------///////-------------------------------------------

    char statement[] = "bytes from shared memory, wrote those bytes to output file named 'output_text'.";
    printf("Consumer consumed %d %s\n", total_bytes_copied, statement);
    printf("Done.\n");

    shmdt(shared_memory);       // Detach shared memory segment
    shmctl(shmid, IPC_RMID, 0); // Deleting the shared memory segment
    close(output_file);         // terminates association between file descriptor and file
    exit(EXIT_SUCCESS);
}
