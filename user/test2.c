// test2.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define PGSIZE 4096

int main(void) {
  int active, total_syscalls, free_pages;
  int pid;
  char *old_brk;

  // --- Initial sysinfo call ---
  active = sysinfo(0);
  total_syscalls = sysinfo(1);
  free_pages = sysinfo(2);
  printf("Initial sysinfo values:\n");
  printf("  Active Processes: %d\n", active);
  printf("  Syscalls (excluding current): %d\n", total_syscalls);
  printf("  Free Memory Pages: %d\n", free_pages);

  // --- Demonstrate sys_call_count increment (param == 1) ---
  // Calling sysinfo(1) twice in a row. Each call is a syscall, so the
  // returned value should increment.
  int sc1 = sysinfo(1);
  int sc2 = sysinfo(1);
  printf("Sysinfo(1) call results (consecutive calls): %d and %d\n", sc1, sc2);

  // --- Demonstrate memory allocation (param == 2) ---
  // Allocate one page via sbrk to force a change in the free pages count.
  old_brk = sbrk(PGSIZE);
  if(old_brk == (char*)-1) {
    printf("sbrk() failed\n");
  } else {
    printf("Allocated one page using sbrk.\n");
  }
  free_pages = sysinfo(2);
  printf("Free Memory Pages after sbrk allocation: %d\n", free_pages);

  // --- Demonstrate active process count (param == 0) ---
  // Fork a child process so that the active process count increases.
  pid = fork();
  if(pid < 0){
    printf("fork failed\n");
    exit(1);
  }
  if(pid == 0){
    // In the child process, sleep for a while so that it remains active,
    // then exit.
    sleep(100);
    exit(0);
  }
  // In the parent, wait a bit to ensure the child is active.
  sleep(50);
  active = sysinfo(0);
  printf("Active Processes (should be at least 2): %d\n", active);

  // Clean up the child process.
  wait((void*)0);
  exit(0);
}
