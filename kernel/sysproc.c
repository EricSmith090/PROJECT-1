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
  if(n < 0)
    n = 0;
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

uint64
sys_procinfo(void)
{
  int pid;
  uint64 addr; // Địa chỉ vùng nhớ của struct procinfo ở phía user
  struct proc *p;
  struct procinfo info;

  // Lấy 2 tham số truyền vào từ user: pid (int) và addr (pointer)
  argint(0, &pid);
  argaddr(1, &addr);

  // Duyệt qua bảng danh sách tiến trình của kernel để tìm pid khớp
  extern struct proc proc[]; 
  int found = 0;
  for(p = proc; p < &proc[NPROC]; p++) {
    acquire(&p->lock);
    if(p->pid == pid) {
      // Sao chép thông tin từ struct proc của kernel sang struct procinfo
      info.pid = p->pid;
      info.ppid = p->parent ? p->parent->pid : 0;
      info.state = p->state;
      info.sz = p->sz;
      safestrcpy(info.name, p->name, sizeof(p->name));
      
      release(&p->lock);
      found = 1;
      break;
    }
    release(&p->lock);
  }

  if(!found) return -1; // Không tìm thấy pid

  // Sao chép dữ liệu từ vùng nhớ Kernel về vùng nhớ User (địa chỉ addr)
  struct proc *curproc = myproc();
  if(copyout(curproc->pagetable, addr, (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}
