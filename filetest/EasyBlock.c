#include "EasyBlock.h"
#include "EasyDefs.h"
#include <string.h>

EASY_BLOCK_SYSTEM gBlockSystem;

EASY_STATUS
InitBlocks(
    IN VOID *MemoryPool,
    IN UINTN Nbyte
    )
{
    UINTN i;

    gBlockSystem.BlockBase = MemoryPool;
    gBlockSystem.BlockNum = Nbyte / BLOCK_SIZE;
    for (i = 0; i < gBlockSystem.BlockNum; ++i) {
        gBlockSystem.Bitmap[i] = 0;
    }

    // FIXME: SetMem(MemoryPool, Nbyte, 0);
    memset(MemoryPool, 0, Nbyte);

    return EASY_SUCCESS;
}

STATIC
EASY_BLOCK*
GetBlock(
    IN UINTN BlockId
    )
{
    return (EASY_BLOCK*)(gBlockSystem.BlockBase + BLOCK_SIZE * BlockId);
}

EASY_STATUS
ReadBlock(
    IN UINTN BlockId,
    IN UINTN Nbyte,
    OUT VOID *Buf
    )
{
    EASY_BLOCK *Block;

    Block = GetBlock(BlockId);
    CopyMem(Buf, Block->BlockByte, Nbyte);

    return EASY_SUCCESS;
}

EASY_STATUS
WriteBlock(
    IN UINTN BlockId,
    IN UINTN Nbyte,
    IN UINTN WritePos,
    IN VOID *Buf
    )
{
    EASY_BLOCK *Block;
    VOID *WriteBuf;

    Block = GetBlock(BlockId);
    WriteBuf = (CHAR8 *)Block->BlockByte + WritePos;
    CopyMem(WriteBuf, Buf, Nbyte);

    return EASY_SUCCESS;
}

EASY_STATUS
AllocBlock(
    OUT UINTN *BlockId
    )
{
    UINTN i;

    for (i = 0; i < MAX_BLOCK; ++i) {
        if (!gBlockSystem.Bitmap[i]) {
            *BlockId = i;
            gBlockSystem.Bitmap[i] = 1;
            return EASY_SUCCESS;
        }
    }

    return EASY_BLOCK_ALLOC_ERROR;
}

EASY_STATUS
FreeBlock(
    IN UINTN BlockId
    )
{
    gBlockSystem.Bitmap[BlockId] = 0;

    return EASY_SUCCESS;
}