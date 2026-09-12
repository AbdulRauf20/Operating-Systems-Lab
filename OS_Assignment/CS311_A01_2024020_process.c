#include "CS311_A01_2024641_process.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

struct Process {
    pid_t pid;
    char name[128];
    int active;
};

// Global array to track processes to keep it beginner-friendly
struct Process proc_list[100];

void init_processes() {
    for(int i = 0; i < 100; i++) {
        proc_list[i].active = 0;
    }
}

void add_process(pid_t pid, const char *name) {
    for(int i = 0; i < 100; i++) {
        if(proc_list[i].active == 0) {
            proc_list[i].pid = pid;
            strncpy(proc_list[i].name, name, 127);
            proc_list[i].name[127] = '\0'; // Ensure null termination
            proc_list[i].active = 1;
            return;
        }
    }
    printf("Process list is full!\n");
}

void remove_process(pid_t pid) {
    for(int i = 0; i < 100; i++) {
        if(proc_list[i].active == 1 && proc_list[i].pid == pid) {
            proc_list[i].active = 0;
        }
    }
}

// Cleans up background processes that have finished running
void cleanup_zombies() {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        remove_process(pid);
    }
}

void builtin_ps() {
    cleanup_zombies(); // Refresh the list to remove dead background tasks
    printf("PID\tCMD\n");
    for(int i = 0; i < 100; i++) {
        if(proc_list[i].active == 1) {
            // Check if process is still responsive to signals
            if (kill(proc_list[i].pid, 0) == 0) {
                printf("%d\t%s\n", proc_list[i].pid, proc_list[i].name);
            } else {
                proc_list[i].active = 0; // Expunge its record if dead
            }
        }
    }
}

void builtin_kill(pid_t pid) {
    if (kill(pid, SIGTERM) == 0) {
        printf("Process %d terminated.\n", pid);
        remove_process(pid);
    } else {
        perror("kill failed");
    }
}
