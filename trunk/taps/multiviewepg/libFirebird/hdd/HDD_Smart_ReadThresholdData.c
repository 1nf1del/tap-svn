#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_Smart_ReadThresholdData (word *DataBuf)
{
  int                   Ret;
  dword                 RS;

  RS = intLock ();
  Ret = SendHDDCommand (0xD1, 0, 0, 0x4F, 0xC2, 0xA0, 0xB0);
  if (Ret != 0)
  {
    //If the ERR flag in the status register has been set,
    //the SMART feature has been disabled.
    intUnlock (RS);
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: HDDSmartReadThresholdData: Ret=%d\r\n", Ret);
#endif
    return 19;
  }
  Ret = ReceiveSector ((word *) DataBuf);
  intUnlock (RS);
  return Ret;
}
