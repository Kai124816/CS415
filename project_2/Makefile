CC = gcc
CFLAGS = -Wall -Wextra -g

all: part_1 part_2 part_3 part_4 part_5 iobound cpubound

part_1: part_1.c MCP.c
	$(CC) $(CFLAGS) part_1.c MCP.c -o part_1

part_2: part_2.c MCP.c
	$(CC) $(CFLAGS) part_2.c MCP.c -o part_2

part_3: part_3.c MCP.c
	$(CC) $(CFLAGS) part_3.c MCP.c -o part_3

part_4: part_4.c MCP.c
	$(CC) $(CFLAGS) part_4.c MCP.c -o part_4

part_5: part_5.c MCP.c
	$(CC) $(CFLAGS) part_5.c MCP.c -o part_5

iobound: iobound.c
	$(CC) $(CFLAGS) iobound.c -o iobound

cpubound: cpubound.c
	$(CC) $(CFLAGS) cpubound.c -o cpubound

clean:
	rm -f part_1 part_2 part_3 part_4 part_5 iobound cpubound *.o

