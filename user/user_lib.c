/*
 * The supporting library for applications.
 * Actually, supporting routines for applications are catalogued as the user 
 * library. we don't do that in PKE to make the relationship between application 
 * and user library more straightforward.
 */

#include "user_lib.h"
#include "util/types.h"
#include "util/snprintf.h"
#include "kernel/syscall.h"



// #include "spike_interface/spike_utils.h"
// #include "spike_interface/spike_utils.c"

uint64 do_user_call(uint64 sysnum, uint64 a1, uint64 a2, uint64 a3, uint64 a4, uint64 a5, uint64 a6,
                 uint64 a7) {
  // sprint("do syscall! a:%lld %lld %lld %lld %lld %lld %lld %lld\n",sysnum,a1,a2,a3,a4,a5,a6,a7);
  
  uint64 ret;

  // before invoking the syscall, arguments of do_user_call are already loaded into the argument
  // registers (a0-a7) of our (emulated) risc-v machine.
  asm volatile(
  	  "ld a0, %1\n\t"
  	  "ld a1, %2\n\t"
  	  "ld a2, %3\n\t"
  	  "ld a3, %4\n\t"
  	  "ld a4, %5\n\t"
  	  "ld a5, %6\n\t"
  	  "ld a6, %7\n\t"
  	  "ld a7, %8\n\t"
      "ecall\n"
      "sd a0, %0"  // returns a 32-bit value
      
      : "=m"(ret)
      :	"m"(sysnum),"m"(a1),"m"(a2),"m"(a3),"m"(a4),"m"(a5),"m"(a6),"m"(a7)
      : "memory");

  return ret;
}

//
// printu() supports user/lab1_1_helloworld.c
//
int printu(const char* s, ...) {
  va_list vl;
  va_start(vl, s);

  char out[256];  // fixed buffer size.
  int res = vsnprintf(out, sizeof(out), s, vl);
  va_end(vl);
  const char* buf = out;
  size_t n = res < sizeof(out) ? res : sizeof(out);

  // make a syscall to implement the required functionality.
  return do_user_call(SYS_user_print, (uint64)buf, n, 0, 0, 0, 0, 0);
}

//
// applications need to call exit to quit execution.
//
int exit(int code) {
  return do_user_call(SYS_user_exit, code, 0, 0, 0, 0, 0, 0); 
}

//
// lib call to naive_malloc
//
void* naive_malloc() {
  return (void*)do_user_call(SYS_user_allocate_page, 0, 0, 0, 0, 0, 0, 0);
}

//
// lib call to naive_free
//
void naive_free(void* va) {
  do_user_call(SYS_user_free_page, (uint64)va, 0, 0, 0, 0, 0, 0);
}
