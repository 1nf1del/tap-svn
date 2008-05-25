#include                "../libFireBird.h"

bool AddEventHandler (word EventID, void *EventHandler, dword State, dword Priority)
{
  dword          fwAddEvtHndlr0;

  if (!(fwAddEvtHndlr0 = FIS_fwAddEventHandler())) return FALSE;

  return CallFirmware (fwAddEvtHndlr0, EventID, (dword)EventHandler, State, Priority) != 0;
}
