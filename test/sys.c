#include <stdio.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#define SYSCALL 333

int main(int argc, char *argv[]) {
  syscall(SYSCALL);
  printf("ok! run dmesg | grep runtime in terminal!\n");
  return 0;
}