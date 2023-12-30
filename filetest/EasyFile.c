#include "EasyFile.h"
#include "EasyBlock.h"
#include "EasyDefs.h"
#include <string.h>
#include <stdio.h>

EASY_FILE_DIR RootDir;
EASY_FILE gFilePool[MAX_FILE_NUM];
BOOLEAN gFilePoolAllocated[MAX_FILE_NUM];

/************************************************************
 *        Tool functions
 ************************************************************/
STATIC
UINTN
GetFileNameLen(
    VOID *FileName
    )
{
    UINTN i;
    CHAR8 *ch;

    ch = FileName;
    for (i = 0; *ch != 0; ++i, ++ch);

    printf("filelen %d\n", i);

    return i;
}

/**
 * return 0 - not equal
 *        1 - equal
 */
STATIC
BOOLEAN
CompareFileName(
    VOID *Name1,
    VOID *Name2
    )
{
    UINTN len1;
    UINTN len2;
    CHAR8 *ch1;
    CHAR8 *ch2;
    UINTN i;

    len1 = GetFileNameLen(Name1);
    len2 = GetFileNameLen(Name2);
    if (len1 != len2) return 0;


    for (i = 0, ch1 = Name1, ch2 = Name2; i < len1; ++i, ++ch1, ++ch2) {
        if (*ch1 != *ch2) return 0;
    }

    return 1;
}

/************************************************************
 *        Directory Layer
 ************************************************************/

EASY_FILE*
EasyDirGetFile(
    EASY_FILE_DIR *Dir,
    VOID *FileName
    )
{
    UINTN i;
    EASY_FILE *File;

    for (i = 0; i < MAX_FILE_NUM; ++i) {
        if (Dir->FileIds[i] == 0)
            continue;
        File = &gFilePool[Dir->FileIds[i]];
        if (CompareFileName(File->Name, FileName))
            return File;
    }

    return NULL;
}

STATIC
EASY_FILE_DIR*
GetCurDir(
    VOID
    )
{
    /** Currently, we only create file at root dir */
    return &RootDir;
}

EASY_STATUS
EasyDirAddFile(
    EASY_FILE_DIR *Dir,
    UINTN FileId
    )
{
    UINTN i;

    for (i = 0; i < MAX_FILE_NUM; ++i) {
        if (Dir->FileIds[i] != 0) {
            Dir->FileIds[i] = FileId;
            return EASY_SUCCESS;
        }
    }
    return -EASY_DIR_TOO_MANY_FILE_ERROR;
}

EASY_STATUS
EasyDirRemoveFile(
    EASY_FILE_DIR *Dir,
    UINTN FileId
)
{    
    UINTN i;
    
    for (i = 0; i< MAX_FILE_NUM; ++i){
        if(Dir->FileIds[i] == FileId){
            Dir->FileIds[i] = 0;
            return EASY_SUCCESS;
        }
    }
    return -EASY_DIR_NOT_FOUND_ERROR;
}

STATIC
UINTN
GetFile(
    VOID
    )
{
    UINTN i;

    for (i = 0; i < MAX_FILE_NUM; ++i) {
        if (!gFilePoolAllocated[i]) {
            return i;
        }
    }

    return -EASY_FILE_NO_MORE_FILE_ERROR;
}

/************************************************************
 *        File Layer
 ************************************************************/

EASY_STATUS
EasyCreateFile(
    VOID *FileName
    )
{
    EASY_STATUS Status;
    EASY_FILE *NewFile = NULL;
    UINTN NewFileId;
    UINTN FileNameLen;
    UINTN NewBlockId;
    EASY_FILE_DIR *CurDir;

    NewFileId = GetFile();
    NewFile = &gFilePool[NewFileId];

    FileNameLen = GetFileNameLen(FileName);
    CopyMem(NewFile->Name, FileName, FileNameLen * sizeof(CHAR8));
    NewFile->Type = EASY_FILE_TYPE_NORMAL;
    NewFile->Id = NewFileId;
    NewFile->FileSize = 0;

    Status = AllocBlock(&NewBlockId);
    if (Status != EASY_SUCCESS) {
        return Status;
    }
    NewFile->BlockNum = 1;
    NewFile->BlockIds[NewFile->BlockNum - 1] = NewBlockId;

    CurDir = GetCurDir();

    EasyDirAddFile(CurDir, NewFile->Id);

    return EASY_SUCCESS;
}

EASY_STATUS
EasyRemoveFile(
    VOID *FileName
    )
{
    UINTN i;
    EASY_STATUS Status;
    EASY_FILE *DeleteFile = NULL;
    EASY_FILE_DIR *CurDir;

    CurDir = GetCurDir();
    DeleteFile = EasyDirGetFile(CurDir, FileName);
    if (!DeleteFile) {
        return -EASY_FILE_NOT_FOUND_ERROR;
    }

    // Free block memory
    for (i = 0; i < DeleteFile->BlockNum; ++i){
        Status = FreeBlock(DeleteFile->BlockIds[i]);
        if (Status != EASY_SUCCESS) {
            return Status;
        }
    }
    // Update dir status
    Status = EasyDirRemoveFile(CurDir, DeleteFile->Id);
    if (Status != EASY_SUCCESS) {
        return Status;
    }

    // Clear file data
    SetMem(DeleteFile->Name, MAX_FILE_NAME_LEN, 0);
    DeleteFile->FileSize = 0;
    DeleteFile->BlockNum = 0;

    return EASY_SUCCESS;
}


EASY_STATUS
EasyReadFile(
    VOID *FileName,
    UINTN Nbyte,
    VOID *Buf
    )
{
    EASY_FILE_DIR *CurDir;
    EASY_FILE *File;

    CurDir = GetCurDir();

    File = EasyDirGetFile(CurDir, FileName);
    if (!File) {
        return -EASY_FILE_NOT_FOUND_ERROR;
    }

    /* Currently we assume one file contains only one blocks! */
    if (Nbyte > BLOCK_SIZE) {
        return -EASY_FILE_NOT_SUPPORT;
    }

    ReadBlock(File->BlockIds[0], Nbyte, Buf);

    return EASY_SUCCESS;
}

EASY_STATUS
EasyWriteFile(
    VOID *FileName,
    UINTN Nbyte,
    VOID *Buf
    )
{
    EASY_FILE_DIR *CurDir;
    EASY_FILE *File;
    EASY_STATUS Status;
    UINTN WritePos;

    CurDir = GetCurDir();

    File = EasyDirGetFile(CurDir, FileName);
    if (!File) {
        return -EASY_FILE_NOT_FOUND_ERROR;
    }

    /* Currently we assume one file contains only one blocks! */
    if (Nbyte > BLOCK_SIZE) {
        return -EASY_FILE_NOT_SUPPORT;
    }

    /* Currently we only support write-append.
     * Thus write postion == file size.
     */
    WritePos = File->FileSize;
    Status = WriteBlock(File->BlockIds[0], Nbyte, WritePos, Buf);
    if (Status != EASY_SUCCESS) {
        return Status;
    }
    File->FileSize += Nbyte;

    return EASY_SUCCESS;
}