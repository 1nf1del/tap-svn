#include "FBLib_tap.h"

bool HDD_TAP_StartedByTAP (void)
{
  tTAPTableInfo         TAPInfo;
  dword                 i, LoadAddress, BatchPointer;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  //Get my own TAPID
  HDD_TAP_GetInfo (TAP_TableIndex, &TAPInfo);

  //Locate a launcher TAP
  for (i = 0; i < TAP_MAX; i++)
  {
    LoadAddress = *(dword *) (TAP_TableAddress + (i << 5) + 12);
    if (LoadAddress)
    {
      //Has the batch pointer being initialized
      BatchPointer = *(dword *) (LoadAddress + 0x24);
      if ((BatchPointer >= 0x80000000) && (BatchPointer <= 0x84000000 - sizeof(dword)) && (*(dword *) BatchPointer == MAGIC))
      {
        if (*(dword *) (BatchPointer + sizeof(dword)) == TAPInfo.TAPID) return TRUE;
      }
    }
  }

  //No launcher found
  return FALSE;
}
