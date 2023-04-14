#include <stdio.h>
#include <ucontext.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    ucontext_t ctx;
    int i, j;

    j = 17;

    getcontext(&ctx); // save PC and other CPU registers into
    j++;
    printf("Hello world %d\n", j);
    sleep(1);
    setcontext(&ctx); // restore PC and other CPU registers

    return 0;
}