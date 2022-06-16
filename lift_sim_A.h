/* Lift Sim A Header File */
/* Defines the main method declarations and the outputLock as this is used in log_A.c */

#ifndef LIFT_SIM_A_H
#define LIFT_SIM_A_H

pthread_mutex_t outputLock; /* Mutex lock for accessing the output file.*/

/* The main line for Part A */
int main(int argc, char **argv);

/* Lift Request Function */
/* Reads lift requests from the input file and places them in the shared buffer. */
void *request(void *ptr);

/* Lift Operation Function */
/* Retrieves lift requests from the shared buffer and performs the specified operations. Takes a pointer to a Lift struct as an argument. (casted to a void pointer.) */
void *lift(void *ptr);

#endif