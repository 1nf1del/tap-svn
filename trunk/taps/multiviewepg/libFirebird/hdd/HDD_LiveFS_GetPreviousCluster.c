#include "FBLib_hdd.h"
#include "../libFireBird.h"

//--------------------------------------- HDD_LiveFS_GetPreviousCluster --------------------------------
//
// Return value: 0..131071 = previous cluster
//                      -1 = Unused or invalid cluster
//                      -2 = Start of chain reached
//
int HDD_LiveFS_GetPreviousCluster (int Cluster)
{
  int                   PointsTo, i;

  PointsTo = GetClusterPointer (Cluster);
  if ((PointsTo < 0) || (PointsTo == 0xffffff)) return -1;

  for (i = 0; i < 131071; i++)
  {
    if (GetClusterPointer (i) == Cluster) return i;
  }

  return -2;
}
