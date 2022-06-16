/* Lift Sim B C File*/
/* Implementation of the main functions for Part B using processes. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "lift_sim.h"
#include "log.h"
#include "lift_sim_B.h"

int shared_id; /* The ID of the shared memory segment */

/* The main line for Part B */
int main(int argc, char **argv)
{
    Lift *l1, *l2, *l3; /* Pointer to a lift struct holding data about the lift. */

    if (argc == 3 && atoi(argv[1]) >= 1 && atoi(argv[1]) <= 100 && atoi(argv[2]) >= 0) /* if command args are correct */
    {
        /* retrieve a segment of shared memory */
        shared_id = shmget(IPC_PRIVATE, sizeof(Shared), S_IRUSR | S_IWUSR);
        
        /* attach the shared memory to the main process */
        shared = (Shared*)shmat(shared_id,NULL,0);

        /* init end value to 0 */
        shared->end = 0;

        /* init binary semaphores (used as mutex locks) */
        sem_init(&shared->bufferLock, 1, 1);
        sem_init(&shared->outputLock, 1, 1);

        /* read the command args */
        shared->maxBuffSize = atoi(argv[1]);
        shared->requestTime = atoi(argv[2]);

        /* init the buffer 'pointers' */
        shared->in = 0;
        shared->out = 0;
        /* init the counting semaphores for the buffer */
        sem_init(&shared->freeSpaces,1,shared->maxBuffSize);
        sem_init(&shared->fullSpaces,1,0);

        /* empty the output log file */
        clearLog();

        /* allocate 3 lift structs */
        /* these are duplicated for each child process, must be freed in each child process */
        /* uses a static variable to assign a unique lift number, so must be created within the same process */
        l1 = initLift();
        l2 = initLift();
        l3 = initLift();


        /* create child processes */
        if (fork() == 0) /*child process (LiftR)*/
        {
            /* free the unused lift structs */
            free(l1);
            free(l2);
            free(l3);

            /* run the lift request function */
            request();
        } else /*parent process*/
        {
            if (fork() == 0) /*child process (Lift1)*/
            {
                /* free the unused lift structs */
                free(l2);
                free(l3);

                /* run the lift operation function */
                lift(l1);
            }
            else /*parent process*/
            {
                if (fork() == 0) /*child process (Lift2)*/
                {
                    /* free the unused lift structs */
                    free(l1);
                    free(l3);

                    /* run the lift operation function */
                    lift(l2);
                }
                else /*parent process*/
                {
                    if (fork() == 0) /*child process (Lift3)*/
                    {
                        /* free the unused lift structs */
                        free(l1);
                        free(l2);

                        /* run the lift operation function */
                        lift(l3);

                        

                    }
                    else /*parent process*/
                    {
                        /* free the original lifts */
                        free(l1);
                        free(l2);
                        free(l3);

                        /* wait for all child processes to terminate */
                        wait(NULL);
                        wait(NULL);
                        wait(NULL);
                        wait(NULL);

                        /* update the lift pointers to point to the returned lifts in shared memory */
                        l1 = &shared->liftReturn[0];
                        l2 = &shared->liftReturn[1];
                        l3 = &shared->liftReturn[2];

                        /* print out the total requests/movements to the log */
                        printTotals(l1,l2,l3);

                        /* destroy semaphores */
                        sem_destroy(&shared->bufferLock);
                        sem_destroy(&shared->outputLock);
                        sem_destroy(&shared->freeSpaces);
                        sem_destroy(&shared->fullSpaces);

                        /* detach the shared memory from the main process */
                        shmdt(shared);

                        /* close the shared memory */
                        shmctl(shared_id,IPC_RMID,NULL);

                        return 0;
                    }
                }
            }
            
        }

        return 0;
    }
    else
    {
        /* prompt user to rerun program with the correct args */
        printf("Usage: lift_sim_B m t\n");
        printf("\tm = buffer size (must be >= 1 and <=100)\n");
        printf("\tt = request time (seconds)\n");
        return 1; /* exit with error code 1 as the program couldn't run */
    }
}

