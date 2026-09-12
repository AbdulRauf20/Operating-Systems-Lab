#include "CS311_A01_2024641_shell.h"
#include "CS311_A01_2024641_process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

void builtin_cd(char *path) {
    if (path == NULL) {
        printf("cd: missing argument\n");
    } else {
        if (chdir(path) != 0) {
            perror("cd failed");
        }
    }
}

void builtin_pwd() {
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
    } else {
        perror("pwd failed");
    }
}

// Parses a single command, configures redirections via dup2, and executes it
void parse_and_exec(char *cmd) {
    char *infile = NULL;
    char *outfile = NULL;
    int append = 0;
    
    // Check for append redirection '>>'
    char *out_ptr = strstr(cmd, ">>");
    if (out_ptr != NULL) {
        append = 1;
        *out_ptr = '\0';
        outfile = strtok(out_ptr + 2, " \t");
    } else {
        // Check for overwrite redirection '>'
        out_ptr = strchr(cmd, '>');
        if (out_ptr != NULL) {
            *out_ptr = '\0';
            outfile = strtok(out_ptr + 1, " \t");
        }
    }

    // Check for input redirection '<'
    char *in_ptr = strchr(cmd, '<');
    if (in_ptr != NULL) {
        *in_ptr = '\0';
        infile = strtok(in_ptr + 1, " \t");
    }

    // Tokenize the command string into an argument array
    char *args[64];
    int i = 0;
    char *token = strtok(cmd, " \t");
    while (token != NULL) {
        args[i] = token;
        i++;
        token = strtok(NULL, " \t");
    }
    args[i] = NULL;

    if (args[0] == NULL) {
        exit(0); // Empty command
    }

    // Handle redirections using dup2
    if (infile != NULL) {
        int fd0 = open(infile, O_RDONLY);
        if (fd0 < 0) {
            perror("Failed to open input file");
            exit(1);
        }
        dup2(fd0, STDIN_FILENO);
        close(fd0);
    }

    if (outfile != NULL) {
        int fd1;
        if (append) {
            fd1 = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd1 = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        if (fd1 < 0) {
            perror("Failed to open output file");
            exit(1);
        }
        dup2(fd1, STDOUT_FILENO);
        close(fd1);
    }

    // Handle built-ins if they appear in a pipeline child process
    if (strcmp(args[0], "cd") == 0) {
        builtin_cd(args[1]);
        exit(0);
    } else if (strcmp(args[0], "pwd") == 0) {
        builtin_pwd();
        exit(0);
    } else if (strcmp(args[0], "ps") == 0) {
        builtin_ps();
        exit(0);
    } else if (strcmp(args[0], "kill") == 0) {
        if (args[1] != NULL) builtin_kill(atoi(args[1]));
        exit(0);
    }

    // Execute the normal external command
    if (execvp(args[0], args) < 0) {
        perror("Command execution failed");
        exit(1);
    }
}

// Processes the full user input line
void execute_line(char *line) {
    int background = 0;
    int len = strlen(line);
    
    // Clean trailing spaces and check for background '&'
    for (int i = len - 1; i >= 0; i--) {
        if (line[i] == ' ' || line[i] == '\t' || line[i] == '\n') {
            line[i] = '\0';
            continue;
        }
        if (line[i] == '&') {
            background = 1;
            line[i] = '\0';
        }
        break;
    }

    // Check if there is a pipe '|'
    char *pipe_ptr = strchr(line, '|');
    if (pipe_ptr != NULL) {
        *pipe_ptr = '\0'; // Split the string into two commands
        char *cmd1 = line;
        char *cmd2 = pipe_ptr + 1;

        int pipefd[2];
        if (pipe(pipefd) < 0) {
            perror("Pipe failed");
            return;
        }

        pid_t pid1 = fork();
        if (pid1 == 0) {
            // Child 1: Write output to pipe
            dup2(pipefd[1], STDOUT_FILENO);
            close(pipefd[0]);
            close(pipefd[1]);
            parse_and_exec(cmd1);
        }

        pid_t pid2 = fork();
        if (pid2 == 0) {
            // Child 2: Read input from pipe
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]);
            close(pipefd[0]);
            parse_and_exec(cmd2);
        }

        // Parent closes the pipe ends
        close(pipefd[0]);
        close(pipefd[1]);

        if (!background) {
            waitpid(pid1, NULL, 0);
            waitpid(pid2, NULL, 0);
        } else {
            printf("[Started pipe in background]\n");
            add_process(pid1, "pipe_cmd1");
            add_process(pid2, "pipe_cmd2");
        }
        return;
    }

    // Check if the single command is a built-in before forking
    char temp_line[1024];
    strncpy(temp_line, line, 1024);
    char *first_arg = strtok(temp_line, " \t");
    
    if (first_arg != NULL) {
        if (strcmp(first_arg, "cd") == 0) {
            char *path = strtok(NULL, " \t");
            builtin_cd(path);
            return;
        } else if (strcmp(first_arg, "pwd") == 0) {
            builtin_pwd();
            return;
        } else if (strcmp(first_arg, "ps") == 0) {
            builtin_ps();
            return;
        } else if (strcmp(first_arg, "kill") == 0) {
            char *pid_str = strtok(NULL, " \t");
            if (pid_str != NULL) {
                builtin_kill(atoi(pid_str));
            } else {
                printf("kill: missing pid\n");
            }
            return;
        }
    }

    // Single external command
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fork failed");
        return;
    }

    if (pid == 0) {
        parse_and_exec(line);
    } else {
        if (!background) {
            // Track the process and block until it finishes
            add_process(pid, line);
            waitpid(pid, NULL, 0);
            remove_process(pid);
        } else {
            printf("[Started in background] PID: %d\n", pid);
            add_process(pid, line);
        }
    }
}
