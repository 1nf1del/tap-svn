#include "FBLib_tap.h"

//--------------------------------------- HDD_TAP_isDisabled --------------------------------
//
// Return codes: 0 = not disabled or no TAPID not found
//               1 = The target TAP has been disabled by this TAP
//               2 = The target TAP has been disabled by an other TAP (e.g. TAPCommander)
//               3 = internal error
//
dword HDD_TAP_isDisabled (dword TAPID)
{
  dword                 LoadAddress, Tablegp, EventHandlerAddress;
  int                   TAPIndex;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return 0;

  if (DisabledEventsArrayInitialized == FALSE)
  {
    memset (DisabledEvents, 0, sizeof (DisabledEvents));
    DisabledEventsArrayInitialized = TRUE;
  }

  TAPIndex = HDD_TAP_GetIndexByID (TAPID);
  if (TAPIndex < 0) return 0;

  LoadAddress = *(dword*) (TAP_TableAddress + (TAPIndex << 5) + 12);
  Tablegp = *(dword*) (TAP_TableAddress + (TAPIndex << 5) + 4);
  EventHandlerAddress = *(dword*)(LoadAddress + 0x200 + 7 * sizeof (dword));

  //Check if the gp value has changed. In this case a different TAP has been loaded.
  if ((DisabledEvents [TAPIndex].TAPgp > 0) && (DisabledEvents [TAPIndex].TAPgp != Tablegp))
  {
    DisabledEvents [TAPIndex].OriginalEventHandler = 0;
    DisabledEvents [TAPIndex].TAPgp                = 0;
  }

  //If the pointer to TAP_EventHandler points between LoadAddress and Tablegp, the TAP isn't disabled (not even by an other TAP)
  if ((EventHandlerAddress > LoadAddress) && (EventHandlerAddress < Tablegp)) return 0;

  //We know that the TAP has been disabled. Who did it?
  if (Tablegp == DisabledEvents [TAPIndex].TAPgp) return 1;

  return 2;
}
