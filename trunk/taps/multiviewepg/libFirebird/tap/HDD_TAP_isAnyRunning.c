#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_isAnyRunning --------------------------------
//
bool HDD_TAP_isAnyRunning (void)
{
  dword                 i, LoadAddress;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  //Use the $gp value (2nd dword in the the TAP table) instead of the TAP ID as this value is always available
  for (i = 0; i < TAP_MAX; i++)
  {
    LoadAddress = *(dword *) (TAP_TableAddress + (i << 5) + 12);
    if ((LoadAddress != 0) && (i != TAP_TableIndex)) return TRUE;
  }
  return FALSE;
}
