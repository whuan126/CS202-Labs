#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main() {
  struct pinfo info;

  if (procinfo(&info) == 0) {
    printf("Parent PID: %d\n", info.ppid);
    printf("Syscall Count: %d\n", info.syscall_count);
    printf("Page Usage: %d\n", info.page_usage);
  } else {
    printf("procinfo syscall failed\n");
  }

  exit(0);
}
