#include "FBLib_hdd.h"
#include "../libFireBird.h"

//--------------------------------------- HDD_LiveFS_GetLastCluster --------------------------------
//
// Return value: 0..131071 = last cluster
//                      -1 = Unused or invalid cluster
//
int HDD_LiveFS_GetLastCluster (int Cluster)
{
  int                   PointsTo;

  do
  {
    PointsTo = HDD_LiveFS_GetNextCluster (Cluster);
    if (PointsTo ==    -1) return -1;
    if (PointsTo >=     0) Cluster = PointsTo;
  } while (PointsTo != -2);

  return Cluster;
}
