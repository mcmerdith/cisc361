#include "mp3.h"

extern mp3_t *head;
extern mp3_t *tail;

// print the doubly linked list defined by globals head and tail. 
// if (bBackward) tail -> head else head -> tail
void print(int bBackward)
{
  // Index variables
  mp3_t *temp;
  int i = 0;

  // Set the start point
  if (bBackward)
  {
    temp = tail;
  }
  else
  {
    temp = head;
  }

  if (temp == NULL)
  {
    printf("No tracks...");
    return;
  }

  while (temp != NULL)
  {
    printf("(%d)--%s:--%s--(%d seconds)\n", ++i,
           temp->artist,
           temp->title,
           temp->runtime);

    // Move to the next node
    if (bBackward)
    {
      temp = temp->prev;
    }
    else
    {
      temp = temp->next;
    }
  }
}
