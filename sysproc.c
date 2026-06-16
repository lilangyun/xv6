#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;
  struct proc *p = myproc();

  if(argint(0, &n) < 0)
    return -1;
  addr = p->sz;
  if(n > 0){
    p->sz += n;
  }
  else if(n < 0){
    // Handle negative sbrk() arguments.
    p->sz = deallocuvm(p->pgdir, p->sz, p->sz + n);
  }
  else{
    return -1;
  }

  // if(growproc(n) < 0)
  //   return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int 
sys_date(void)
{
  struct rtcdate *r;
  if(argptr(0,(void *)&r, sizeof(*r))){
    return -1;
  }
  cmostime(r);
  return 0;
}

int
sys_alarm(void)
{
  int ticks;
  void (*handler)();  // function pointer

  if(argint(0, &ticks) < 0)
    return -1;
  if(argptr(1, (char**)&handler, 1) < 0)
    return -1;
  myproc()->alarmticks = ticks;
  myproc()->alarmhandler = handler;
  myproc()->alarmcount = 0;
  myproc()->in_alarm = 0;
  return 0;
}

int
sys_sigreturn(void)
{
  struct proc *p = myproc();
  p->in_alarm = 0;
  return 0;
}