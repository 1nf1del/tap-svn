#include "FBLib_main.h"

char *GetToppyString (word SysID)
{
  dword                 i;
  tToppyInfo           *ToppyInfo;
  tFWDATHeader         *FWDatHeader;

  if (LoadFirmwareDat(&FWDatHeader, &ToppyInfo, NULL))
  {
    for (i = 0; i < FWDatHeader->NrOfToppyInfoEntries; i++, ToppyInfo++)
    {
      if (ToppyInfo->SysID == SysID) return ToppyInfo->Device;
    }
  }

  return "???";
}
