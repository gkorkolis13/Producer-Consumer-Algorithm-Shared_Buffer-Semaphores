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
    int bytes_copied, buffer_index = 0; // bytes_copied and buffer_index variables 
    char data[O_BUFSIZ];                // Array of 128 characters/bytes
    char file_data[BUFSIZ];             // Array of 8192 elements/characters, 8192 bytes
                                        // Default buffer size for my computer
    
    int running = 1;                    // Reference from Linux Book, Chapter 14, Page 591
    void *shared_memory = (void *)0;    
    finite_buffer_st *shared_buffer;
    int shmid;                          

    // Printing info about Producer's process
    printf("Producer's PID: %d\n", pid);

    // ----------------------------------------------------------------------------------------
    /*      
                                    Semaphores Initialization: S,N,E,G
    */
    int sem_s_id = semget(S_KEY_ID, 1, 0666 | IPC_CREAT);
    int sem_n_id = semget(N_KEY_ID, 1, 0666 | IPC_CREAT);
    int sem_e_id = semget(E_KEY_ID, 1, 0666 | IPC_CREAT);
    int sem_g_id = semget(G_KEY_ID, 1, 0666 | IPC_CREAT);


    if ( !set_semvalue(sem_s_id, S_SEMAPHORE_VALUE) ){
        fprintf(stderr, "Failed to initialize Semaphore S\n");
        exit(EXIT_FAILURE);
    }
    
    if ( !set_semvalue(sem_n_id, N_SEMAPHORE_VALUE) ){
        fprintf(stderr, "Failed to initialize Semaphore N\n");
        exit(EXIT_FAILURE);
    }

    if ( !set_semvalue(sem_e_id, E_SEMAPHORE_VALUE) ){
        fprintf(stderr, "Failed to initialize Semaphore E\n");
        exit(EXIT_FAILURE);
    }   
    
    if ( !set_semvalue(sem_g_id, G_SEMAPHORE_VALUE) ){
        fprintf(stderr, "Failed to initialize Semaphore G\n");
        exit(EXIT_FAILURE);
    }

    //printf("Producer %d semaphores successfully established. \n", pid);
    // --------------------------------------///////-------------------------------------------
    
    // ----------------------------------------------------------------------------------------
    //                                 Shared Memory - Initializing
    /*                                      
        Creating shared memory using shmget 
        Reference from Linux Book, Chapter 14, Page 588
        SHM_KEY is 1101, defined in sem_identifiers  
    */
    if ( (shmid = shmget((key_t)SHM_KEY, sizeof(finite_buffer_st), 0666 | IPC_CREAT)) == -1 ){
        fprintf(stderr, "shmget failed .. shared memory was not created for this process\n");
        exit(EXIT_FAILURE);
    }

    /*      
        Shared Memory - Attaching shared memeory to the address space of the process using shmat
        Reference from Linux Book, Chapter 14, Page 588-589
    */
    shared_memory = shmat(shmid, (void *)0, 0);
    if ( shared_memory == ((void *)-1) ){
        fprintf(stderr, "shmat failed .. shared memory was not attached to address space for this process\n");
        exit(EXIT_FAILURE);
    }
    //printf("Memory attached at %p\n", shared_memory);
    printf("Producer %d shared memory successfully established. \n", pid);
    // Reference from Linux Book, Chapter 14, Page 5
    shared_buffer = (finite_buffer_st *) shared_memory;
    // --------------------------------------///////-------------------------------------------
    
    // ----------------------------------------------------------------------------------------
    //                                          Producer  
    /* 
        open establishes an access path to input file, 
        It returns an non-negative integer if successfull
        Input text file "text.txt" 
    */  
    int input_file = open(INPUT_FILE, O_RDONLY);    
    if ( input_file == -1) 
    {   
        // if input_file returns -1, it failed to establish access path 
        fprintf(stderr,"Failed to establish access path to input file ..\n");
        exit(EXIT_FAILURE);
    }
    
    /* 
        Getting information from the input file like size of file in bytes
        Setting the 
    */
    struct stat st;
    stat(INPUT_FILE, &st);
    //int file_input_size = st.st_size;
    shared_buffer -> file_size = st.st_size;
    //printf("The input file size of '%s' is %d bytes\n", INPUT_FILE, file_input_size);
    
    signal(sem_g_id);

    while( (bytes_copied = read(input_file, file_data, BUFSIZ)) != 0 ){
        if (bytes_copied < 0){
            fprintf(stderr, "Error reading from file\n");
            exit(EXIT_FAILURE);
        }	    

	    // Size of the data already been copied to shared memory
	    int size = 0;
	    
	    while(bytes_copied > size){
	        // Copying files from read file to a buffer that will be sent to shared memory.
	        strncpy(data, file_data+size, O_BUFSIZ);
       
            size += O_BUFSIZ;
            int copy = O_BUFSIZ;

            //checking the case where the data is not O_BUFSIZ size
            if(size > bytes_copied){
                copy -= (size - bytes_copied);
            }
			
            wait(sem_e_id);
            wait(sem_s_id);

			// CRITICAL SECTION
	        // Clearing the shared memory
	        shared_buffer -> shared_mem[buffer_index].count = 0;
	        memset( shared_buffer -> shared_mem[buffer_index].buffer, 0, sizeof(shared_buffer -> shared_mem[buffer_index].buffer));	
	        // Adding data to shared memory
            // Setting the count for the copied data
            shared_buffer -> shared_mem[buffer_index].count = copy;    
            // Copy the data that was read
             strncpy( shared_buffer -> shared_mem[buffer_index].buffer, data, copy );  
			// END OF CRITICAL SECTION

            signal(sem_s_id);   
            signal(sem_n_id);

            // Increment buffer index
            if (++buffer_index == NUMBER_OF_BUFFERS) buffer_index = 0;            
            memset(data, '\0', sizeof(data));
        }
    }
    // --------------------------------------///////-------------------------------------------

    char statement[] = "bytes to shared memory, read those bytes from the input file named 'text.txt'.";
    printf("Producer produced %ld %s\n", st.st_size, statement);
    printf("Done.\n");

    shmdt(shared_memory);       // Detach shared memory segment
    shmctl(shmid, IPC_RMID, 0); // Deleting the shared memory segment
    close(input_file);          // terminates association between file descriptor and file
    exit(EXIT_SUCCESS);
}
