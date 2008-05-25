#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_isRunning --------------------------------
//
bool HDD_TAP_isRunning (dword TAPID)
{
  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  return (HDD_TAP_GetIndexByID (TAPID) >= 0);
}
