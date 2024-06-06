#include "types.h"
#include "user.h"

int
main(int argc, char *argv[])
{
  printf(1, "%d\n", pgcnt());

  sbrk(4096);
  printf(1, "%d\n", pgcnt());

  malloc(1);
  printf(1, "%d\n", pgcnt());

  exit();
}