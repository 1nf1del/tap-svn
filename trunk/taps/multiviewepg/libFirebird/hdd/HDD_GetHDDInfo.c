#include "FBLib_hdd.h"
#include "../libFireBird.h"

bool HDD_GetHDDInfo (HDDINFO *HddInfo)
{
  dword         *pHddInfoStructure1, *pHddInfoStructure2;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  if (!(pHddInfoStructure1 = (dword*)FIS_vHddInfoStructure1())) return FALSE;
  if (!(pHddInfoStructure2 = (dword*)FIS_vHddInfoStructure2())) return FALSE;

  if (HddInfo)
  {
    //On the TF6000 models, the HddInterruptOccured field is missing from the HDDINFO struct
    //Check if there is a RAM pointer at offset 4 instead of 8
    if (isValidRAMPtr(pHddInfoStructure1 [1]))
    {
      //TF6xxx
      HddInfo->SectorsPerCluster   = pHddInfoStructure1 [0];
      HddInfo->HddInterruptOccured = 0;
      memcpy (&HddInfo->pClusterBuffer, &pHddInfoStructure1 [1], 5 * sizeof(dword));
    }
    else
    {
      //TF5xxx
      memcpy (&HddInfo->SectorsPerCluster, pHddInfoStructure1, 7 * sizeof(dword));
    }
    HddInfo->NrOfClusters = pHddInfoStructure2 [0];

    return TRUE;
  }
  else return FALSE;
}
