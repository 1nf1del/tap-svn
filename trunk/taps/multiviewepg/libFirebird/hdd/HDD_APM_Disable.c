#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_APM_Disable (void)
{
  int                   Ret;

  Ret = SendHDDCommand (0x85, 0x00, 0, 0, 0, 0xA0, 0xEF);
  if (Ret != 0)
  {
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: HDDDisableAPM: Ret=%d\r\n", Ret);
#endif

    //Send a "APM Maximum Performance" to circumvent devices, which do not implement the "Disable APM" command
    Ret = SendHDDCommand (0x05, 0xFE, 0, 0, 0, 0xA0, 0xEF);
#ifdef DEBUG_FIREBIRDLIB
    if (Ret != 0) TAP_Print ("FireBirdLib: HDDAPMEnable: Ret=%d\r\n", Ret);
#endif
  }
  return Ret;
}
