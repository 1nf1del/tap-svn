#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_isDisabledAll --------------------------------
//
bool HDD_TAP_isDisabledAll (void)
{
  dword                 TAPID, i;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  for (i = 0; i < TAP_MAX; i++)
  {
    if (i != TAP_TableIndex)
    {
      TAPID = HDD_TAP_GetIDByIndex ((int) i);
      if (TAPID)
      {
        if (HDD_TAP_isDisabled (TAPID) == 0) return FALSE;
      }
    }
  }

  return TRUE;
}
