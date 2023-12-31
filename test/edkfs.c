#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define EFI 332
int main(int argc, char *argv[]) {
  printf("welcome to edk-fs!\n");
  char str[100];
  char *args[10];
  char cmd[10];
  strcpy(cmd, "start");
  if (syscall(EFI, cmd, 0, args)) {
    print("failed to create edk-fs\n");
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
          args[num] = malloc(strlen(token) + 1);
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