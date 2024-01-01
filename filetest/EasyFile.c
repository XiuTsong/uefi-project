#include "EasyFile.h"
#include "EasyBlock.h"
#include "EasyDefs.h"
#include <string.h>
#include <stdio.h>

EASY_DIR *RootDir;
EASY_DIR *CurrentDir;
EASY_FILE gFilePool[MAX_FILE_NUM];
BOOLEAN gFilePoolAllocated[MAX_FILE_NUM];

EASY_FILE*
CreateFileInternal(
    VOID *FileName,
    EASY_FILE_TYPE Type
    );

EASY_STATUS
EasyDirAddFile(
    EASY_DIR *Dir,
    UINTN FileId
    );

EASY_FILE*
EasyDirGetFile(
    EASY_DIR *Dir,
    VOID *FileName
    );

EASY_STATUS
EasyDirRemoveFile(
    EASY_DIR *Dir,
    UINTN FileId
);

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

    // printf("filelen %d\n", i);
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
 *        File Pool Related
 ************************************************************/

#define FilePoolGetFileById(FileId) (&gFilePool[(FileId)])

STATIC
EASY_FILE*
FilePoolGetNewFile(
    VOID *FileName,
    EASY_FILE_TYPE Type
    )
{
    UINTN i;
    EASY_FILE *NewFile;
    UINTN FileNameLen;

    /** We remain the first file in gFilePool unused */
    for (i = 1; i < MAX_FILE_NUM; ++i) {
        if (gFilePoolAllocated[i]) {
            continue;
        }
        gFilePoolAllocated[i] = 1;
        NewFile = &gFilePool[i];
        FileNameLen = GetFileNameLen(FileName);
        CopyMem(NewFile->Name, FileName, FileNameLen * sizeof(CHAR8));
        NewFile->Type = Type;
        NewFile->FileSize = 0;
        return NewFile;
    }

    return NULL;
}

EASY_STATUS
EasyInitFilePool(
    VOID
    )
{
    UINTN i;

    // FIXME: SetMem()
    memset(gFilePoolAllocated, 0, sizeof(gFilePoolAllocated));

    for (i = 0; i < MAX_FILE_NUM; ++i) {
        gFilePool[i].Id = i;
        gFilePoolAllocated[i] = 0;
    }

    return EASY_SUCCESS;
}

/************************************************************
 *        EasyDir Related
 ************************************************************/
#define GetCurDir() (CurrentDir)
#define SetCurDir(Dir) (CurrentDir = (Dir))

STATIC
EASY_DIR*
EasyFileToEasyDir(
    EASY_FILE* File
    )
{
    if (File->Type != EASY_TYPE_DIR) 
        return NULL;

    return (EASY_DIR *)GetBlock(File->BlockIds[0]);
}

STATIC
EASY_FILE*
EasyDirToEasyFile(
    EASY_DIR* Dir
    )
{
    return Dir->SelfFile;
}

EASY_DIR*
GetEasyDirByName(
    VOID *DirName,
    EASY_DIR *CurDir
    )
{
    EASY_FILE *DirFile;
    EASY_DIR *Dir;

    if (CompareFileName(DirName, "/")) {
        return RootDir;
    }

    DirFile = EasyDirGetFile(CurDir, DirName);
    if (!DirFile || DirFile->Type != EASY_TYPE_DIR) {
        return NULL;
    }

    Dir = EasyFileToEasyDir(DirFile);

    return Dir;
}

BOOLEAN
EasyDirCheckFileExist(
    EASY_DIR *Dir,
    VOID *FileName
    )
{
    UINTN i;
    EASY_FILE *File;
    /** TODO: Check whether @FileName exist in @Dir */
    for (i = 0; i < MAX_FILE_NUM; ++i) {
        if (Dir->FileIds[i] == 0)
            continue;
        File = &gFilePool[Dir->FileIds[i]];
        if (CompareFileName(File->Name, FileName))
            return 1;
    }
    return 0;
}

