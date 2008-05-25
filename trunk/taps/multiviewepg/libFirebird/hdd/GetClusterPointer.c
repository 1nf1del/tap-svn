#include "FBLib_hdd.h"
#include "../libFireBird.h"

//--------------------------------------- GetClusterPointer --------------------------------
//
// Return value: 0..131071 = cluster pointer
//                0xfffffe = Last cluster in the chain
//                0xffffff = Empty cluster
//                      -1 = Cluster is invalid
//                      -2 = The cluster pointer is invalid
//
int GetClusterPointer (int Cluster)
{
  byte                 *pHDDLiveFSFAT1 = (byte *) HDD_LiveFS_GetFAT1Address();
  int                   PointsTo;

  if ((Cluster > 131071) || (Cluster < 0)) return -1;

  PointsTo = (pHDDLiveFSFAT1 [3 * Cluster] << 16) | (pHDDLiveFSFAT1 [3 * Cluster + 1] << 8) | (pHDDLiveFSFAT1 [3 * Cluster + 2]);

  if ((PointsTo > 131071) && (PointsTo != 0xfffffe) && (PointsTo != 0xffffff)) PointsTo = -2;

  return PointsTo;
}
