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
  InitBlockLayer(gMemoryPool, TOTAL_BYTE);
  InitFileLayer();
}

void file_test_1(VOID)
{
  EASY_STATUS Status;
  CHAR8 buf[10];

  memset(buf, 0, 10);

  Status = EasyCreateFile("abc.txt");
  if (Status != EASY_SUCCESS) {
    printf("Create File failed\n");
    return;
  }

  Status = EasyWriteFile("abc.txt", 5, "abcde");
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  Status = EasyWriteFile("abc.txt", 3, "def");
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  Status = EasyReadFile("abc.txt", 8, buf);
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  printf("buf: %s\n", buf);

  Status = EasyRemoveFile("abc.txt");
  if (Status != EASY_SUCCESS) {
    printf("Remove File failed\n");
    return;
  }

  memset(buf, 0, 10);

  Status = EasyCreateFile("def.txt");
  if (Status != EASY_SUCCESS) {
    printf("Create File failed\n");
    return;
  }

  Status = EasyWriteFile("def.txt", 3, "def");
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  Status = EasyReadFile("def.txt", 3, buf);
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  printf("buf: %s\n", buf);
}

void file_test_2()
{
  EASY_STATUS Status;
  CHAR8 buf1[10];
  CHAR8 buf2[10];
  CHAR8 buf3[10];

  memset(buf1, 0, 10);
  memset(buf2, 0, 10);
  memset(buf3, 0, 10);

  Status = EasyCreateFile("a.txt");
  Status |= EasyCreateFile("b.txt");
  Status |= EasyCreateFile("c.txt");
  if (Status != EASY_SUCCESS) {
    printf("Create File failed\n");
    return;
  }

  Status = EasyWriteFile("a.txt", 3, "xxx");
  Status |= EasyWriteFile("b.txt", 3, "!!!");
  Status |= EasyWriteFile("c.txt", 3, "@@@");
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  Status = EasyReadFile("a.txt", 3, buf1);
  Status |= EasyReadFile("b.txt", 3, buf2);
  Status |= EasyReadFile("c.txt", 3, buf3);
  if (Status != EASY_SUCCESS) {
    printf("Write File failed\n");
    return;
  }

  printf("buf1: %s\n", buf1);
  printf("buf2: %s\n", buf2);
  printf("buf3: %s\n", buf3);
}

void dir_test_1()
{
  EASY_STATUS Status;
  CHAR8 buf[100];
  memset(buf, 0, 10);

  Status = EasyCreateFile("a.txt");
  Status |= EasyCreateFile("b.txt");
  Status |= EasyCreateFile("c.txt");
  if (Status != EASY_SUCCESS) {
    printf("Create File failed\n");
    return;
  }

  EasyDirListFiles("/", buf);

  printf("files: %s\n", buf);
}

int
main (
  VOID
  )
{
  EFI_STATUS  Status;

  gMemoryPool = malloc(sizeof(CHAR8) * TOTAL_BYTE);

  init_file_system();

  file_test_1();

  file_test_2();

  dir_test_1();

  return EFI_SUCCESS;
}
