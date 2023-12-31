#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define EFI 332
#define STRING_MAX 100
#define CMD_MAX 20
#define ARG_MAX 10

int main(int argc, char *argv[]) {
  printf("welcome to edk-fs!\n");
  char str[STRING_MAX];
  char *args[ARG_MAX];
  char cmd[CMD_MAX];

  memset(cmd, 0, CMD_MAX);
  memset(str, 0, STRING_MAX);
  memset(args, 0, ARG_MAX);

  strcpy(cmd, "start");
  if (syscall(EFI, cmd, 0, args)) {
    printf("failed to create edk-fs\n");
  }
  int num = 0;
  while (1) {
    fputs("edk-fs > ", stdout);
    // char str[50];
    // fgets(str, sizeof(str), stdin); // 从标准输入获取字符串
    // printf("您输入的字符串是：%s\n", str);

    if (fgets(str, sizeof(str), stdin) == NULL) {
      break;
    }
    str[strcspn(str, "\n")] = '\0';
    char *token = strtok(str, " "); // 以空格为分隔符
    if (!strcmp(token, "exit") || !strcmp(token, "quit") ||
        !strcmp(token, "q")) {

      break;
    } else if (!strcmp(token, "create") || !strcmp(token, "remove") ||
               !strcmp(token, "write") || !strcmp(token, "read") ||
               !strcmp(token, "ls") || !strcmp(token, "mkdir") ||
               !strcmp(token, "echo")) {
      strcpy(cmd, token);
      while (token != NULL) {
        token = strtok(NULL, " ");
        if (token != NULL) {
          args[num] = malloc(STRING_MAX);
          memset(args[num], 0, STRING_MAX);
          strcpy(args[num], token);
          //   printf("args[%d]:%s\n", num, args[num]);
          num++;
        }
      }
      printf("syscall EFI with cmd: %s,args num: %d\n", cmd, num);
      syscall(EFI, cmd, num, args);
      for (; num > 0; num--) {
        free(args[num - 1]);
        num--;
      }
    } else {
      printf("cmd %s not found\n", token);
    }
  }
  printf("edk-fs finish!\n");
  return 0;
}
