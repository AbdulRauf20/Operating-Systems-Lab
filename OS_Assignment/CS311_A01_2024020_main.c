#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linenoise.h"
#include "CS311_A01_2024641_shell.h"
#include "CS311_A01_2024641_process.h"

int main(int argc, char **argv) {
    char *line;

    // Initialize the process tracking array
    init_processes();

    // Main loop displaying the prompt
    while ((line = linenoise("mysh> ")) != NULL) {
        // Automatically reap finished background zombies before next command
        cleanup_zombies();

        if (line[0] != '\0') {
            linenoiseHistoryAdd(line);
            execute_line(line);
        }
        free(line);
    }

    return 0;
}