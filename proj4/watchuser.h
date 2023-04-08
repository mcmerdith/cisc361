#pragma once

#include <utmpx.h>

typedef struct watched_user
{
    char username[__UT_NAMESIZE],               // username to watch
        tty[__UT_LINESIZE],                     // the tty the user is logged in on
        host[__UT_HOSTSIZE];                    // the host the user is logged in on
    int b_logged_on;                            // if the user is logged on
    struct watched_user *next_node, *prev_node; // linked-list stuff
} watched_user;

typedef struct login_user
{
    char username[__UT_NAMESIZE]; // the logged in username
    struct login_user *next_node; // linked-list stuff
} login_user;

// add a username to the watchlist
void watch_user(char *username);

// remove a username from the watchlist
void stop_watch_user(char *username);