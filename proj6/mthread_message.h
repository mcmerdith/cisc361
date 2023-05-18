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

void mbox_create(mbox **mb);
void mbox_destroy(mbox **mb);
void mbox_deposit(mbox *mb, char *message, int len);
void mbox_withdraw(mbox *mb, char *message, int *len);