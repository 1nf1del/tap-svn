#include "FBLib_hdd.h"
#include "../libFireBird.h"

tTAP_Hdd_WriteSectorDMA TAP_Hdd_WriteSectorDMA = NULL;

void HDD_WriteSectorDMA (dword StartSector, dword NrSectors, byte *DataBuffer)
{
  if (TAP_Hdd_WriteSectorDMA == NULL) TAP_Hdd_WriteSectorDMA = (tTAP_Hdd_WriteSectorDMA) TAP_GetSystemProc(oTAP_Hdd_WriteSectorDMA);
  TAP_Hdd_WriteSectorDMA (StartSector, NrSectors, DataBuffer);
}
