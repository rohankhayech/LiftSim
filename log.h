/*
 * Copyright (c) 2020 Rohan Khayech
 */

/* Log Header File */
/* Header file for both log_A.c and log_B.c */
/* Both files have the same method declarations, but different implementations */
#ifndef LOG_H
#define LOG_H
#include "lift_sim.h"

/* Clear Log Function */
/* Clears the output file so that it is empty for the fresh output. */
void clearLog();

/* Print Request to Log Function */
/* Prints details of the specified Lift Request to the output file. */
void printRequestToLog(LiftRequest *req);

/* Print Operation to Log Function */
/* Prints details of the specified Lift Operation to the output file. */
/* Takes a lift struct, a lift request, the previous floor of the lift, and the number of moves performed by this operation as arguments. */
void printOperationToLog(Lift* lift, LiftRequest* req,int prevFloor, int moves);

/* Print Totals Function */
/* Prints the total number of requests and movements performed by all three lifts to the output file. */
void printTotals(Lift* l1, Lift* l2, Lift* l3);
#endif