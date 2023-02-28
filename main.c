//
// adapted from http://www.cprogramming.com/snippets/source-code/singly-linked-list-insert-remove-add-count
//

#include "mp3.h"
#define  BUFFERSIZE 128

node_t *head;

void insert(char *name, int data);
void print();
void freeList();
void delete(int data);

int main()
{
  int i, num, len;
  struct node *n;
  char buffer[BUFFERSIZE], c;

  head = NULL;

  while (1) {
    printf("\nList Operations\n");
    printf("===============\n");
    printf("(1) Insert\n");
    printf("(2) Display\n");
    printf("(3) Delete\n");
    printf("(4) Exit\n");
    printf("Enter your choice : ");
    if (scanf("%d%c", &i, &c) <= 0) {          // use c to capture \n
      printf("Enter only an integer...\n");
      exit(0);
    } else {
        switch(i)
        {
        case 1: printf("Enter the name to insert : ");
		if (fgets(buffer, BUFFERSIZE , stdin) != NULL) {
                  len = strlen(buffer);
                  buffer[len - 1] = '\0';   // override \n to become \0
                } else {
                    printf("wrong name...");
                    exit(-1);
                  }
                printf("Enter the number to insert : ");
                scanf("%d%c", &num, &c);  // use c to capture \n
                printf("[%s] [%d]\n", buffer, num);
                insert(buffer, num);
                break;
        case 2: if (head == NULL)
                  printf("List is Empty\n");
                else
                  print();
                break;
	case 3: printf("Enter the data to delete : ");
		scanf("%d%c", &num, &c);
		delete(num);
                break;
        case 4: freeList();
                return 0;
        default: printf("Invalid option\n");
        }
    }
  }

  return 0;
}
