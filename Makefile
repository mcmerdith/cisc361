CC=gcc
# CC=gcc -Wall

mysh: shell-with-builtin.o shell-builtins.o get_path.o which.o
	$(CC) -g shell-with-builtin.o shell-builtins.o get_path.o which.o -o mysh

shell-with-builtin.o: shell-with-builtin.c sh.h
	$(CC) -g -c shell-with-builtin.c

shell-builtins.o: shell-builtins.c sh.h
	$(CC) -g -c shell-builtins.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

which.o: which.c get_path.h
	$(CC) -g -c which.c

clean:
	rm -rf *.o mysh
