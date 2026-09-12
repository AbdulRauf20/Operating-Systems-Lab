#ifndef PROCESS_H
#define PROCESS_H

#include <sys/types.h>

void init_processes();
void add_process(pid_t pid, const char *name);
void remove_process(pid_t pid);
void cleanup_zombies();
void builtin_ps();
void builtin_kill(pid_t pid);

#endif
