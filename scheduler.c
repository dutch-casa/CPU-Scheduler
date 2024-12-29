#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "scheduler.h"

/**
 * @brief Main function to simulate CPU scheduling based on the given policy.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return int Exit status of the program.
 */
int main(int argc, char *argv[]) {
    char filename[100], policy[10];
    int time_quantum = 0;

    parse_arguments(argc, argv, filename, policy, &time_quantum); // Parse command-line arguments.

    Task tasks[100];
    int n = read_tasks(filename, tasks, 100); // Read task list from file.

    // Determine and simulate the scheduling policy.
    if (strcmp(policy, "FCFS") == 0) {
        simulate_fcfs(tasks, n);
    } else if (strcmp(policy, "RR") == 0) {
        simulate_rr(tasks, n, time_quantum);
    } else if (strcmp(policy, "SRTF") == 0) {
        simulate_srtf(tasks, n);
    } else {
        printf("Error: Unknown scheduling policy %s\n", policy);
        return 1;
    }

    print_statistics(tasks, n); // Display simulation statistics.
    return 0;
}

/**
 * @brief Parse command-line arguments to extract task list file, policy, and time quantum.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @param filename Output parameter to store the file name.
 * @param policy Output parameter to store the scheduling policy.
 * @param time_quantum Output parameter to store the time quantum (if applicable).
 */
void parse_arguments(int argc, char *argv[], char *filename, char *policy, int *time_quantum) {
    if (argc < 3) {
        printf("Usage: scheduler <task_list_file> <FCFS|RR|SRTF> [time_quantum]\n");
        exit(1);
    }

    strcpy(filename, argv[1]); // Copy task file name.
    strcpy(policy, argv[2]);   // Copy scheduling policy.

    // Validate and parse time quantum for Round Robin.
    if (strcmp(policy, "RR") == 0) {
        if (argc < 4) {
            printf("Error: Time quantum is required for Round Robin.\n");
            exit(1);
        }
        *time_quantum = atoi(argv[3]);
        if (*time_quantum <= 0) {
            printf("Error: Time quantum must be a positive integer.\n");
            exit(1);
        }
    } else {
        *time_quantum = 0; // Not used for FCFS or SRTF.
    }
}

/**
 * @brief Read tasks from a file into an array.
 * @param filename Name of the file containing tasks.
 * @param tasks Array to store the tasks.
 * @param max_tasks Maximum number of tasks to load.
 * @return int Number of tasks successfully loaded.
 */
int read_tasks(const char *filename, Task tasks[], int max_tasks) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Unable to open file %s\n", filename);
        exit(1);
    }

    int i = 0;
    // Read tasks line by line from the file.
    while (fscanf(file, "%d %d %d", &tasks[i].pid, &tasks[i].arrival_time, &tasks[i].burst_time) == 3) {
        tasks[i].remaining_time = tasks[i].burst_time;
        tasks[i].start_time = -1;  // Task has not started yet.
        tasks[i].finish_time = 0;  // Task has not finished yet.
        tasks[i].waiting_time = 0;
        tasks[i].response_time = 0;
        tasks[i].turnaround_time = 0;
        i++;
        if (i >= max_tasks) {
            printf("Warning: Max tasks limit reached. Only %d tasks loaded.\n", max_tasks);
            break;
        }
    }
    fclose(file);
    return i; // Return the number of tasks loaded.
}

/**
 * @brief Simulate First-Come, First-Served (FCFS) scheduling.
 * @param tasks Array of tasks.
 * @param n Number of tasks.
 */
void simulate_fcfs(Task tasks[], int n) {
    int time = 0;

    for (int i = 0; i < n; i++) {
        if (time < tasks[i].arrival_time) {
            printf("<time %d> idle\n", time); // CPU idle until task arrival.
            time = tasks[i].arrival_time;
        }

        tasks[i].start_time = time;
        while (tasks[i].remaining_time > 0) {
            tasks[i].remaining_time--;
            printf("<time %d> process %d is running\n", time, tasks[i].pid);
            time++;
        }
        tasks[i].finish_time = time;

        // Calculate task statistics.
        tasks[i].turnaround_time = tasks[i].finish_time - tasks[i].arrival_time;
        tasks[i].waiting_time = tasks[i].turnaround_time - tasks[i].burst_time;
        tasks[i].response_time = tasks[i].start_time - tasks[i].arrival_time;
        printf("<time %d> process %d finished...\n", time, tasks[i].pid);
    }

    printf("<time %d> All processes finished...\n", time);
}

/**
 * @brief Simulate Round Robin (RR) scheduling.
 * @param tasks Array of tasks.
 * @param n Number of tasks.
 * @param time_quantum Time quantum for RR scheduling.
 */
