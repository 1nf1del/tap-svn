#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_AAM_Disable (void)
{
  int                   Ret;

  Ret = SendHDDCommand (0xC2, 0, 0, 0, 0, 0xA0, 0xEF);
  if (Ret != 0)
  {
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: HDDDisableAAM: Ret=%d\r\n", Ret);
#endif

    //Send a "AAM Maximum Performance" to circumvent devices, which do not implement the "Disable AAM" command
    Ret=SendHDDCommand (0x42, 0xFE, 0, 0, 0, 0xA0, 0xEF);
#ifdef DEBUG_FIREBIRDLIB
    if (Ret != 0) TAP_Print ("FireBirdLib: HDDAAMDisable: Ret=%d\r\n", Ret);
#endif
  }
  return Ret;
}
