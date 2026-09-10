app: main.o helper.o
	gcc -o app main.o helper.o

main.o: main.c
	gcc -Wall -c main.c

helper.o: helper.c
	gcc -Wall -c helper.c

clean:
	rm -f app main.o helper.o
