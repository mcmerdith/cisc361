#pragma once

typedef struct sem_t
{
    int count;
    struct tcb *queue;
} sem_t;