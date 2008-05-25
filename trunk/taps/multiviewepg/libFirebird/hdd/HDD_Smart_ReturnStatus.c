#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_Smart_ReturnStatus (void)
{
  int                   Ret;

  Ret = SendHDDCommand (0xDA, 0, 0, 0x4F, 0xC2, 0x00, 0xB0);
  if (Ret != 0)
  {
    //If the ERR flag in the status register has been set,
    //the SMART feature has been disabled.
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: HDDSmartReturnStatus: Ret=%d\r\n", Ret);
#endif
    return 19;
  }
  if (*HDD_pLBAMid == 0x4F) return 0;
                       else return 20;
}
