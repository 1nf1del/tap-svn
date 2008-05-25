#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vTAP_Vfd_Control (void)
{
  dword                *p;
  static dword          vTAP_Vfd_Control = 0;

  if (!vTAP_Vfd_Control)
  {
    if (!LibInitialized) InitTAPex();
    if (!LibInitialized) return 0;

    p = (dword*)TAP_Vfd_Control;
    while (*p != JR_RA_CMD)
    {
      if ((*p & 0xffff0000) == 0xa3840000)
      {
        vTAP_Vfd_Control = FWgp + (short)(*p & 0xffff);
        break;
      }
      p++;
    }
  }

  return vTAP_Vfd_Control;
}
