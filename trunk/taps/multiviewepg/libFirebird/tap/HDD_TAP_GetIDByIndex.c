#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_GetIDByIndex --------------------------------
//
dword HDD_TAP_GetIDByIndex (int TAPIndex)
{
  dword                 LoadAddress;

  if ((TAPIndex < 0) || (TAPIndex >= TAP_MAX)) return 0;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  LoadAddress = *(dword *) (TAP_TableAddress + (TAPIndex << 5) + 12);
  if (LoadAddress == 0) return 0;

  return *(dword*)(LoadAddress + 0x0020);
}
