/* Lift Sim B Header File */
/* Defines the main method declarations struct used for shared memory.*/

#ifndef LIFT_SIM_B_H
#define LIFT_SIM_B_H

/* The main line for Part B. */
int main(int argc, char **argv);

/* A struct containing all the fields to be stored in shared memory. */
typedef struct Shared {
    int maxBuffSize; /* The maximum amount of requests that the buffer can store. */
    int requestTime; /* The time in seconds that a lift takes to perform a request. */
    int end; /* Boolean indicating whether there are any requests left to read (if not the program can end once requests have been processed). */
    sem_t bufferLock; /* Binary semaphore used as a mutex lock for accessing the buffer. */
    sem_t freeSpaces; /* Counting semaphore indicating how many free spaces are avaliable in the buffer/how many producer processes are waiting */
    sem_t fullSpaces; /* Counting semaphore indicating how many filled spaces are avaliable in the buffer/how many consumer processes are waiting */
    LiftRequest buffer[100]; /* Buffer used to store the lift requests. Has a static number of spaces in Part B so that enough shared memory can be allocated. If maxBuffSize is <100 the remaining spaces are unused. */
    int in; /* 'Pointer' to the next free space in the buffer. */ 
    int out; /* 'Pointer' to the next filled space in the buffer */
    sem_t outputLock; /* Binary semaphore used as a mutex lock for accessing the output file. */
    Lift liftReturn[3]; /* Array used to return the lift structs back to the parent process. */
} Shared;

Shared* shared; /* Pointer to the Shared struct in shared memory */

/* Lift Request Function */
/* Reads lift requests from the input file and places them in the shared buffer. */
void request(); 

/* Lift Operation Function */
/* Retrieves lift requests from the shared buffer and performs the specified operations. Takes a pointer to a Lift struct as an argument. */
void lift(Lift* lift);

#endif