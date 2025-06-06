# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g

# Source files
SRCS = park.c threads.c queue.c car_state.c passenger_cond.c timestamps.c simulation_stats.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = park

# Default rule
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compile each .c to .o
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)
