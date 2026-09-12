all: mysh

mysh: CS311_A01_2024641_main.o CS311_A01_2024641_shell.o CS311_A01_2024641_process.o linenoise.o
	gcc -Wall -g CS311_A01_2024641_main.o CS311_A01_2024641_shell.o CS311_A01_2024641_process.o linenoise.o -o mysh

CS311_A01_2024641_main.o: CS311_A01_2024641_main.c
	gcc -Wall -g -c CS311_A01_2024641_main.c -o CS311_A01_2024641_main.o

CS311_A01_2024641_shell.o: CS311_A01_2024641_shell.c
	gcc -Wall -g -c CS311_A01_2024641_shell.c -o CS311_A01_2024641_shell.o

CS311_A01_2024641_process.o: CS311_A01_2024641_process.c
	gcc -Wall -g -c CS311_A01_2024641_process.c -o CS311_A01_2024641_process.o

linenoise.o: linenoise.c
	gcc -Wall -g -c linenoise.c -o linenoise.o

clean:
	rm -f CS311_A01_2024641_main.o CS311_A01_2024641_shell.o CS311_A01_2024641_process.o linenoise.o mysh