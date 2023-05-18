#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mthread_message.h"
#include "ud_thread.h"

message_node *_new_message(char *message, int len)
{
    message_node *msg = malloc(sizeof(message_node));

    msg->message = calloc(strlen(message) + 1, sizeof(char));
    strcpy(msg->message, message);
    msg->len = len;
    msg->sender = 0;
    msg->receiver = 0;
    msg->next = NULL;

    return msg;
}

void _free_message(message_node *message)
{
    if (message->message)
        free(message->message);
    free(message);
}

void _free_message_queue(message_node *message)
{
    message_node *curr = message, *temp;
    while (curr != NULL)
    {
        temp = curr->next;

        _free_message(curr);

        curr = temp;
    }
}

void mbox_create(mbox **mb)
{
    *mb = malloc(sizeof(mbox));    // Allocate the mailbox
    (*mb)->message_queue = NULL;   // Initially empty queue
    sem_init(&(*mb)->mbox_sem, 1); // Initially ready semaphore
}

void mbox_destroy(mbox **mb)
{

    _free_message_queue((*mb)->message_queue); // Cleanup the queue
    sem_destroy(&(*mb)->mbox_sem);             // Cleanup the semaphore
}

void mbox_deposit(mbox *mb, char *message, int len)
{
    // Secure a lock on the box
    sem_wait(mb->mbox_sem);

    // Get the queue
    message_node **target = &mb->message_queue;

    // Move through the queue until we find an empty slot
    while (*target != NULL)
        target = &(*target)->next;

    // Add the new message
    target = _new_message(message, len);

    // Release our lock
    sem_signal(mb->mbox_sem);
}

void mbox_withdraw(mbox *mb, char *message, int *len)
{
    // Secure a lock on the box
    sem_wait(mb->mbox_sem);

    // Get the queue
    message_node *queue = mb->message_queue;

    if (queue == NULL)
    {
        // If no queue return nothing
        *message = NULL;
        *len = 0;
    }
    else
    {
        // Copy the message
        strcpy(message, queue->message);
        // Set the length
        *len = queue->len;
        // Remove the message from the queue
        mb->message_queue = queue->next;
        // Free the message
        _free_message(queue);
    }

    // Release our lock
    sem_signal(mb->mbox_sem);
}