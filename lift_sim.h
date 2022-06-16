/* Lift Sim Header File */
/* Defines the lift and lift request struct and declares common functions for both Part A and Part B to avoid duplication of code. */

#ifndef LIFT_SIM_H
#define LIFT_SIM_H

/* A lift request. */
typedef struct LiftRequest {
    int rNum; /*The request number. */
    int from; /* The floor at which the request is made. */
    int to; /* The destination floor. */
} LiftRequest;

/* Struct containing data about a lift. */
typedef struct Lift {
    int lNum; /* The lift number. */
    int floor; /* The current floor of the lift. */
    int numReqs; /* The total number of requests this lift has performed. */
    int totMoves; /* Total number of floors the lift has moved through. */
} Lift;

/* Initilize Lift Function */
/* Allocates a new lift struct with default values and a unqiue lift number. */
/* Returns a pointer to the allocated lift.  Calling class must free the lift when finished with it. */
Lift* initLift();

/* Read Request Function */
/* Reads a request from the specified input file and returns a pointer to it. */
/* Takes a file pointer as an argument. Calling class must open the file before calling this function and only close it after ALL calls to this function. */
/* This ensures lines are read one by one and duplicate reads are not performed. */
/* If the next line does not contain a valid request or 100 requests have been read the function will return NULL */
LiftRequest* readRequest(FILE* sim_input);

/* Move Lift Function */
/* Moves the given lift to the request floor, then to the destination floor of the given request. */
/* The function then prints the details of the operation to the console and the output file. */
void moveLift(Lift* lift, LiftRequest* LiftRequest);
#endif