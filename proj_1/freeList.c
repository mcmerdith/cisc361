#include "mp3.h"

extern mp3_t *head;

void freeElem(mp3_t *elem)
{
  free(elem->artist);
  free(elem->title);
  free(elem);
}

void freeList()
{
  mp3_t *temp;
  int i = 0;

  while (head != NULL)
  {
    temp = head;
    head = head->next; // point to next MP3 record
    freeElem(temp);    // then free MP3 record
    i++;
  }
  printf("free %d MP3 records...\n", i);
}
