# Copyright (c) 2020 Rohan Khayech

# Makefile for the Lift Sim.
# Includes make targets for both Part A and Part B.
# See readme.txt for instructions on how to compile using make.

# Compiler info
CC = gcc
CFLAGS = -Wall -ansi -pedantic -Werror -std=c89 -pthread -g

# Executable and object files for Part A
EXECA = lift_sim_A
OBJA = lift_sim.o lift_sim_A.o log_A.o

# Executable and object files for Part B
EXECB = lift_sim_B
OBJB = lift_sim.o lift_sim_B.o log_B.o

# Make target for compiling Part A
$(EXECA) : $(OBJA)
	$(CC) $(OBJA) -o $(EXECA) -pthread

# Make target for compiling Part B
$(EXECB) : $(OBJB)
	$(CC) $(OBJB) -o $(EXECB) -pthread -lrt

# Make targets for compiling each C file

lift_sim.o : lift_sim.c lift_sim.h log.h
	$(CC) -c lift_sim.c $(CFLAGS)

lift_sim_A.o : lift_sim_A.c lift_sim_A.h log.h lift_sim.h
	$(CC) -c lift_sim_A.c $(CFLAGS) $(PFLAGS)

log_A.o : log_A.c lift_sim.h log.h
	$(CC) -c log_A.c $(CFLAGS) $(PFLAGS)

log_B.o : log_B.c lift_sim.h log.h
	$(CC) -c log_B.c $(CFLAGS) $(PFLAGS)
	
lift_sim_B.o : lift_sim_B.c lift_sim_B.h log.h lift_sim.h 
	$(CC) -c lift_sim_B.c $(CFLAGS) $(PFLAGS)
	
# Make target to remove all compiled objects and executables.
clean :
	rm -f $(OBJA) $(OBJB) $(EXECA) $(EXECB)