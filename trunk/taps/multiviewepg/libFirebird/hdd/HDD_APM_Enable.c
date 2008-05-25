#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_APM_Enable (byte APMLevel)
{
  int                   Ret;

  Ret = SendHDDCommand (0x05, APMLevel, 0, 0, 0, 0xA0, 0xEF);
#ifdef DEBUG_FIREBIRDLIB
  if (Ret != 0) TAP_Print ("FireBirdLib: HDDAPMEnable: Ret=%d\r\n", Ret);
#endif
  return Ret;
}
