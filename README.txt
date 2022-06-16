LIFT SIMULATOR README
Author: Rohan Khayech

HOW TO COMPILE AND RUN
    PART A - THREAD IMPLEMENTATION
        
        1. Use the avaliable sim_input file or replace it with another valid input file
           (as per the assignment spec).
        2. Open a UNIX terminal in this folder.
        3. run 'make lift_sim_A' to compile the program.
        4. run './lift_sim_A m t' to run the program.
            where:
            m = buffer size (must be >= 1)
            t = request time (seconds)
        5. Output will be written to the sim_out file.

    PART B - PROCESS IMPLEMENTATION

        1. Use the avaliable sim_input file or replace it with another valid input file
           (as per the assignment spec).
        2. Open a UNIX terminal in this folder.
        3. run 'make lift_sim_B' to compile the program.
        4. run './lift_sim_B m t' to run the program.
            where:
            m = buffer size (must be >= 1 and <=100)
            t = request time (seconds) 
            (t must be >= 1, see known issues in assignment report)
        5. Output will be written to the sim_out file.