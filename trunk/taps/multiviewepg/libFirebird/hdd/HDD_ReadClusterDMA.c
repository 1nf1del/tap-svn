#include "FBLib_hdd.h"
#include "../libFireBird.h"

void HDD_ReadClusterDMA (dword Cluster, byte *DataBuffer)
{
  if (TAP_Hdd_ReadSectorDMA == NULL) TAP_Hdd_ReadSectorDMA = (tTAP_Hdd_ReadSectorDMA) TAP_GetSystemProc(oTAP_Hdd_ReadSectorDMA);
  TAP_Hdd_ReadSectorDMA ((Cluster + 1) * HDD_GetClusterSize(), HDD_GetClusterSize(), DataBuffer);
}
