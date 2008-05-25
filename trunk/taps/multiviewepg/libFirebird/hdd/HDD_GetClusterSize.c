#include "FBLib_hdd.h"
#include "../libFireBird.h"

dword                   ClusterSize = 0;
unsigned char           HDDIOBuffer [512];

dword HDD_GetClusterSize (void)
{
  if (ClusterSize == 0)
  {
    HDD_ReadSectorDMA (0, 1, HDDIOBuffer);
    ClusterSize = (HDDIOBuffer [0x22] << 8) | HDDIOBuffer [0x23];
  }
  return ClusterSize;
}
