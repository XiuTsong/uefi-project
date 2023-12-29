#ifndef __BLOCK_H__
#define __BLOCK_H__

#include "EasyDefs.h"

#define MAX_BLOCK 100
#define BLOCK_SIZE 1024

typedef struct {
    BOOLEAN Bitmap[MAX_BLOCK];
    UINTN BlockNum;
    VOID* BlockBase;
} EASY_BLOCK_SYSTEM;

typedef struct {
    CHAR8 BlockByte[BLOCK_SIZE];
} EASY_BLOCK;

EASY_STATUS
InitBlocks(
    VOID *MemoryPool,
    UINTN Nbyte
    );

EASY_STATUS
ReadBlock(
    UINTN BlockId,
    UINTN Nbyte,
    VOID *Buf
    );

EASY_STATUS
WriteBlock(
    UINTN BlockId,
    UINTN Nbyte,
    UINTN Pos,     // Write start position
    VOID *Buf
    );

EASY_STATUS
AllocBlock(
    UINTN *BlockId
    );

EASY_STATUS
FreeBlock(
    UINTN *BlockId
    );

#endif