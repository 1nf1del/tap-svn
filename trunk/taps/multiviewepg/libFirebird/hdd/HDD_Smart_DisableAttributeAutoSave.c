#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_Smart_DisableAttributeAutoSave (void)
{
  int                   Ret;

  Ret = SendHDDCommand (0xD2, 0x00, 0, 0x4F, 0xC2, 0xA0, 0xB0);
#ifdef DEBUG_FIREBIRDLIB
  if (Ret != 0) TAP_Print ("FireBirdLib: HDDSmartDisableAttributeAutoSave: Ret=%d\r\n", Ret);
#endif
  return Ret;
}
