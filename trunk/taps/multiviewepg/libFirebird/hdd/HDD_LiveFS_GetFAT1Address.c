#include "FBLib_hdd.h"
#include "../libFireBird.h"

dword HDD_LiveFS_GetFAT1Address (void)
{
  return FIS_vHDDLiveFSFAT1();
}
