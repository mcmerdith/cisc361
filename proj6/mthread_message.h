typedef struct message_node
{
    char *message;
    int len,
        sender,
        receiver;
    struct message_node *next;
} message_node;

typedef struct mbox
{
    struct message_node *message_queue;
    struct sem_t *mbox_sem;
} mbox;