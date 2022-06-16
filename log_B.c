#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "log.h"
#include "lift_sim_B.h"

/* Clear Log Function */
/* Clears the output file so that it is empty for the fresh output. */
void clearLog()
{
    FILE *sim_out; /* Pointer to the output file. */
    
    sem_wait(&shared->outputLock); /* Wait until the output file is avaliable. */
    /* CRITICAL SECTION */
    sim_out = fopen("sim_out", "wb"); /*Open the output file in write mode. */
    if (sim_out != NULL) /* if the file opened correctly */
    {
        fprintf(sim_out, "\n"); /* This prints an empty line, overwriting the existing contents of the file as the file is opened in write mode. */

        if (ferror(sim_out))
        {
            printf("Error writing to sim_out.\n");
        }

        fclose(sim_out); /* close the file */
    }
    else
    {
        printf("Error opening sim_out.\n");
    }

    sem_post(&shared->outputLock); /* Release the output file to be used by other processes. */
    /* END CRITICAL SECTION */
}

/* Print Request to Log Function */
/* Prints details of the specified Lift Request to the output file. */
void printRequestToLog(LiftRequest* req)
{
    FILE *sim_out; /* Pointer to the output file. */

    sem_wait(&shared->outputLock); /* Wait until the output file is avaliable. */
    sim_out = fopen("sim_out", "ab"); /*Open the output file in append mode. */
    if (sim_out != NULL) /* if the file opened correctly */
    {
        /* Print request details */
        fprintf(sim_out, "\n----------------------------------------\n");
        fprintf(sim_out, "New Lift Request from Floor %d to Floor %d\n", req->from, req->to);
        fprintf(sim_out, "Request No: %d\n", req->rNum);

        if (ferror(sim_out))
        {
            printf("Error writing to sim_out.\n");
        }

        fclose(sim_out); /* close the file */
    }
    else
    {
        printf("Error opening sim_out.\n"); 
    }

    sem_post(&shared->outputLock); /* Release the output file to be used by other processes. */
}

/* Print Operation to Log Function */
/* Prints details of the specified Lift Operation to the output file. */
/* Takes a lift struct, a lift request, the previous floor of the lift, and the number of moves performed by this operation as arguments. */
void printOperationToLog(Lift* lift, LiftRequest* req, int prevFloor, int moves)
{
    FILE *sim_out; /* Pointer to the output file. */

    sem_wait(&shared->outputLock); /* Wait until the output file is avaliable. */
    sim_out = fopen("sim_out", "ab"); /*Open the output file in append mode. */
    if (sim_out != NULL)  /* if the file opened correctly */
    {
        /* print operation details */
        fprintf(sim_out, "\n----------------------------------------\n");
        fprintf(sim_out, "Lift-%d Operation\n", lift->lNum);
        fprintf(sim_out, "Previous position: Floor %d\n", prevFloor);
        fprintf(sim_out, "Request: Floor %d to Floor %d\n", req->from, req->to);
        fprintf(sim_out, "Detail operations:\n");
        fprintf(sim_out, "\tGo from Floor %d to Floor %d\n", prevFloor, req->from);
        fprintf(sim_out, "\tGo from Floor %d to Floor %d\n", req->from, req->to);
        fprintf(sim_out, "\t#Movement for this request: %d\n", moves);
        fprintf(sim_out, "\t#Request: %d\n", lift->numReqs);
        fprintf(sim_out, "\tGlobal #Request: %d\n", req->rNum);
        fprintf(sim_out, "\tTotal #Movement: %d\n", lift->totMoves);
        fprintf(sim_out, "Current position: Floor %d\n", lift->floor);

        if (ferror(sim_out))
        {
            printf("Error writing to sim_out.\n");
        }

        fclose(sim_out); /* close the file */
    }
    else
    {
        printf("Error opening sim_out.\n");
    }

    sem_post(&shared->outputLock); /* Release the output file to be used by other processes. */
}

/* Print Totals Function */
/* Prints the total number of requests and movements performed by all three lifts to the output file. */
void printTotals(Lift *l1, Lift *l2, Lift *l3)
{
    FILE *sim_out; /* Pointer to the output file. */

    sem_wait(&shared->outputLock); /* Wait until the output file is avaliable. */
    sim_out = fopen("sim_out", "ab"); /*Open the output file in append mode. */
    if (sim_out != NULL) /* if the file opened correctly */
    {
        /* print totals */
        fprintf(sim_out, "\n----------------------------------------\n");
        fprintf(sim_out, "Total number of requests: %d\n", l1->numReqs + l2->numReqs + l3->numReqs);
        fprintf(sim_out, "Total number of movements: %d\n", l1->totMoves + l2->totMoves + l3->totMoves);
        fprintf(sim_out, "\n----------------------------------------\n");

        if (ferror(sim_out))
        {
            printf("Error writing to sim_out.\n");
        }

        fclose(sim_out); /* close the file */
    }
    else
    {
        printf("Error opening sim_out.\n");
    }

    sem_post(&shared->outputLock); /* Release the output file to be used by other processes. */
}