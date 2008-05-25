#include                "../libFireBird.h"

bool DelEventHandler (word EventID, void *EventHandler)
{
  dword                 fwDelEvtHndlr0;

  if (!(fwDelEvtHndlr0 = FIS_fwDelEventHandler())) return FALSE;

  return CallFirmware (fwDelEvtHndlr0, EventID, (dword)EventHandler, 0, 0) != 0;
}
