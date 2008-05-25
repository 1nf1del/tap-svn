#include "FBLib_hdd.h"
#include "../libFireBird.h"

byte HDD_Smart_ExecuteOfflineImmediate (byte TestMode)
{
  int                   Ret;

  Ret = SendHDDCommand (0xD4, 0, TestMode, 0x4F, 0xC2, 0x00, 0xB0);
#ifdef DEBUG_FIREBIRDLIB
  if (Ret != 0) TAP_Print ("FireBirdLib: HDDSmartExecuteOfflineImmediates: Ret=%d\r\n", Ret);
#endif
  return *HDD_pLBAHigh;
}
