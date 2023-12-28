#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define EFI 332
int main(int argc, char *argv[]) {
  int num = argc - 2;
  char **args = NULL;
  printf("ok!argc:%d,num:%d\n", argc, num);

  if (argc > 2) {
    args = malloc((argc - 2) * sizeof(char *));
    for (int i = 2; i < argc; i++) {
      args[i - 2] = malloc(strlen(argv[i]) + 1);
      strcpy(args[i - 2], argv[i]);
      // args[i - 2] = argv[i];
    }
  }
  char *cmd = malloc(strlen(argv[1]) + 1);
  strcpy(cmd, argv[1]);
  // memcpy(cmd, argv[1], sizeof(argv[1]));
  printf("cmd:%s, num:%d\n ", cmd, num);

  syscall(EFI, cmd, num, args);
  for (int i = 0; i < num; i++) {
    free(args[i]);
  }
  free(args);
  free(cmd);
  return 0;
}