#include <stdio.h>
#include <unistd.h>
#include <utmpx.h>
#include <stdio.h>

void printing()
{
    printf("this is stdout\n");
    fprintf(stderr, "this is stderr\n");
}

void sleeping()
{
    for (int i = 0; i < 15; i++)
    {
        printf("[%d] sleeping\n", i);
        sleep(1);
    }
}

void utmpx()
{
    struct utmpx *up;

    setutxent();             /* start at beginning */
    while (up = getutxent()) /* get an entry */
    {
        printf("hello\n");
        if (up->ut_type == USER_PROCESS) /* only care about users */
        {
            printf("%s has logged on %s from %s\n", up->ut_user, up->ut_line, up->ut_host);
        }
    }
}

int main()
{
    utmpx();
    return 0;
}