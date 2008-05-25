#include "FBLib_hdd.h"
#include "../libFireBird.h"

//--------------------------------------- HDD_LiveFS_GetFirstCluster --------------------------------
//
// Return value: 0..131071 = first cluster
//                      -1 = Unused or invalid cluster
//
int HDD_LiveFS_GetFirstCluster (int Cluster)
{
  int                   PointsTo;

  do
  {
    PointsTo = HDD_LiveFS_GetPreviousCluster (Cluster);
    if (PointsTo ==    -1) return -1;
    if (PointsTo >=     0) Cluster = PointsTo;
  } while (PointsTo != -2);

  return Cluster;
}
