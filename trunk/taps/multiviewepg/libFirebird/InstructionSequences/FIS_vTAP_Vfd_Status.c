#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vTAP_Vfd_Status (void)
{
  dword                *p;
  static dword          vTAP_Vfd_Status = 0;

  if (!vTAP_Vfd_Status)
  {
    if (!LibInitialized) InitTAPex();
    if (!LibInitialized) return 0;

    p = (dword*)TAP_Vfd_GetStatus;
    while (*p != JR_RA_CMD)
    {
      if ((*p & 0xffff0000) == 0x93840000)
      {
        vTAP_Vfd_Status = FWgp + (short)(*p & 0xffff);
        break;
      }
      p++;
    }
  }

  return vTAP_Vfd_Status;
}
