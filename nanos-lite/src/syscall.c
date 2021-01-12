#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "common.h"
#include "proc.h"

extern int _write(int fd, void *buf, size_t count);
static int program_break;

static inline int do_open(const char * path, int flags, int mode) {
  return fs_open(path, flags, mode);
}

static inline int do_close(int fd) {
  return fs_close(fd);
}

static inline int do_read(int fd, void * buf, size_t count) {
  if (fd >= 0 && fd <= 2) {
    return 0;
  }
  return fs_read(fd, buf, count);
}

static inline int do_write(int fd, const void * buf, size_t count) {
  return fs_write(fd, buf, count);
}

static inline size_t do_lseek(int fd, size_t offset, int whence) {
  return fs_lseek(fd, offset, whence);
}

int do_brk(int addr) {
  program_break = addr;
  return 0;
}

_Context* do_syscall(_Context *c) {
  // Log("进入do_syscall%d", c->GPR1);
  intptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  // 针对不同的系统调用号来进行分发
  // GPR1 调用类型
  // GPR2 - 4 arg0 - arg3
  // GPRx 返回值
  switch (a[0]) {
    case SYS_exit: _halt(0); c->GPRx = 0; break;
    case SYS_yield:  _yield();c->GPRx = 0; break;
    case SYS_open: 
      c->GPRx = fs_open((const char *)(a[1]), a[2], a[3]);
      break;
    case SYS_read: 
      c->GPRx = do_read(a[1], (void*)(a[2]), a[3]);
      break;
    case SYS_write: 
      /**
       * a[1] int fd
       * a[2] void * buf 
       * a[3] size_t count
       */
      Log("进入write");
      // a[1] = c->GPR2;

      // Log("fd = %d", *(int *)(a[1]));
      // Log("尬住了");
      // if (a[1] == 1 || a[1] == 2) {
      //   for (int i = 0; i < a[3]; i++) {
      //     _putc(*((char *)(a[2]) + i));
      //   }
      // }
      // // if (*(int *)(a[1]) == 1 || *(int *)(a[1]) == 2) {

      // //   for (int i = 0; i < *(size_t *) (a[3]); i++) {
      // //     _putc(*((char *)(*(void **)(a[2])) + i));
      // //   }
      // // }
      // c->GPRx = a[3];
      c->GPRx = do_write(a[1], (void*)(a[2]), a[3]);
      break;
    case SYS_kill: break;
    case SYS_getpid: break;
    case SYS_close: 
      c->GPRx = do_close(a[1]);
      break;
    case SYS_lseek: 
      c->GPRx = do_lseek(a[1], a[2], a[3]);
      break;
    case SYS_brk: 
      // 设置堆区大小的系统调用
      // 始终返回0，表示堆区大小的调整总是成功
      // #ifdef HAS_VME
      //   extern int mm_brk(uintptr_t new_brk);
      //   return mm_brk(brk);
      // #else
      //   return 0;
      // #endif
      Log("brk");
      c->GPRx = do_brk(a[1]);
      break;
    case SYS_fstat: break;
    case SYS_time: break;
    case SYS_signal: break;
    case SYS_execve: break;
    case SYS_fork: break;
    case SYS_link: break;
    case SYS_unlink: break;
    case SYS_wait: break;
    case SYS_times: break;
    case SYS_gettimeofday: break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }


  return NULL;
}
