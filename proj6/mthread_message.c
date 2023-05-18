#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mthread_lib.h"
#include "ud_thread.h"

// The current running thread
extern tcb *running_queue;

// The global message queue
message_node *message_queue = NULL;
sem_t *message_lock, *message_queue_sem;

message_node *_new_message(char *message, int len)
{
    message_node *msg = malloc(sizeof(message_node));

    msg->message = calloc(strlen(message) + 1, sizeof(char));
    strcpy(msg->message, message);
    msg->len = len;
    msg->sender = running_queue->id;
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
    *target = _new_message(message, len);

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
        *message = 0;
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

void _message_init()
{
    if (message_lock == NULL)
        sem_init(&message_lock, 1);

    if (message_queue_sem == NULL)
        sem_init(&message_queue_sem, 0);
}

void send(int tid, char *message, int len)
{
    _message_init();

    // Obtain a lock on the queue
    sem_wait(message_lock);

    // Get the queue
    message_node **target = &message_queue;

    // Move through the queue until we find an empty slot
    while (*target != NULL)
        target = &(*target)->next;

    // Add the new message
    *target = _new_message(message, len);
    (*target)->receiver = tid;

    // Release our lock
    sem_signal(message_lock);

    // Notify the other threads that a message is ready
    sem_signal(message_queue_sem);
}

void receive(int *tid, char *message, int *len)
{
    _message_init();

    // Wait for a message to be ready
    sem_wait(message_queue_sem);

read_queue:

    // Lock the queue
    sem_wait(message_lock);

    message_node **current = &message_queue;

    // Find a suitable message
    while (*current != NULL)
    {
        // If the caller wants any message
        if (*tid == 0)
            break;

        // If this message is from the target sender
        if (*tid == (*current)->sender)
            break;
    }

    if (*current == NULL)
    {
        sem_signal(message_lock);      // give up the queue
        sem_signal(message_queue_sem); // We don't want the message so give it away
        sem_wait(message_queue_sem);   // wait until we have a new message
        goto read_queue;               // When a new message is ready, reread the queue
    }

    strcpy(message, (*current)->message);
    *len = (*current)->len;
    *tid = (*current)->sender;

    message_node *temp = *current;
    *current = (*current)->next;
    _free_message(temp);

    sem_signal(message_lock);
}