#include "mp3.h"

extern mp3_t *head;
extern mp3_t *tail;

// insert a track with artist, title, and runtime into a doubly linked list defined by globals head and tail
void insert(char *artist, char *title, int runtime)
{
  mp3_t *mp3;

  mp3 = (mp3_t *)malloc(sizeof(mp3_t));             // malloc space for MP3
  mp3->artist = (char *)malloc(strlen(artist) + 1); // malloc space for artist
  mp3->title = (char *)malloc(strlen(title) + 1);   // malloc space for title
  strcpy(mp3->artist, artist);                      // "assign" artist via copy
  strcpy(mp3->title, title);                        // "assign" title via copy
  mp3->runtime = runtime;                           // assign data value
  mp3->next = NULL;
  mp3->prev = NULL;

  if (head == NULL)
  {
    head = mp3; // add the first MP3
    tail = mp3; // its also the last
  }
  else
  {
    mp3->prev = tail; // append after the tail
    tail->next = mp3; // previous tail points to this
    tail = mp3;       // new item is the new tail
  }
}
