/*
 * contains the implementation of all syscalls.
 */

#include <stdint.h>
#include <errno.h>

#include "util/types.h"
#include "syscall.h"
#include "string.h"
#include "process.h"
#include "util/functions.h"
#include "pmm.h"
#include "vmm.h"
#include "spike_interface/spike_utils.h"

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
  // buf is now an address in user space of the given app's user stack,
  // so we have to transfer it into phisical address (kernel is running in direct mapping).
  assert( current );
  char* pa = (char*)user_va_to_pa((pagetable_t)(current->pagetable), (void*)buf);
  sprint(pa);
  return 0;
}

//
// implement the SYS_user_exit syscall
//
ssize_t sys_user_exit(uint64 code) {
  sprint("User exit with code:%d.\n", code);
  // in lab1, PKE considers only one app (one process). 
  // therefore, shutdown the system when the app calls exit()
  shutdown(code);
}

//
// maybe, the simplest implementation of malloc in the world ... added @lab2_2
//
uint64 sys_user_allocate_page(ssize_t size) {
  // sprint("alloc %llx\n",size);
  void *pre,*nxt,*cursize;
  for(void *now=current->head;now;now=*(void**)nxt)
  {
    pre=user_va_to_pa(current->pagetable,(void**)now-3);
    nxt=user_va_to_pa(current->pagetable,(void**)now-2);
    cursize=user_va_to_pa(current->pagetable,(void**)now-1);
    // sprint("pre:%llx nxt:%llx cursize %llx now %llx ufree_addr %llx\n",(void**)now-3,(void**)now-2,(void**)now-1,now,current->ufree_addr);
    // sprint("pre:%llx nxt:%llx cursize %llx now %llx ufree_addr %llx\n",pre,nxt,cursize,now,current->ufree_addr);
    if(-(*(ssize_t*)cursize)>=size)
    {
      *(ssize_t*)cursize=-*(ssize_t*)cursize;
      return (uint64)now;
    }
    // sprint("not this!\n");
  }
  pre=current->ufree_addr;
  current->ufree_addr+=8;
  nxt=current->ufree_addr;
  current->ufree_addr+=8;
  cursize=current->ufree_addr;
  current->ufree_addr+=8;
  void *ret=current->ufree_addr;
  current->ufree_addr=(void*)((void**)current->ufree_addr+size/8+1);
  // sprint("pre:%llx nxt:%llx cursize %llx ret %llx ufree_addr %llx\n",pre,nxt,cursize,ret,current->ufree_addr);
  while(current->ufree_addr > current->ufree_page)
  {
    void* pa = alloc_page();
    uint64 va = (uint64) (current->ufree_page);
    current->ufree_page += PGSIZE;
    user_vm_map((pagetable_t)current->pagetable, va, PGSIZE, (uint64)pa,
          prot_to_type(PROT_WRITE | PROT_READ, 1));
  }
  // sprint("mapped!\n");
  pre=user_va_to_pa(current->pagetable,pre);
  nxt=user_va_to_pa(current->pagetable,nxt);
  cursize=user_va_to_pa(current->pagetable,cursize);
  if(current->head==NULL)
  {
    current->head=current->tail=ret;
    *(void**)pre=*(void**)nxt=NULL;
  }
  else
  {
    *(void**)pre=current->tail;
    *(void**)nxt=NULL;
    *(void**)user_va_to_pa(current->pagetable,(void**)current->tail-2)=ret;
    current->tail=ret;
  }
  *(ssize_t*)cursize=size;
  // sprint("alloced!\n");
  return (uint64)ret;
}

//
// reclaim a page, indicated by "va". added @lab2_2
//
uint64 sys_user_free_page(uint64 va) 
{
  // sprint("free %llx\n",va);
  void *pre,*nxt,*cursize;
  pre=user_va_to_pa(current->pagetable,(void**)va-3);
  nxt=user_va_to_pa(current->pagetable,(void**)va-2);
  cursize=user_va_to_pa(current->pagetable,(void**)va-1);
  *(ssize_t*)cursize=-*(ssize_t*)cursize;
  // uint64 begpa=lookup_pa(current->pagetable,(uint64)((void*)va-3));
  // uint64 endpa=lookup_pa(current->pagetable,(uint64)va+*(ssize_t*)cursize);
  // user_vm_unmap((pagetable_t)current->pagetable, va, PGSIZE, 1);
  return 0;
}

//
// [a0]: the syscall number; [a1] ... [a7]: arguments to the syscalls.
// returns the code of success, (e.g., 0 means success, fail for otherwise)
//
long do_syscall(long a0, long a1, long a2, long a3, long a4, long a5, long a6, long a7) {
  // sprint("doing syscall! a:%lld %lld %lld %lld %lld %lld %lld %lld\n",a0,a1,a2,a3,a4,a5,a6,a7);
  switch (a0) {
    case SYS_user_print:
      return sys_user_print((const char*)a1, a2);
    case SYS_user_exit:
      return sys_user_exit(a1);
    // added @lab2_2
    case SYS_user_allocate_page:
      return sys_user_allocate_page(a1);
    case SYS_user_free_page:
      return sys_user_free_page(a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}
