#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void) {
  int active, total_syscalls, free_pages;
  int pid = fork();

  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }

  if(pid == 0){
    // Child process: sleep for a while to remain in the process table.
    sleep(100);
    exit(0);
  }
  
  // Parent waits a bit to ensure child is still active.
  sleep(50);
  active = sysinfo(0);
  total_syscalls = sysinfo(1);
  free_pages = sysinfo(2);

  printf("Active Processes (should be >= 2): %d\n", active);
  printf("Syscalls so far (excluding sys_sysinfo call): %d\n", total_syscalls);
  printf("Free Memory Pages: %d\n", free_pages);

  // Wait for the child so it can clean up.
  wait((void*)0);
  exit(0);
}
