#include <stdio.h>
#include <stdlib.h>
#include "EasyDefs.h"

VOID *gMemoryPool;

EFI_STATUS
SampleRuntimeService (
  UINT32               *key
)
{
  *key = *(UINT32 *)gMemoryPool;

  return EFI_SUCCESS;
}

int
main (
  VOID
  )
{
  EFI_STATUS  Status;

  gMemoryPool = malloc(sizeof(UINTN) * 102400);

  return EFI_SUCCESS;
}