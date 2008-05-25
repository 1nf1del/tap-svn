#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_Stop (void)
{
  int                   Ret;
  dword                 RS;

  RS = intLock ();
  Ret = SendHDDCommand (0, 0, 0, 0, 0, 0, 0xE1);
  intUnlock (RS);
#ifdef DEBUG_FIREBIRDLIB
  if (Ret != 0) TAP_Print ("FireBirdLib: StopHDD: Ret=%d\r\n", Ret);
#endif
  return Ret;
}
