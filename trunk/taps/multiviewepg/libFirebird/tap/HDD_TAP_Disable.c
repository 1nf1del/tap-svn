#include "FBLib_tap.h"

tDisabledEvents         DisabledEvents [TAP_MAX];
bool                    DisabledEventsArrayInitialized = FALSE;

//--------------------------------------- HDD_TAP_Disable --------------------------------
//
bool HDD_TAP_Disable (dword TAPID, bool DisableEvents)
{
  dword                 LoadAddress, Tablegp, isDisabled;
  int                   TAPIndex;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  if (DisabledEventsArrayInitialized == FALSE)
  {
    memset (DisabledEvents, 0, sizeof(DisabledEvents));
    DisabledEventsArrayInitialized = TRUE;
  }

  isDisabled = HDD_TAP_isDisabled (TAPID);

  //Disabled by another TAP? Do not touch.
  if (isDisabled == 2) return FALSE;


  TAPIndex = HDD_TAP_GetIndexByID (TAPID);
  if (TAPIndex < 0) return FALSE;

  LoadAddress = *(dword *) (TAP_TableAddress + (TAPIndex << 5) + 12);
  Tablegp = *(dword *) (TAP_TableAddress + (TAPIndex << 5) + 4);

  if (DisableEvents)
  {
    if (isDisabled != 0) return FALSE;          //The TAP has already been disabled (by us or an other TAP)

    //The 8th entry of the jump table (which has an offset of 0x200 to LoadAddress) contains the pointer to the TAP_EventHandler
    DisabledEvents [TAPIndex].TAPgp                     = Tablegp;
    DisabledEvents [TAPIndex].OriginalEventHandler      = *(dword*)(LoadAddress + 0x200 + 7 * sizeof (dword));
    *(dword*)(LoadAddress + 0x200 + 7 * sizeof (dword)) = (dword) &HDD_TAP_DisabledEventHandler;
  }
  else
  {
    if (isDisabled != 1) return FALSE;          //The TAP has not been disabled by us

    *(dword*)(LoadAddress + 0x200 + 7 * sizeof (dword)) = DisabledEvents [TAPIndex].OriginalEventHandler;
    DisabledEvents [TAPIndex].OriginalEventHandler      = 0;
    DisabledEvents [TAPIndex].TAPgp                     = 0;
  }

  return TRUE;
}
