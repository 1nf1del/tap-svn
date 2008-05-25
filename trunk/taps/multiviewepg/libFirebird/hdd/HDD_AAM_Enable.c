#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_AAM_Enable (byte AAMLevel)
{
  int                   Ret;

  Ret = SendHDDCommand (0x42, AAMLevel, 0, 0, 0, 0xA0, 0xEF);
#ifdef DEBUG_FIREBIRDLIB
  if (Ret != 0) TAP_Print ("FireBirdLib: HDDAAMEnable: Ret=%d\r\n", Ret);
#endif
  return Ret;
}
