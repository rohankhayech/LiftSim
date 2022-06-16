# Lift Simulator

## Description

This C program simulates 3 lifts handling a singular queue of requests to move from floor to floor. This demonstrates knowledge and use of parallel programming in the C language. 

This repository contains two implementations of the simulator as described below:

### Implementation A - Using Threads
Program A has been implemented using the POSIX threads (Pthreads) library, using mutex locks and condition variables to handle access to shared resources.

### Implementation B - Using Processes
Program B has been implemented using forked processes, using System V shared memory to share resources and POSIX binary and counting semaphores to control access.

## How to Compile and Run
### A - Thread Implementation
    1. Use the avaliable sim_input file containing a list of requests from floor to floor.
    2. Open a UNIX terminal in this folder.
    3. run 'make lift_sim_A' to compile the program.
    4. run './lift_sim_A m t' to run the program.
        where:
        m = buffer size (must be >= 1)
        t = request time (seconds)
    5. Output will be written to the sim_out file.

### B - Process Implementation
    1. Use the avaliable sim_input file  containing a list of requests from floor to floor.
    2. Open a UNIX terminal in this folder.
    3. run 'make lift_sim_B' to compile the program.
    4. run './lift_sim_B m t' to run the program.
        where:
        m = buffer size (must be >= 1 and <=100)
        t = request time (seconds) (must be >= 1)
    5. Output will be written to the sim_out file.