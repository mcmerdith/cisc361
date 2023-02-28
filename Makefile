all: taste

taste: main.o add.o print.o freeList.o tastyDelete.o
	gcc -g main.o add.o print.o freeList.o tastyDelete.o -o taste

main.o: main.c mp3.h
	gcc -g -c main.c

add.o: add.c mp3.h
	gcc -g -c add.c

print.o: print.c mp3.h
	gcc -g -c print.c

freeList.o: freeList.c mp3.h
	gcc -g -c freeList.c

tastyDelete.o: tastyDelete.c mp3.h
	gcc -g -c tastyDelete.c

clean:
	\rm *.o taste
