#include "FBLib_hdd.h"
#include "../libFireBird.h"

dword HDD_GetFirmwareDirCluster (void)
{
  if (!LibInitialized) InitTAPex();
  if (!LibInitialized) return 0;

  return *(dword*)(*(dword*)(pWD2) + 8);
}

