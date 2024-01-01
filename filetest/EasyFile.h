#ifndef __FILE_H__
#define __FILE_H__

#include "EasyDefs.h"
#include "EasyBlock.h"

typedef unsigned int EASY_FILE_TYPE;

#define EASY_TYPE_FILE   0
#define EASY_TYPE_DIR   0

#define MAX_FILE_NUM 100
#define MAX_FILE_NAME_LEN 20
#define MAX_FILE_BLOCKS 10
#define MAX_DIR_NUM 10

typedef struct {
    CHAR8 Name[MAX_FILE_NAME_LEN];
    UINTN BlockIds[MAX_FILE_BLOCKS];
    UINTN BlockNum;
    UINTN FileSize;     // Byte
    EASY_FILE_TYPE Type; // EASY_FILE can be file or directory
    UINTN Id;
} EASY_FILE;

typedef struct {
    UINTN FileIds[MAX_FILE_NUM];
    EASY_FILE *SelfFile; // Point to EASY_FILE struct that contains itself
    UINTN FileNum;
} EASY_DIR; // sizeof(EASY_FILE_DIR) = 8Byte * 101 < 1024Byte(1 Block)

EASY_DIR*
EasyCreateDir(
    VOID *DirName
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

EASY_STATUS
EasyDirListFiles(
    VOID *DirName,
    VOID *buf
    );

EFI_STATUS
InitFileLayer(
    VOID
    );

EFI_STATUS
EasyPwd(
    VOID *buf
    );

EASY_STATUS
EasyCd(
    VOID *DirName
    );

EASY_STATUS
EasyCat(
    VOID *FileName,
    VOID *Buf
    );

EASY_STATUS
EasyLs(
    VOID *Buf
    );
#endif
