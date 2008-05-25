#include "../libFireBird.h"

void InitTAPexFailedMsg(char *ProgramName)
{
  char                  s [28];

  TAP_SPrint (s, "%d %4.4x %8.8x", SysID, ApplID, FWgp);
  ShowMessageWin (ProgramName ? ProgramName : "", "FBLib failed to init", s, 500);
}
