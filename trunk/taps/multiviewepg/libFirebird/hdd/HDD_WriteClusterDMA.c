#include "FBLib_hdd.h"
#include "../libFireBird.h"

void HDD_WriteClusterDMA (dword Cluster, byte *DataBuffer)
{
  if (TAP_Hdd_WriteSectorDMA == NULL) TAP_Hdd_WriteSectorDMA = (tTAP_Hdd_WriteSectorDMA) TAP_GetSystemProc(oTAP_Hdd_WriteSectorDMA);
  TAP_Hdd_WriteSectorDMA ((Cluster + 1) * HDD_GetClusterSize(), HDD_GetClusterSize(), DataBuffer);
}
