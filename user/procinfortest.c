#include "kernel/types.h"
#include "user/user.h"

struct procinfo {
  int pid;
  int ppid;
  int state;
  uint64 sz;
  char name[16];
};

int main(int argc, char *argv[]) {
  struct procinfo info;

  // Gọi system call procinfo cho tiến trình hiện tại (getpid)
  if (procinfo(getpid(), &info) == 0) {
    printf("Process Name: %s\n", info.name);
    printf("PID: %d, PPID: %d\n", info.pid, info.ppid);
    printf("State: %d\n", info.state);
    printf("Memory Size: %ld bytes\n", info.sz);
  } else {
    printf("Error: procinfo failed\n");
  }

  exit(0);
}