STATIC
EASY_DIR*
CreateDirInternal(
    VOID *FileName
    )
{
    EASY_FILE *NewDirFile;
    EASY_DIR *NewDir;

    /** Directory is also a type of "EASY_FILE" */
    NewDirFile = CreateFileInternal(FileName, EASY_TYPE_DIR);
    if (!NewDirFile)
    {
        return NULL;
    }

    NewDir = EasyFileToEasyDir(NewDirFile);
    NewDir->SelfFile = NewDirFile;
    NewDir->FileNum = 0;

    return NewDir;
}

EASY_DIR*
CreateDir(
    VOID *FileName,
    BOOLEAN IsRoot,
    EASY_DIR *CurDir
    )
{
    EASY_STATUS Status;
    EASY_DIR *NewDir;
    EASY_DIR *DotDir; // ls .
    EASY_DIR *DotDotDir; // cd ..

    if (!IsRoot && EasyDirCheckFileExist(CurDir, FileName)) {
        // FIXME: Already exist, return NULL
        return NULL;
    }

    NewDir = CreateDirInternal(FileName);
    if (!NewDir) {
        return NULL;
    }

    DotDir = CreateDirInternal(".");
    if (!DotDir) {
        return NULL;
    }

    DotDotDir = CreateDirInternal("..");
    if (!DotDotDir) {
        return NULL;
    }

    Status = EasyDirAddFile(DotDir, EasyDirToEasyFile(NewDir)->Id);
    if (Status != EASY_SUCCESS) {
        return NULL;
    }

    DotDir->SelfFile = NewDir->SelfFile;
    if (IsRoot) {
        /** RootDir: ".." -> RootDir(NewDir) itself */
        DotDotDir->SelfFile = NewDir->SelfFile;
    } else {
        /** Common Dir . */
        DotDotDir->SelfFile = CurDir->SelfFile;
    }

    /** Add . and .. to newly created directory */
    Status = EasyDirAddFile(NewDir, EasyDirToEasyFile(DotDir)->Id);
    if (Status != EASY_SUCCESS) {
        return NULL;
    }
    Status = EasyDirAddFile(NewDir, EasyDirToEasyFile(DotDotDir)->Id);
    if (Status != EASY_SUCCESS) {
        return NULL;
    }

    if (!IsRoot && CurDir) {
        Status = EasyDirAddFile(CurDir, NewDir->SelfFile->Id);
        if (Status != EASY_SUCCESS) {
            return NULL;
        }
    }

    return NewDir;
}

EASY_STATUS
EasyCreateRootDir(
    VOID
    )
{
    RootDir = CreateDir("/", 1, NULL);
    if (!RootDir) {
        return -EASY_DIR_CREATE_ROOT_DIR_FAILED;
    }
    CurrentDir = RootDir;

    return EASY_SUCCESS;
}

EASY_DIR*
EasyCreateDir(
    VOID *DirName
    )
{
    EASY_DIR *CurDir = GetCurDir();

    return CreateDir(DirName, 0, CurDir);
}

EASY_STATUS
EasyDirListFiles(
    VOID *DirName,
    VOID *Buf
    )
{
    EASY_DIR *Dir;
    EASY_DIR *CurDir = GetCurDir();
    EASY_FILE *File;
    CHAR8 *BufPtr;
    UINTN FileLen;
    UINTN i;

    Dir = GetEasyDirByName(DirName, CurDir);

    BufPtr = Buf;
    for (i = 0; i < Dir->FileNum; ++i) {
        File = FilePoolGetFileById(Dir->FileIds[i]);
        FileLen = GetFileNameLen(File->Name);
        CopyMem(BufPtr, File->Name, sizeof(CHAR8) * FileLen);
        BufPtr += FileLen;
        /** Add space ' ' to separate file names */
        *BufPtr = ' ';
        BufPtr++;
    }

    return EASY_SUCCESS;
}

