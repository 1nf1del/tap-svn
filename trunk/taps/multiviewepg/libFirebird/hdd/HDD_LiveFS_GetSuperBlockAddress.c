#include "FBLib_hdd.h"
#include "../libFireBird.h"

dword            pHDDLiveFSSuperBlock = 0;

dword HDD_LiveFS_GetSuperBlockAddress (void)
{
  return FIS_vHDDLiveFSSupperblock();
}
