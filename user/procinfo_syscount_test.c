// user/procinfo_delta_test.c
#include "kernel/types.h"
#include "user/user.h"

int
main(void)
{
  struct pinfo before, after;

  // get a baseline
  procinfo(&before);

  // explicit calls we want to measure
  getpid();
  write(1, "hi\n", 3);
  open("README", 0);
  close(3);

  // now measure again
  procinfo(&after);

  // the difference should be exactly 4
  int delta = after.syscall_count - before.syscall_count;
  printf("Delta syscalls: %d (should be 4)\n", delta);

  exit(0);
}
