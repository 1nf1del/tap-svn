#include "FBLib_hdd.h"
#include "../libFireBird.h"

//--------------------------------------- HDD_LiveFS_GetFAT2Address --------------------------------
//
dword HDD_LiveFS_GetFAT2Address (void)
{
  dword                 pHDDLiveFSFAT1 = HDD_LiveFS_GetFAT1Address ();

  if (pHDDLiveFSFAT1 == 0) return 0;

  return pHDDLiveFSFAT1 + 0x00060000;
}
