all: mp3 chatgpt

chatgpt: chatgpt.o
	gcc -g chatgpt.o  -o chatgpt

chatgpt.o:
	gcc -g -c chatgpt.c

mp3: main.o add.o remove.o print.o freeList.o
	gcc -g main.o add.o remove.o print.o freeList.o -o mp3

main.o: main.c mp3.h
	gcc -g -c main.c

add.o: add.c mp3.h
	gcc -g -c add.c

remove.o: remove.c mp3.h
	gcc -g -c remove.c

print.o: print.c mp3.h
	gcc -g -c print.c

freeList.o: freeList.c mp3.h
	gcc -g -c freeList.c

clean:
	\rm *.o mp3 chatgpt
