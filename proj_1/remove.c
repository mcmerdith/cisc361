#include "mp3.h"

extern mp3_t *head;
extern mp3_t *tail;

void freeElem(mp3_t *elem);

// Remove all tracks by an artist in the doubly linked list defined by the globals head and tail
void removeMp3(char *artist)
{
  mp3_t *temp, *curr;

  curr = head;

  while (curr != NULL)
  {
    if (curr->artist && strcmp(curr->artist, artist) == 0)
    {
      // We essentially stich the nodes neighbors next & prev pointers together
      // to prepare for the deletion of this node

      if (curr->prev)
      {
        // Set the previous node's next pointer to this node's next pointer
        curr->prev->next = curr->next;
      }
      else
      {
        // If no previous node, the next node is now head
        head = curr->next;
      }

      if (curr->next)
      {
        // Set the next nodes previous pointer to this node's previous pointer
        curr->next->prev = curr->prev;
      }
      else
      {
        // If no next node, the previous node is now tail
        tail = curr->prev;
      }

      temp = curr;       // We will be deleting temp after moving the current pointer
      curr = curr->next; // Move to the next node
      freeElem(temp);    // Free the previous node (delete it)
      continue;
    }

    // Don't delete it
    curr = curr->next;
  }
}
