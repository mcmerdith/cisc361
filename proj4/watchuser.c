#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include "watchuser.h"
#include "bgjob.h"

extern pthread_t watchuser_id;
extern int b_watchuser_running;

pthread_mutex_t watchuser_mutex;
watched_user *watchuser_head;

void watch_user(char *username)
{
    watched_user *new_user = malloc(sizeof(watched_user));
    strncpy(new_user->username, username, __UT_NAMESIZE);
    memset(new_user->tty, 0, __UT_LINESIZE);
    memset(new_user->host, 0, __UT_HOSTSIZE);
    new_user->next_node = new_user->prev_node = NULL;
    new_user->b_logged_on = 0;

    pthread_mutex_lock(&watchuser_mutex);

    watched_user **current = &watchuser_head;
    while (1)
    {
        if (*current && strncmp((*current)->username, username, __UT_NAMESIZE) == 0)
        {
            printf("User '%.*s' was already on the watchlist\n", __UT_NAMESIZE, username);
            break;
        }

        if (*current != NULL && (*current)->next_node == NULL) // try and find an element with a next_node
        {
            (*current)->next_node = new_user;
            new_user->prev_node = *current;
            break;
        }
        else if (*current == NULL)
        {
            *current = new_user;
            break;
        }

        current = &(*current)->next_node;
    }

    pthread_mutex_unlock(&watchuser_mutex);

    // start the thread if its not running

    if (!b_watchuser_running)
    {
        run_thread(&watchuser_id, &thread_watchuser);
        b_watchuser_running = 1;
    }
}

void stop_watch_user(char *username)
{
    pthread_mutex_lock(&watchuser_mutex);

    watched_user **current = &watchuser_head, *temp;

    while (*current != NULL)
    {
        if (*current != NULL && strcmp((*current)->username, username) == 0) // try and find an element with the correct username
        {
            temp = *current;

            if (temp->prev_node)
                temp->prev_node->next_node = temp->next_node; // remove this element from the list

            if (temp->next_node)
                temp->next_node->prev_node = temp->prev_node;

            *current = temp->next_node;

            free(temp);
        }
        else
        {
            current = &(*current)->next_node;
        }
    }

    pthread_mutex_unlock(&watchuser_mutex);
}

void *thread_watchuser(void *arg)
{
    while (1)
    {
        pthread_mutex_lock(&watchuser_mutex);

        login_user *login_user_head, **curr = &login_user_head;

        struct utmpx *up;

        setutxent();               /* start at beginning */
        while ((up = getutxent())) /* get an entry */
        {
            if (up->ut_type == USER_PROCESS) /* only care about users */
            {
                *curr = malloc(sizeof(login_user));
                strncpy((*curr)->username, up->ut_user, __UT_NAMESIZE);

                for (watched_user *current = watchuser_head; current != NULL; current = current->next_node)
                {
                    if (current->b_logged_on)
                        continue;
                    if (strncmp((*curr)->username, current->username, __UT_NAMESIZE) != 0)
                        continue;

                    current->b_logged_on = 1;

                    strncpy(current->tty, up->ut_line, __UT_LINESIZE);
                    strncpy(current->host, up->ut_host, __UT_HOSTSIZE);

                    printf("%.*s has logged on %.*s from %.*s\n", __UT_NAMESIZE, current->username, __UT_LINESIZE, current->tty, __UT_HOSTSIZE, current->host);
                    break;
                }

                curr = &(*curr)->next_node;
            }
        }
        endutxent();

        *curr = NULL;

        int b_logged_out;
        for (watched_user *current = watchuser_head; current != NULL; current = current->next_node)
        {
            if (!current->b_logged_on)
                continue;

            b_logged_out = 1;

            for (login_user *current_user = login_user_head; current_user != NULL; current_user = current_user->next_node)
            {
                if (strncmp(current_user->username, current->username, __UT_NAMESIZE) == 0)
                    b_logged_out = 0;
            }

            if (b_logged_out)
            {
                printf("%.*s has logged off %.*s from %.*s\n", __UT_NAMESIZE, current->username, __UT_LINESIZE, current->tty, __UT_HOSTSIZE, current->host);
                current->b_logged_on = 0;
            }
        }

        pthread_mutex_unlock(&watchuser_mutex);

        sleep(5);
    }
}