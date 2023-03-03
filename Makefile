CC=gcc
# CC=gcc -Wall

mysh: sh.o shell_builtins.o get_path.o search_path.o
	$(CC) -g sh.o shell_builtins.o get_path.o search_path.o -o mysh

sh.o: sh.c sh.h shell_builtins.h search_path.h
	$(CC) -g -c sh.c

shell_builtins.o: shell_builtins.c shell_builtins.h search_path.h
	$(CC) -g -c shell_builtins.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

search_path.o: search_path.c search_path.h get_path.h
	$(CC) -g -c search_path.c

clean:
	rm -rf *.o mysh
