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
#include "kernel/elf.h"

#include "spike_interface/spike_utils.h"

//
// implement the SYS_user_print syscall
//
ssize_t sys_user_print(const char* buf, size_t n) {
  sprint(buf);
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
typedef union {
  uint64 buf[MAX_CMDLINE_ARGS];
  char *argv[MAX_CMDLINE_ARGS];
} arg_buf;
ssize_t sys_user_print_function_name(void* addr)
{
    int sym_count = 0;
    char* symname = NULL;
    int i;
    // section_header tmp;
    // sprint("section_table:%llx,ehdr:%llx,shnum:%llx,strtab:%llx %s\n",section_table,&elfloader.ehdr,&elfloader.ehdr.shnum,strtab,strtab);

    for (i = 0; i < elfloader.ehdr.shnum; i++)
    {
      
      symname = strtab + section_table[i].name;
      // sprint("i:%d name:%s %llx %llx %llx %llx %llx\n",i,symname,section_table[i].type,section_table[i].addr,section_table[i].name,section_table[i].offset,section_table[i].size);
        if (section_table[i].type == 2) {
            // symtab = (symbol_table*)((char*)&ehdr + shdr[i].offset);
            sym_count = section_table[i].size / sizeof(symbol_table);
            break;
        }
    }
    // sprint("sym_count:%d\n",sym_count);
    uint64 funcaddr=0;
    char* funcname=NULL;
    for (i = 0; i < sym_count; i++) {
        symname = strtab + symtab[i].name;
        // sprint("%llx,%llx,%s\n",(void*)symtab[i].value,symtab[i].name,symname);
        if ((void*)symtab[i].value <= addr)
        {
          if(symtab[i].value>funcaddr)
          {
            funcaddr=symtab[i].value;
            funcname=symname;
          }
        }
    }
    sprint("%s\n",funcname);
    static const char str_main[10]="main";
    int res=strcmp(funcname,str_main);
    // sprint("res:%d\n",res);
    return res;
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
    case SYS_user_print_function_name:
      return sys_user_print_function_name((void*)a1);
    default:
      panic("Unknown syscall %ld \n", a0);
  }
}
