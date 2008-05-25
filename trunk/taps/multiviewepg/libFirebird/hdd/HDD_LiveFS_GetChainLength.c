#include "FBLib_hdd.h"
#include "../libFireBird.h"

//--------------------------------------- HDD_LiveFS_GetLastCluster --------------------------------
//
// Return value: >= 0 = last cluster
//                 -1 = Unused or invalid cluster
//
dword HDD_LiveFS_GetChainLength (int Cluster)
{
  int                   PointsTo;
  dword                 NrClusters = 0;

  PointsTo = HDD_LiveFS_GetFirstCluster (Cluster);
  if (PointsTo < 0) return 0;

  do
  {
    PointsTo = HDD_LiveFS_GetNextCluster (PointsTo);
    NrClusters++;
  } while (PointsTo > 0);

  return NrClusters;
}
