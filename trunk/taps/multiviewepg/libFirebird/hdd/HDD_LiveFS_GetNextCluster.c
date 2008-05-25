#include "FBLib_hdd.h"
#include "../libFireBird.h"

//--------------------------------------- HDD_LiveFS_GetNextCluster --------------------------------
//
// Return value: 0..131071 = next cluster
//                      -1 = Unused or invalid cluster
//                      -2 = End of chain reached
//
int HDD_LiveFS_GetNextCluster (int Cluster)
{
  int                   PointsTo;

  PointsTo = GetClusterPointer (Cluster);
  if (PointsTo <         0) return -1;
  if (PointsTo == 0xffffff) return -1;
  if (PointsTo == 0xfffffe) return -2;  //Cluster is the last cluster in the chain

  return PointsTo;
}
