CC = gcc
CFLAGS = -Wall -Wextra -g
OBJS = MCP.o

all: part_1 part_2 part_3 part_4

part_1: part_1.c MCP.c
	$(CC) $(CFLAGS) part_1.c MCP.c -o part_1

part_2: part_2.c MCP.c
	$(CC) $(CFLAGS) part_2.c MCP.c -o part_2

part_3: part_3.c MCP.c
	$(CC) $(CFLAGS) part_3.c MCP.c -o part_3

part_4: part_4.c MCP.c
	$(CC) $(CFLAGS) part_4.c MCP.c -o part_4

clean:
	rm -f part_1 part_2 part_3 part_4 *.o