/* Lift Request Function */
/* Reads lift requests from the input file and places them in the shared buffer. */
void request()
{
    LiftRequest *r; /* Points to a lift request from the input file. */
    FILE *sim_input; /* Pointer to the input file */

    /*attach the shared memory to the process */
    shared = (Shared *)shmat(shared_id, NULL, 0);

    /*open the input file*/
    sim_input = fopen("sim_input", "rb"); /* input file only accessed by the one process, doesn't require mutual exclusion */
    if (sim_input != NULL) /* if the file opened correctly */
    {
        while (!(shared->end)) /* while there are still requests to read */
        {
            /* retrieve a lift request from the input file if avaliable*/
            r = readRequest(sim_input);

            if (r != NULL) /* if the request is valid */
            {

                /* CRITICAL SECTION */
                sem_wait(&shared->freeSpaces); /*wait until there is a free space in the buffer*/
                sem_wait(&shared->bufferLock); /*wait until buffer is avaliable and obtain lock*/

                /* place the lift request struct in the shared memory buffer */
                shared->buffer[shared->in] = *r;

                /* print new request to console */ 
                printf("New Lift Request from Floor %d to Floor %d\n", r->from, r->to);
                printf("Request No: %d\n", r->rNum);

                /* print new request to the log file */
                printRequestToLog(r);

                /* free the request pointer */
                free(r);

                /* increase the in counter to the next free space in the buffer */
                shared->in = (shared->in + 1) % shared->maxBuffSize;

                sem_post(&shared->bufferLock); /* release lock on buffer */
                sem_post(&shared->fullSpaces); /*increase the number of filled spaces in the buffer*/

                /* END CRITICAL SECTION */
            }
            else /* no more requests in file or request was invalid, terminate process*/
            {
                shared->end = 1; /* liftR can terminate */
                printf("No more requests in file (or invalid request). Ending LiftR.\n");
            }
        }

        /*close the input file */
        fclose(sim_input);
    }
    else
    {
        printf("Error opening file.\n");
        shared->end = 1; /* liftR can terminate */
    }

    shmdt(shared);
}

/* Lift Operation Function */
/* Retrieves lift requests from the shared buffer and performs the specified operations. Takes a pointer to a Lift struct as an argument. */
void lift(Lift *lift)
{
    LiftRequest r; /* Stores the lift request taken from the buffer */
    int done = 0;  /* Boolean indicating whether there are requests left to perform (if not the Lift thread will terminate) */

    /*attach the shared memory to the process */
    shared = (Shared *)shmat(shared_id, NULL, 0);

    /* take and perform request if avaliable or wait until avaliable */
    while (done == 0)
    {
        int semVal; /* Stores the value of the full semaphore (counts how many requests are in the buffer). */
        
        sem_getvalue(&shared->fullSpaces,&semVal);

        if (shared->end == 1 && semVal<=0) /*if no more requests in file or buffer*/
        {
            done = 1; /* lift can terminate */
        }
        else
        {
            /* wait while buffer is empty or signalled to end */
            sem_wait(&shared->fullSpaces);

            sem_wait(&shared->bufferLock); /*wait until buffer is avaliable and obtain lock*/
            /* CRITICAL SECTION */

            /* retrieve the next request from the buffer and increase the out counter to the next place */
            r = shared->buffer[shared->out];
            shared->out = (shared->out + 1) % shared->maxBuffSize;

            /* perform the lift operation */
            moveLift(lift, &r);

            sem_post(&shared->bufferLock); /* release lock on buffer */
            /* END CRITICAL SECTION */
            sem_post(&shared->freeSpaces); /*increase the number of empty spaces in the buffer*/
            
            /* wait t seconds before retrieving another request */
            sleep(shared->requestTime);
            
        }    
    }

    /* return the lift struct to the main process via shared memory */
    shared->liftReturn[(lift->lNum)-1] = *lift;

    /* free the lift pointer */
    free(lift);

    shmdt(shared);
}