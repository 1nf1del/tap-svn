#include "FBLib_shutdown.h"
#include "../libFireBird.h"

bool Shutdown (TaskEnum Task)
{
  //static dword          ShutdownFcn = 0;

  if (!LibInitialized) InitTAPex ();
  if (!LibInitialized) return FALSE;

  //2008-01-20: EnqueueEventA is a new routine. Lets try it again.
  CallFirmware ((dword)EnqueueEventAddress, Task, 0, 0, 0);

  //if (Task != TaskPower) CallFirmware ((dword)EnqueueEventAddress, Task, 0, 0, 0);
  //else
  //{
  //  if (!ShutdownFcn)
  //  {
  //    if (!(ShutdownFcn = FIS_fwShutdown())) return FALSE;
  //  }
  //  CallFirmware (ShutdownFcn, 0, 0, 0, 0);
  //}
  return TRUE;
}
