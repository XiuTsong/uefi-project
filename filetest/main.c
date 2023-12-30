#include <stdio.h>
#include <stdlib.h>
#include "EasyFile.h"
#include "EasyBlock.h"
#include "EasyDefs.h"
#include <string.h>

#define TOTAL_BYTE 102400 

VOID *gMemoryPool;

EFI_STATUS
SampleRuntimeService (
  UINT32               *key
)
{
  *key = *(UINT32 *)gMemoryPool;

  return EFI_SUCCESS;
}

void init_file_system(VOID)
{
  InitBlocks(gMemoryPool, TOTAL_BYTE);
  InitEasyFile();
}

void file_test(VOID)
{
  EASY_STATUS Status;
  CHAR8 buf[10];

  memset(buf, 0, 10);

  Status = EasyCreateFile("abc.txt");
  if (Status != EASY_SUCCESS) {
    printf("Create File failed\n");
    return;
  }

  Status = EasyWriteFile("abc.txt", 5, "abcd");
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  Status = EasyReadFile("abc.txt", 5, buf);
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  printf("buf: %s\n", buf);
}

int
main (
  VOID
  )
{
  EFI_STATUS  Status;

  gMemoryPool = malloc(sizeof(CHAR8) * TOTAL_BYTE);

  init_file_system();

  file_test();

  return EFI_SUCCESS;
}
