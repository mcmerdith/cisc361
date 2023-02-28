//
// CISC361: HW #1: valgrind and memory sizes
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFERSIZE 128

typedef struct mp3 {
  char       *singer;
  char       *title;
  int        length;
  struct mp3 *next;
} mp3_t;

int main (int argc, char *argv[])
{
  char  buffer1[BUFFERSIZE];
  char  buffer2[BUFFERSIZE];
  mp3_t *mp3;
  int   len;

  printf("sizeof int [%ld]\n", sizeof(int));
  printf("sizeof int* [%ld]\n", sizeof(int *));
  printf("sizeof char* [%ld]\n", sizeof(char *));
  printf("sizeof mp3_t [%ld]\n", sizeof(mp3_t));
  printf("sizeof mp3_t* [%ld]\n", sizeof(mp3_t *));

  printf("Enter a singer: ");
  fgets(buffer1, BUFFERSIZE, stdin);
  printf("Enter a title: ");
  fgets(buffer2, BUFFERSIZE, stdin);

  mp3 = (mp3_t *) malloc(sizeof(mp3_t));

  len = (int) strlen(buffer1);
  printf("length [%d] of string %s", len, buffer1); 
  buffer1[len - 1] = '\0';               // why minus 1 ???
  mp3->singer = (char *) malloc(len);
  strcpy(mp3->singer, buffer1);

  len = (int) strlen(buffer2);
  printf("length [%d] of string %s", len, buffer2); 
  buffer2[len - 1] = '\0';               // why minus 1 ??? Arrays (char* strings) are 0-indexed so len is out of range
  mp3->title = (char *) malloc(len);
  strcpy(mp3->title, buffer2);

  printf("signer is [%s]...\n", mp3->singer);
  printf("title is [%s]...\n", mp3->title);

  // We need to make sure we free() all the resources we malloc()'d
  free(mp3->singer);
  free(mp3->title);
  free(mp3);

  return 0;
}
