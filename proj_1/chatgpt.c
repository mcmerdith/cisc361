
/*
############################################################  DISCLAIMER  ############################################################
This code was written by ChatGPT using the prompt provided on canvas.
I am NOT attempting to claim this as my actual work, see the tarball file for my solution
I have not edited this file at all, this is the raw output from ChatGPT
The only problem I identified was that it doesn't free the memory when option 5 is selected (but then the program exits so its fine?)
######################################################################################################################################
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100

struct MP3
{
  char *artist;
  char *title;
  int runtime;
  struct MP3 *next;
  struct MP3 *prev;
};

struct MP3 *head = NULL;
struct MP3 *tail = NULL;

// Function to add an MP3 to the end of the list
void add_mp3()
{
  char artist[MAX_LEN], title[MAX_LEN];
  int runtime;

  printf("Enter artist name: ");
  fgets(artist, MAX_LEN, stdin);
  artist[strcspn(artist, "\n")] = 0;

  printf("Enter song title: ");
  fgets(title, MAX_LEN, stdin);
  title[strcspn(title, "\n")] = 0;

  printf("Enter runtime (in seconds): ");
  scanf("%d", &runtime);
  getchar();

  struct MP3 *new_mp3 = (struct MP3 *)malloc(sizeof(struct MP3));
  new_mp3->artist = (char *)malloc(strlen(artist) + 1);
  new_mp3->title = (char *)malloc(strlen(title) + 1);
  strcpy(new_mp3->artist, artist);
  strcpy(new_mp3->title, title);
  new_mp3->runtime = runtime;
  new_mp3->next = NULL;

  if (tail == NULL)
  {
    head = tail = new_mp3;
    new_mp3->prev = NULL;
  }
  else
  {
    tail->next = new_mp3;
    new_mp3->prev = tail;
    tail = new_mp3;
  }
}

// Function to delete MP3(s) by artist name
void delete_mp3()
{
  char artist[MAX_LEN];

  printf("Enter artist name: ");
  fgets(artist, MAX_LEN, stdin);
  artist[strcspn(artist, "\n")] = 0;

  struct MP3 *curr = head;
  while (curr != NULL)
  {
    if (strcmp(curr->artist, artist) == 0)
    {
      if (curr->prev != NULL)
      {
        curr->prev->next = curr->next;
      }
      else
      {
        head = curr->next;
      }

      if (curr->next != NULL)
      {
        curr->next->prev = curr->prev;
      }
      else
      {
        tail = curr->prev;
      }

      free(curr->artist);
      free(curr->title);
      free(curr);

      curr = head;
    }
    else
    {
      curr = curr->next;
    }
  }
}

// Function to print the list from beginning to end
void print_list_forward()
{
  struct MP3 *curr = head;
  while (curr != NULL)
  {
    printf("Artist: %s\n", curr->artist);
    printf("Title: %s\n", curr->title);
    printf("Runtime: %d\n", curr->runtime);
    printf("\n");
    curr = curr->next;
  }
}

// Function to print the list from end to beginning
void print_list_reverse()
{
  struct MP3 *curr = tail;
  while (curr != NULL)
  {
    printf("Artist: %s\n", curr->artist);
    printf("Title: %s\n", curr->title);
    printf("Runtime: %d\n", curr->runtime);
    printf("\n");
    curr = curr->prev;
  }
}

int main()
{
  int choice;
  while (1)
  {
    printf("1. Add MP3\n");
    printf("2. Delete MP3\n");
    printf("3. Print list (forward)\n");
    printf("4. Print list (reverse)\n");
    printf("5. Exit\n");
    printf("Enter your choice: ");
    scanf("%d", &choice);
    getchar();

    switch (choice)
    {
    case 1:
      add_mp3();
      break;
    case 2:
      delete_mp3();
      break;
    case 3:
      print_list_forward();
      break;
    case 4:
      print_list_reverse();
      break;
    case 5:
      exit(0);
    default:
      printf("Invalid choice\n");
      break;
    }
  }

  return 0;
}