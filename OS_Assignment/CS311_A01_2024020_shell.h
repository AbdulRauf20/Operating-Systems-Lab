#ifndef SHELL_H
#define SHELL_H

void builtin_cd(char *path);
void builtin_pwd();
void execute_line(char *line);

#endif
