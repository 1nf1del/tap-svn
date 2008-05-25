#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_Terminate --------------------------------
//
void HDD_TAP_Terminate (dword TAPID)
{
  int                   TAPIndex;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return;

  //Return if that entry is not in use
  TAPIndex = HDD_TAP_GetIndexByID (TAPID);
  if (TAPIndex < 0) return;

  if (*(dword *)(TAP_TableAddress + (TAPIndex << 5) + 12) == 0) return;

  *(char*)(TAP_TableAddress + (TAPIndex << 5) + 28) |= 0x01;
}
