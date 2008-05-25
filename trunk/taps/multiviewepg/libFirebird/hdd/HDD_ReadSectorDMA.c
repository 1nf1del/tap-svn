#include "FBLib_hdd.h"
#include "../libFireBird.h"

tTAP_Hdd_ReadSectorDMA TAP_Hdd_ReadSectorDMA = NULL;

void HDD_ReadSectorDMA (dword StartSector, dword NrSectors, byte *DataBuffer)
{
  if (TAP_Hdd_ReadSectorDMA == NULL) TAP_Hdd_ReadSectorDMA = (tTAP_Hdd_ReadSectorDMA) TAP_GetSystemProc(oTAP_Hdd_ReadSectorDMA);
  TAP_Hdd_ReadSectorDMA (StartSector, NrSectors, DataBuffer);
}
