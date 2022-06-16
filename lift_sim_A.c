/*
 * Copyright (c) 2020 Rohan Khayech
 */

/* Lift Sim A C File*/
/* Implementation of the main functions for Part A using threads. */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "lift_sim.h"
#include "log.h"
#include "lift_sim_A.h"


/* Shared variables */

int numItems;    /* The current amount of requests stored in the buffer. */
int maxBuffSize; /* The maximum amount of requests that the buffer can store. */
int requestTime; /* The time in seconds that a lift takes to perform a request. */
int end; /* Boolean indicating whether there are any requests left to read (if not the program can end once requests have been processed). */
pthread_mutex_t bufferLock; /* Mutex lock for accessing the buffer. */
pthread_cond_t fullCond; /* Condition variable. Threads will wait on this condition if the buffer is full. */
pthread_cond_t emptyCond; /* Condition variable. Threads will wait on this condition if the buffer is empty. */
LiftRequest **buffer; /* Buffer used to store the lift requests. Dynamic array of LiftRequest pointers. */
int in;  /* 'Pointer' to the next free space in the buffer. */
int out; /* 'Pointer' to the next filled space in the buffer. */

/* The main line for Part A */
int main(int argc, char **argv)
{
    pthread_t liftR; /* The Lift Request thread. */
    pthread_t lift1, lift2, lift3; /* A lift thread. */
    Lift *l1, *l2, *l3; /* Pointer to a lift struct holding data about the lift. */
    
    if (argc == 3 && atoi(argv[1]) >= 1 && atoi(argv[2]) >= 0) /* if command args are correct */
    {
        /* init end value to 0 */
        end = 0;

        /* read the command args */
        maxBuffSize = atoi(argv[1]);
        requestTime = atoi(argv[2]);

        /* allocate the buffer array */
        buffer = (LiftRequest**)calloc(maxBuffSize, sizeof(LiftRequest*));
        /* init the buffer 'pointers' and counter */
        in = 0;
        out = 0;
        numItems = 0;

        /* allocate 3 lift structs */
        l1 = initLift();
        l2 = initLift();
        l3 = initLift();

        /* init mutex locks */
        pthread_mutex_init(&bufferLock, NULL);
        pthread_mutex_init(&outputLock, NULL);

        /* init condition variables for the buffer */
        pthread_cond_init(&fullCond, NULL);
        pthread_cond_init(&emptyCond, NULL);

        /* empty the output log file */
        clearLog();

        /* create child threads */
        pthread_create(&liftR, NULL, request, NULL);
        pthread_create(&lift1, NULL, lift, (void*)l1);
        pthread_create(&lift2, NULL, lift, (void*)l2);
        pthread_create(&lift3, NULL, lift, (void*)l3);

        /* join and wait for all child threads to terminate */
        pthread_join(liftR, NULL);
        pthread_join(lift1, NULL);
        pthread_join(lift2, NULL);
        pthread_join(lift3, NULL);

        /* free the buffer pointer */
        free(buffer);

        /* print out the total requests/movements to the log */
        printTotals(l1,l2,l3);

        /* free the lift pointers */
        free(l1);
        free(l2);
        free(l3);

        /* destroy mutex locks */
        pthread_mutex_destroy(&bufferLock);
        pthread_mutex_destroy(&outputLock);

        /* destroy condition variables for the buffer */
        pthread_cond_destroy(&fullCond);
        pthread_cond_destroy(&emptyCond);

        return 0;
    }
    else
    {
        /* prompt user to rerun program with the correct args */
        printf("Usage: lift_sim_A m t\n");
        printf("\tm = buffer size (must be >= 1)\n");
        printf("\tt = request time (seconds)\n");
        return 1; /* exit with error code 1 as the program couldn't run */
    }
}

/* Lift Request Function */
/* Reads lift requests from the input file and places them in the shared buffer. */
void* request(void* ptr)
{
    LiftRequest *r; /* Points to a lift request from the input file. */
    FILE *sim_input; /* Pointer to the input file */

    /*open the input file*/
    sim_input = fopen("sim_input", "rb"); /* input file only accessed by the one process, doesn't require mutual exclusion */
    if (sim_input != NULL) /* if the file opened correctly */
    {
        while (!end) /* while there are still requests to read */
        {
            /* retrieve a lift request from the input file if avaliable*/
            r = readRequest(sim_input);

            if (r != NULL) /* if the request is valid */
            {

                /* CRITICAL SECTION */
                pthread_mutex_lock(&bufferLock); /* obtain lock for buffer */
                while (numItems == maxBuffSize) /* wait if buffer is full*/
                {
                    pthread_cond_wait(&fullCond, &bufferLock); /* wait until a consumer signals that the buffer is no longer full*/
                }

                /* place the lift request struct in the shared memory buffer */
                buffer[in] = r;

                /* print new request to console */
                printf("New Lift Request from Floor %d to Floor %d\n",r->from,r->to);
                printf("Request No: %d\n",r->rNum);

                /* print new request to the log file */
                printRequestToLog(r);

                /* increase the in counter to the next free space in the buffer */
                in = (in + 1) % maxBuffSize;
                numItems++;

                pthread_cond_broadcast(&emptyCond); /* signal to consumers that buffer is no longer empty*/

                pthread_mutex_unlock(&bufferLock); /* release lock on buffer */
                /* END CRITICAL SECTION */
            }
            else /* no more requests in file or request was invalid, terminate process*/
            {
                end = 1; /* liftR can terminate */
                printf("No more requests in file. Ending LiftR.\n");
            }
            
        }

        /*close the input file */
        fclose(sim_input);
    }
    else
    {
        printf("Error opening file.\n");
        end = 1; /* liftR can terminate */
    }
    return NULL;
}

/* Lift Operation Function */
/* Retrieves lift requests from the shared buffer and performs the specified operations. Takes a pointer to a Lift struct as an argument. (casted to a void pointer.) */
void *lift(void* ptr)
{
    Lift* lift; /* The current lift. */
    LiftRequest *r; /* Stores the lift request taken from the buffer */
    int done = 0; /* Boolean indicating whether there are requests left to perform (if not the Lift thread will terminate) */

    /* Cast and assign the lift pointer from the method argument */
    lift = (Lift*)ptr;

    /* take and perform request if avaliable or wait until avaliable */
    while (done==0)
    {

        pthread_mutex_lock(&bufferLock); /* obtain lock on buffer */
        /* CRITICAL SECTION */

        while (numItems == 0) /* while buffer is empty */
        {
            if (end == 0) 
            {
                pthread_cond_wait(&emptyCond, &bufferLock); /* wait until producer signals that the buffer is no longer empty */
            }
            else /*if no more requests in file or buffer*/
            {
                done = 1; /* lift can terminate */
                pthread_mutex_unlock(&bufferLock);
                break;
            }
        }

        if(done==0) /* skip the operation code if no more requests to take */
        {
            /* retrieve the next request from the buffer, increase the out counter to the next place and decrease the numItems in the buffer */
            r = buffer[out];
            out = (out + 1) % maxBuffSize;
            numItems--;

            pthread_cond_broadcast(&fullCond); /* signal to producers that buffer is no longer full */

            /* perform the lift operation */
            moveLift(lift, r);

            pthread_mutex_unlock(&bufferLock); /* release lock on buffer */

            /* END CRITICAL SECTION */

            /* free the request pointer */
            free(r);

            /* wait t seconds before retrieving another request */
            sleep(requestTime);
        }
    }
    return NULL;
}