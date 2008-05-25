#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_SetStandbyTimer (dword TimeoutSec)
{
  int                   Ret;
  int                   RegValue;
  dword                 RS;

  RegValue = TimeoutSec / 5;
  if (RegValue > 240) RegValue = (TimeoutSec/1800) + 241;
  if (RegValue > 251) RegValue = 0;

  RS = intLock();
  Ret = SendHDDCommand (0, RegValue, 0, 0, 0, 0, 0xE3);
  intUnlock (RS);
#ifdef DEBUG_FIREBIRDLIB
  if (Ret != 0) TAP_Print ("FireBirdLib: HDDSetStandbyTimer: Ret=%d\r\n", Ret);
#endif
  return Ret;
}
