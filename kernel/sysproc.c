#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
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

  backtrace();

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

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
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

int 
sys_sigalarm(void)
{
  int gap;
  uint64 handler;
  struct proc *p =myproc();

  if(argint(0, &gap) < 0)
    return -1;
  if(argaddr(1, &handler))
    return -1;
  p->gap = gap;
  p->handler = (void (*)())handler;
  p->passed_time = 0;
  return 0;
}

int 
sys_sigreturn(void)
{
  struct proc* p = myproc();
  p->trapframe->epc = p->epc;
  p->trapframe->ra = p->ra;
  p->trapframe->sp = p->sp;
  p->trapframe->gp = p->gp;
  p->trapframe->tp = p->tp;
  p->trapframe->t0 = p->t0;
  p->trapframe->t1 = p->t1;
  p->trapframe->t2 = p->t2;
  p->trapframe->s0 = p->s0;
  p->trapframe->s1 = p->s1;
  p->trapframe->a0 = p->a0;
  p->trapframe->a1 = p->a1;
  p->trapframe->a2 = p->a2;
  p->trapframe->a3 = p->a3;
  p->trapframe->a4 = p->a4;
  p->trapframe->a5 = p->a5;
  p->trapframe->a6 = p->a6;
  p->trapframe->a7 = p->a7;
  p->trapframe->s2 = p->s2;
  p->trapframe->s3 = p->s3;
  p->trapframe->s4 = p->s4;
  p->trapframe->s5 = p->s5;
  p->trapframe->s6 = p->s6;
  p->trapframe->s7 = p->s7;
  p->trapframe->s8 = p->s8;
  p->trapframe->s9 = p->s9;
  p->trapframe->s10 = p->s10;
  p->trapframe->s11 = p->s11;
  p->trapframe->t3 = p->t3;
  p->trapframe->t4 = p->t4;
  p->trapframe->t5 = p->t5;
  p->trapframe->t6 = p->t6;

  //we finish handler now
  p->if_handler = 0;
  return 0;
}
