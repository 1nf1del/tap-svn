#include "FBLib_hdd.h"
#include "../libFireBird.h"

dword CalcAbsSectorFromFAT (TYPE_File *fp, dword RelSector)
{
  dword                 AbsCluster;
  unsigned char         *pFAT;


  AbsCluster = fp->startCluster;
  if (AbsCluster > 131071) return 0;

  pFAT = (char *) HDD_LiveFS_GetFAT1Address();

  while (RelSector >= HDD_GetClusterSize())
  {
    AbsCluster = (pFAT [3 * AbsCluster] << 16) + (pFAT [3 * AbsCluster + 1] << 8) + pFAT [3 * AbsCluster + 2];
    if (AbsCluster > 131071) return 0;
    RelSector -= HDD_GetClusterSize();
  }
  return (HDD_GetClusterSize() * (AbsCluster + 1) + RelSector);
}
