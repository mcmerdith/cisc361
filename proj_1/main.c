//
// adapted from http://www.cprogramming.com/snippets/source-code/singly-linked-list-insert-remove-add-count
//

#include "mp3.h"
#define BUFFERSIZE 128

mp3_t *head;
mp3_t *tail;

void insert(char *artist, char *title, int runtime);
void removeMp3(char *artist);
void print(int bBackward);
void freeList();

/*
Print the options:
  (1) add an MP3 to the list
  (2) delete MP3(s) from the list
  (3) print the list from the beginning to the end
  (4) print the list from the end to the beginning
  (5) exit the program
*/
void printOptions()
{
  printf("\nList Operations\n");
  printf("===============\n");
  printf("(1) Add Track\n");
  printf("(2) Remove Artist Tracks\n");
  printf("(3) Print list\n");
  printf("(4) Print list backward\n");
  printf("(5) Exit\n");
}

// Read an integer and newline from stdin into i and buff respectively
// exits if read fails
void readInt(char *prompt, int *i)
{
  char buff;

  printf("%s", prompt);

  if (scanf("%d%c", i, &buff) <= 0)
  {
    printf("NaN\n");
    exit(1);
  }
}

// fgets BUFFERSIZE characters from stdin
// Replaces newline with null-byte
void readString(char *prompt, char *buff)
{
  do
  {
    printf("%s", prompt);
  } while (fgets(buff, BUFFERSIZE, stdin) == NULL);

  buff[strlen(buff) - 1] = '\0'; // override \n to become \0
}

// Read an artist, track, and run time into their respective variables from stdin
void readMp3Parameters(char *artistbuff, char *titlebuff, int *runtime)
{
  readString("Enter the artist name : ", artistbuff);
  readString("Enter the track title : ", titlebuff);
  readInt("Enter the run time : ", runtime);
}

int main()
{
  // Use the same variables for every run
  int choice, runtime, index;
  char artistbuffer[BUFFERSIZE], titlebuffer[BUFFERSIZE];

  // Start with no tracks
  head = NULL;
  tail = NULL;

  while (1)
  {
    printOptions();

    readInt("Enter your choice : ", &choice);

    switch (choice)
    {
    case 1:
      // Add
      readMp3Parameters(artistbuffer, titlebuffer, &runtime);
      insert(artistbuffer, titlebuffer, runtime);
      break;
    case 2:
      // Remove
      readString("Enter the artist name : ", artistbuffer);
      removeMp3(artistbuffer);
      break;
    case 3:
      // forward
      print(0);
      break;
    case 4:
      // backward
      print(1);
      break;
    case 5:
      // exit
      freeList();
      return 0;
    default:
      printf("Invalid option\n");
    }
  }
  return 0;
}