EASY_FILE*
EasyDirGetFile(
    EASY_DIR *Dir,
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

EASY_STATUS
EasyDirAddFile(
    EASY_DIR *Dir,
    UINTN FileId
    )
{
    UINTN i;

    for (i = 0; i < MAX_FILE_NUM; ++i) {
        if (Dir->FileIds[i] == 0) {
            Dir->FileIds[i] = FileId;
            Dir->FileNum++;
            return EASY_SUCCESS;
        }
    }
    return -EASY_DIR_TOO_MANY_FILE_ERROR;
}

EASY_STATUS
EasyDirRemoveFile(
    EASY_DIR *Dir,
    UINTN FileId
)
{
    UINTN i;

    for (i = 0; i< MAX_FILE_NUM; ++i){
        if(Dir->FileIds[i] == FileId){
            Dir->FileIds[i] = 0;
            Dir->FileNum--;
            return EASY_SUCCESS;
        }
    }
    return -EASY_DIR_NOT_FOUND_ERROR;
}

/************************************************************
 *        EasyFile Related
 ************************************************************/

EASY_FILE*
CreateFileInternal(
    VOID *FileName,
    EASY_FILE_TYPE Type
    )
{
    EASY_STATUS Status;
    EASY_FILE *NewFile = NULL;
    UINTN NewBlockId;

    NewFile = FilePoolGetNewFile(FileName, Type);
    if (!NewFile) {
        return NULL;
    }

    Status = AllocBlock(&NewBlockId);
    if (Status != EASY_SUCCESS) {
        return NULL;
    }
    NewFile->BlockNum = 1;
    NewFile->BlockIds[NewFile->BlockNum - 1] = NewBlockId;

    return NewFile;
}

EASY_STATUS
EasyCreateFile(
    VOID *FileName
    )
{
    EASY_FILE *NewFile = NULL;
    EASY_DIR *CurDir;

    CurDir = GetCurDir();

    if (EasyDirCheckFileExist(CurDir, FileName))
    {
        /** File already exist, do nothing. */
        return EASY_SUCCESS;
    }

    NewFile = CreateFileInternal(FileName, EASY_TYPE_FILE);
    if (!NewFile) {
        return -EASY_FILE_CREATE_FAILED;
    }

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
    EASY_DIR *CurDir;

    CurDir = GetCurDir();

    if (!EasyDirCheckFileExist(CurDir, FileName))
    {
        /** File does not exist, do nothing. */
        return EASY_SUCCESS;
    }

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
    EASY_DIR *CurDir;
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
    EASY_DIR *CurDir;
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

/************************************************************
 *        File Layer
 ************************************************************/
EFI_STATUS
InitFileLayer(
    VOID
)
{
    EASY_STATUS Status;
    Status = EasyInitFilePool();
    if (Status != EASY_SUCCESS) {
        return Status;
    }

    Status = EasyCreateRootDir();
    if (Status != EASY_SUCCESS) {
        return Status;
    }

    return Status;
}

EFI_STATUS
EasyPwd(
    VOID *buf
    )
{
    EASY_DIR *CurDir = GetCurDir();
    EASY_FILE *CurDirFile = EasyDirToEasyFile(CurDir);
    UINTN FileNameLen;

    FileNameLen = GetFileNameLen(CurDirFile->Name);

    CopyMem(buf, CurDirFile->Name, sizeof(CHAR8) * FileNameLen);

    return EASY_SUCCESS;
}

EASY_STATUS
EasyCd(
    VOID *DirName
    )
{
    EASY_DIR *Dir;
    EASY_DIR *CurDir;

    CurDir = GetCurDir();
    if (CompareFileName(DirName, ".")) {
        return EASY_SUCCESS;
    }

    Dir = GetEasyDirByName(DirName, CurDir);
    if (!Dir) {
        printf("%s: get dir failed\n", __func__);
        return -EASY_DIR_NOT_FOUND_ERROR;
    }
    if (CompareFileName(DirName, "..")) {
        SetCurDir(EasyFileToEasyDir(Dir->SelfFile));
    } else {
        SetCurDir(Dir);
    }

    return EASY_SUCCESS;
}
