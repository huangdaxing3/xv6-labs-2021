#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
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


#ifdef LAB_PGTBL
int
sys_pgaccess(void)
{
  // lab pgtbl: your code here.  
  uint64 addr;
  uint64 mask;  
  int len;
  pagetable_t pagetable; 
  struct proc *p = myproc(); 
  int procmask = 0;
  pte_t *pte;

  if(argaddr(0, &addr) < 0 || argint(1, &len) < 0 || argaddr(2, &mask) < 0)
    return -1;

  for(int i=0; i<len; i++, addr += PGSIZE) {
    pagetable = p->pagetable;
 
    for(int level = 2; level > 0; level--) {
      pte = &pagetable[PX(level, addr)];
      if(*pte & PTE_V) {
        pagetable = (pagetable_t)PTE2PA(*pte);
      } else {
        return -1;
      }      
    }
    pte = &pagetable[PX(0, addr)];
    if(*pte & PTE_A) {  
      procmask = procmask | (1L << i);
      *pte = *pte & (~PTE_A);
    }
    
  }
 
  pagetable = p->pagetable;
  copyout(pagetable, mask, (char *)&procmask, sizeof(procmask));
  return 0;
}
#endif

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
