#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"


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
uint64 sys_hello(void)
{
  int n;
  argint(0, &n);
  print_hello(n);
  return 0;
}
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
uint64
sys_sched_tickets(void)
{
  int n;
  argint(0,&n);         //n is the inout parameter from user level, here it is number of tickets for that process
  return set_tickets_to_proc(n);
}

//syscall sched_statistics to print pid, name, tickets and ticks of the process
//this function returns int 0
uint64
sys_sched_statistics(void)
{
  print_sched_statistics();
  return 0;
}

