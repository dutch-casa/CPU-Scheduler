# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2

# Targets and dependencies
TARGET = scheduler
SRC = scheduler.c
HDR = scheduler.h
OBJ = $(SRC:.c=.o)

# Default rule
all: $(TARGET)

# Linking the final executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ)

# Compiling object files
scheduler.o: scheduler.c scheduler.h
	$(CC) $(CFLAGS) -c scheduler.c -o scheduler.o

# Clean rule to remove compiled files
clean:
	rm -f $(OBJ) $(TARGET)

# Run the program with default parameters (example usage)
run: all
	./$(TARGET) task_list.txt FCFS

# Phony targets
.PHONY: all clean run
