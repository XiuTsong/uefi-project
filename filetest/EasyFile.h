#ifndef __FILE_H__
#define __FILE_H__

#include "EasyDefs.h"
#include "EasyBlock.h"

typedef unsigned int EASY_FILE_TYPE;

#define EASY_FILE_TYPE_NORMAL   0

#define MAX_FILE_NUM 100
#define MAX_FILE_NAME_LEN 20
#define MAX_FILE_BLOCKS 10
#define MAX_DIR_NUM 10

typedef struct {
    CHAR8 Name[MAX_FILE_NAME_LEN];
    UINTN BlockIds[MAX_FILE_BLOCKS];
    UINTN BlockNum;
    EASY_FILE_TYPE Type;
    UINTN FileSize;     // Byte
    UINTN Id;
} EASY_FILE;

typedef struct {
    UINTN FileIds[MAX_FILE_NUM];
    UINTN DirIds[MAX_FILE_NUM];
} EASY_FILE_DIR;

EASY_STATUS
InitEasyFile(
    VOID
    );

EASY_STATUS
EasyCreateFile(
    VOID *FileName
    );

EASY_STATUS
EasyRemoveFile(
    VOID *FileName
    );

EASY_STATUS
EasyReadFile(
    VOID *FileName,
    UINTN Nbyte,
    VOID *Buf
    );

EASY_STATUS
EasyWriteFile(
    VOID *FileName,
    UINTN Nbyte,
    VOID *Buf
    );

#endif
