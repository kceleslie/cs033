CC = gcc
GEN = generator
SOV = solver
CFLAGS = -g -Wall -Wextra -DFULL

GEN_OBJS = generator.c common.o
SOV_OBJS = solver.c common.o

all:  generator

common.o: common.c
	$(CC) $(CFLAGS) -c common.c

generator: $(GEN_OBJS)
	$(CC) $(CFLAGS) -o $(GEN) $(GEN_OBJS)

solver:	$(SOV_OBJS)
	$(CC) $(CFLAGS) -o $(SOV) $(SOV_OBJS)

clean:
	rm -f $(GEN)
