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
  char buffer[STRING_MAX];

  memset(cmd, 0, CMD_MAX);
  memset(str, 0, STRING_MAX);
  memset(buffer, 0, STRING_MAX);
  memset(args, 0, ARG_MAX * sizeof(char *));

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
    }
    strcpy(cmd, token);

    while (token != NULL) {
      token = strtok(NULL, " ");
      if (token != NULL) {
        args[num] = malloc(STRING_MAX);
        memset(args[num], 0, STRING_MAX);
        strcpy(args[num], token);
        printf("args[%d]:%s\n", num, args[num]);
        num++;
      }
    }
    if (!strcmp(cmd, "rm")) {
      strcpy(cmd, "remove");
    }
    if (!strcmp(cmd, "echo")) {
      strcpy(cmd, "write");
      char *temp = args[0];
      args[0] = args[1];
      args[1] = temp;
    }
    if (!strcmp(cmd, "torch")) {
      strcpy(cmd, "create");
    }
    if (!strcmp(cmd, "create") || !strcmp(cmd, "remove") ||
        !strcmp(cmd, "write") || !strcmp(cmd, "read") || !strcmp(cmd, "ls") ||
        !strcmp(cmd, "mkdir") || !strcmp(cmd, "cd") || !strcmp(cmd, "pwd") ||
        !strcmp(cmd, "cat")) {

      // for (int i = 0; i < num; i++) {
      //   printf("args[%d]:%s\n", i, args[i]);
      // }
      printf("syscall EFI with cmd: %s,args num: %d\n", cmd, num);
      syscall(EFI, cmd, num, args, buffer);
      if (!strcmp(cmd, "read")) {
        printf("read:%s\n", buffer);
      }
      if (!strcmp(cmd, "ls")) {
        printf("ls:%s\n", buffer);
      }
      if (!strcmp(cmd, "cat")) {
        printf("cat:%s\n", buffer);
      }
      if (!strcmp(cmd, "pwd")) {
        printf("pwd:%s\n", buffer);
      }
      // printf("num: %d\n", num);
    } else {
      printf("cmd %s not found\n", cmd);
    }
    for (; num > 0; num--) {
      free(args[num - 1]);
    }
  }
  printf("edk-fs finish!\n");
  return 0;
}
