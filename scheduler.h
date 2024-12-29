#ifndef SCHEDULER_H
#define SCHEDULER_H

// Structure to represent a Task
typedef struct {
    int pid;              // Process ID
    int arrival_time;     // Arrival time
    int burst_time;       // Total burst time
    int remaining_time;   // Remaining burst time (for RR and SRTF)
    int start_time;       // Time the process starts
    int finish_time;      // Time the process finishes
    int waiting_time;     // Total waiting time
    int response_time;    // Response time
    int turnaround_time;  // Turnaround time
} Task;

// Function prototypes

// Parse command-line arguments
void parse_arguments(int argc, char *argv[], char *filename, char *policy, int *time_quantum);

// Read tasks from a file into an array
int read_tasks(const char *filename, Task tasks[], int max_tasks);

// Simulate First-Come-First-Serve scheduling
void simulate_fcfs(Task tasks[], int n);

// Simulate Round Robin scheduling
void simulate_rr(Task tasks[], int n, int time_quantum);

// Simulate Shortest Remaining Time First scheduling (to be implemented)
void simulate_srtf(Task tasks[], int n);

// Calculate and print statistics
void print_statistics(Task tasks[], int n);


#endif
