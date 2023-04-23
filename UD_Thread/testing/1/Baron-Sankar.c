/* Lauren Baron and Swetha Sankar
 * CISC361-010
 * Programming Assignment 6: User-Level Thread Library
 * Phase 1
 * Extra Credit: Write our own challenging test case
 * NOTE: takes user input in order to know how many threads to create
 * to get the output we got in our ECOutput.txt file, input 6
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ud_thread.h"

int threadNum;
#define BUFFER_SIZE 256

void my_thread(int id)
{
	for (int i = 0; i < threadNum; i++)
	{
		printf("Thread %d yielding\n", id);
		t_yield();
	}
	t_terminate();
	printf("Thread %d terminated\n", id);
}

int main(void)
{
	printf("\n\n");
	// ask user to input how many threads they want to create
	char tempNum[BUFFER_SIZE];
	printf("Enter number of threads: ");
	fgets(tempNum, BUFFER_SIZE, stdin);

	printf("\n\n");
	// init
	threadNum = atoi(tempNum);
	printf("Initializing user-level thread library\n");
	t_init();
	printf("User-level thread library initialized\n");

	printf("\n\n"); // spacer

	// create
	printf("Creating %d threads\n", threadNum);
	for (int i = 0; i < threadNum + 1; i++)
	{
		t_create(my_thread, i, 1);
	}
	printf("Created %d threads\n", threadNum);

	printf("\n\n"); // spacer

	// yield
	for (int i = 0; i < threadNum; i++)
	{
		printf("Thread %d yielding\n", i + 1);
		t_yield();
		printf("\n");
	}

	// terminate by calling yield one more time
	printf("\n\n");
	printf("Terminating threads...\n");
	t_yield();

	// shut down
	printf("\n\n");
	printf("Shutting down user-level thread library\n");
	t_shutdown();
	printf("User-level thread library shut down\n");

	printf("\n\n");
	return 0;
}
