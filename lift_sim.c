/*
 * Copyright (c) 2020 Rohan Khayech
 */

/* Lift Sim C File */
/* Contains common functions for both Part A and Part B to avoid duplication of code. */ 

#include <stdio.h>
#include <stdlib.h>
#include "log.h"
#include "lift_sim.h"

/* Initilize Lift Function */
/* Allocates a new lift struct with default values and a unqiue lift number. */
/* Returns a pointer to the allocated lift.  Calling class must free the lift when finished with it. */
Lift* initLift()
{
    Lift* lift; /* Pointer to the new lift. */
    static int n = 1; /* Gives each new lift a unique identifier. */

    /* Allocate memory for the lift struct */
    lift = (Lift *)malloc(sizeof(Lift));

    /* Init the lift struct */
    lift->lNum = n;
    lift->floor = 1;
    lift->numReqs = 0;
    lift->totMoves = 0;

    n++; /* increase the static identifier for the next call */

    /* return the created lift */
    return lift; 
}

/* Read Request Function */
/* Reads a request from the specified input file and returns a pointer to it. */
/* Takes a file pointer as an argument. Calling class must open the file before calling this function and only close it after ALL calls to this function. */
/* This ensures lines are read one by one and duplicate reads are not performed. */
/* If the next line does not contain a valid request or 100 requests have been read the function will return NULL */
LiftRequest* readRequest(FILE* sim_input)
{
    LiftRequest* r; /* Points to the request read from the input file. */
    char line[10]; /* Buffer array which stores a line from the input file */
    int to; /* The floor at which the request is made. */ 
    int from; /* The destination floor. */

    static int n = 1; /*  Gives each new request a unique identifier. */

    r = NULL; /* Request is initilized to NULL until a valid request is read. */
    if (!feof(sim_input) && n<=100) /* if the end of file has not been reached or 100 requests have been read */
    {
        if (fgets(line, 10, sim_input)) /* get the next line of text and continue if this succeeded */
        {
            /* read the to and from floors from the line of text */
            int numRead = sscanf(line,"%d %d",&from,&to);
            if (numRead >= 2) /* if floors were read correctly */
            {
                /* Allocate a new LiftRequest struct */
                r = (LiftRequest*)malloc(sizeof(LiftRequest));

                /* initilize the new request */
                r->rNum = n;
                r->from = from;
                r->to = to;

                n++; /* increase the static identifier for the next call */
            }
            else /* if line was invalid */
            {
                printf("Invalid line. numRead = %d\n",numRead);
                printf("Line: %s \n",line);
            }
            
        }
        else /* if line was invalid */
        {
            printf("Couldn't get line.\n");
        }
        
    }
    
    /* return the request */
    return r;
}

/* Move Lift Function */
/* Moves the given lift to the request floor, then to the destination floor of the given request. */
/* The function then prints the details of the operation to the console and the output file. */
void moveLift(Lift* lift, LiftRequest* req)
{
    int prevFloor; /* The prev floor the lift was on. */
    int moves; /* The number of movements involved in the operation. */

    /* get the prev floor */
    prevFloor = lift->floor;

    /* calculate the number of moves from the prev floor to the request floor, then to the destination floor */
    moves = abs(prevFloor-req->from) + abs(req->from - req->to);

    /* update the lift state*/
    lift->totMoves += moves;
    lift->floor = req->to; /* moves the lift */
    lift->numReqs++;
    
    /* print details of operation to the screen */
    printf("Lift-%d Operation\n", lift->lNum);
    printf("Previous position: Floor %d\n",prevFloor);
    printf("Request: Floor %d to Floor %d\n",req->from,req->to);
    printf("Detail operations:\n");
    printf("\tGo from Floor %d to Floor %d\n",prevFloor,req->from);
    printf("\tGo from Floor %d to Floor %d\n",req->from,req->to);
    printf("\t#Movement for this request: %d\n",moves);
    printf("\t#Request: %d\n",lift->numReqs);
    printf("\tTotal #Movement: %d\n",lift->totMoves);
    printf("Current position: Floor %d\n",lift->floor);

    /* print details of the operation to the output file */
    printOperationToLog(lift,req,prevFloor,moves);
}