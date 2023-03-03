CC=gcc
# CC=gcc -Wall

mysh: sh.o shell-builtins.o get_path.o search_path.o
	$(CC) -g sh.o shell-builtins.o get_path.o search_path.o -o mysh

sh.o: sh.c sh.h shell-builtins.h search_path.h
	$(CC) -g -c sh.c

shell-builtins.o: shell-builtins.c shell-builtins.h search_path.h
	$(CC) -g -c shell-builtins.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

search_path.o: search_path.c search_path.h get_path.h
	$(CC) -g -c search_path.c

clean:
	rm -rf *.o mysh
