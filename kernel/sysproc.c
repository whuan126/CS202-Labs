#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "syscall.h"

uint sys_call_count = 0;

struct run {
  struct run *next;
};

// External declaration for the mem_alloc data
extern struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

uint64
sys_sysinfo(void) {
  int param;
  if(argint(0, &param) < 0)
    return -1;

  uint ret = -1;
  struct proc *p;
  
  switch(param) {
  case 0: {
      // Count active processes
      int active = 0;
      for(p = proc; p < &proc[NPROC]; p++){
          acquire(&p->lock);
          if(p->state != UNUSED)
              active++;
          release(&p->lock);
      }
      ret = active;
      break;
  }
  case 1: {
      //  System call count excluding the current sys_sysinfo call
      ret = sys_call_count - 1;
      break;
  }
  case 2: {
      // Count free memory pages
      int free_pages = 0;
      struct run *r;
      acquire(&kmem.lock);
      for(r = kmem.freelist; r; r = r->next)
          free_pages++;
      release(&kmem.lock);
      ret = free_pages;
      break;
  }
  default:
      ret = -1;
      break;
  }
  return ret;
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
// Lab 1 procinfo
uint64
sys_procinfo(void)
{
    uint64 addr;
    argaddr(0, &addr);            // just fetch the user pointer

    struct proc *p = myproc();
    struct pinfo info;
    info.ppid          = p->parent ? p->parent->pid : -1;
    info.syscall_count = p->syscall_count;
    info.page_usage    = (p->sz + PGSIZE - 1) / PGSIZE;

    // copyout returns < 0 if the user address is invalid
    if (copyout(p->pagetable, addr, (char *)&info, sizeof(info)) < 0)
        return -1;

    return 0;
}
