#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char **argv)
{
  printf("hello returns [%d]\n", hello());
  exit(0);
}