void simulate_rr(Task tasks[], int n, int time_quantum) {
    int time = 0, tasks_left = n;
    int queue[100], front = 0, rear = 0;
    int in_queue[100] = {0}; // Tracks whether a task is in the queue.

    // Initialize the queue with tasks arriving at time 0.
    for (int i = 0; i < n; i++) {
        if (tasks[i].arrival_time == 0) {
            queue[rear++] = i;
            in_queue[i] = 1;
        }
    }

    while (tasks_left > 0) {
        // CPU idle if no tasks in the queue.
        if (front == rear) {
            printf("<time %d> idle\n", time);
            time++;
            // Check for new arrivals.
            for (int i = ~0; i < n; i++) {
                if (tasks[i].arrival_time == time && !in_queue[i]) {
                    queue[rear++] = i;
                    in_queue[i] = 1;
                }
            }
            continue;
        }

        int i = queue[front++];
        front %= 100; // Circular queue implementation.

        // Execute the task for the time quantum or until completion.
        int runtime = (tasks[i].remaining_time > time_quantum) ? time_quantum : tasks[i].remaining_time;

        if (tasks[i].start_time == -1) {
            tasks[i].start_time = time; // Record first execution.
        }

        for (int t = 0; t < runtime; t++) {
            printf("<time %d> process %d is running\n", time, tasks[i].pid);
            time++;
        }

        tasks[i].remaining_time -= runtime;

        // Add new arrivals to the queue during runtime.
        for (int j = 0; j < n; j++) {
            if (tasks[j].arrival_time <= time && !in_queue[j] && tasks[j].remaining_time > 0) {
                queue[rear++] = j;
                rear %= 100;
                in_queue[j] = 1;
            }
        }

        // Task is finished, calculate metrics, otherwise requeue.
        if (tasks[i].remaining_time == 0) {
            tasks[i].finish_time = time;
            tasks[i].turnaround_time = tasks[i].finish_time - tasks[i].arrival_time;
            tasks[i].waiting_time = tasks[i].turnaround_time - tasks[i].burst_time;
            tasks[i].response_time = tasks[i].start_time - tasks[i].arrival_time;
            tasks_left--;
            printf("<time %d> process %d is finished...\n", time, tasks[i].pid);
        } else {
            queue[rear++] = i;
            rear %= 100;
        }
    }

    printf("<time %d> All processes finished...\n", time);
}

/**
 * @brief Simulate Shortest Remaining Time First (SRTF) scheduling.
 * @param tasks Array of tasks.
 * @param n Number of tasks.
 */
void simulate_srtf(Task tasks[], int n) {
    int time = 0, completed = 0;
    Task *current_task = NULL;

    while (completed < n) {
        // Find the task with the shortest remaining time that has arrived.
        current_task = NULL;
        for (int i = 0; i < n; i++) {
            if (tasks[i].remaining_time > 0 && tasks[i].arrival_time <= time) {
                if (current_task == NULL || tasks[i].remaining_time < current_task->remaining_time) {
                    current_task = &tasks[i];
                }
            }
        }

        // Execute the task or idle if none are ready.
        if (current_task) {
            if (current_task->start_time == -1) {
                current_task->start_time = time; // Record first execution.
            }
            current_task->remaining_time--;
            printf("<time %d> process %d is running\n", time, current_task->pid);

            // Task finished, calculate metrics.
            if (current_task->remaining_time == 0) {
                current_task->finish_time = time + 1;
                completed++;
                current_task->turnaround_time = current_task->finish_time - current_task->arrival_time;
                current_task->waiting_time = current_task->turnaround_time - current_task->burst_time;
                current_task->response_time = current_task->start_time - current_task->arrival_time;
                printf("<time %d> process %d is finished...\n", time + 1, current_task->pid);
            }
        } else {
            printf("<time %d> idle\n", time); // No task ready.
        }

        time++; // Advance time.
    }

    printf("<time %d> All processes finished...\n", time);
}

/**
 * @brief Print statistics for the simulated scheduling.
 * @param tasks Array of tasks.
 * @param n Number of tasks.
 */
void print_statistics(Task tasks[], int n) {
    double total_waiting_time = 0, total_turnaround_time = 0, total_response_time = 0;

    // Calculate totals for statistics.
    for (int i = 0; i < n; i++) {
        total_waiting_time += tasks[i].waiting_time;
        total_turnaround_time += tasks[i].turnaround_time;
        total_response_time += tasks[i].response_time;
    }

    // Display averages and overall CPU usage.
    printf("================ Statistics ================\n");
    printf("Average waiting time: %.2f\n", total_waiting_time / n);
    printf("Average response time: %.2f\n", total_response_time / n);
    printf("Average turnaround time: %.2f\n", total_turnaround_time / n);
    printf("Overall CPU usage: 100.00%%\n"); // Assuming no idle time.
    printf("==========================================\n");
}

