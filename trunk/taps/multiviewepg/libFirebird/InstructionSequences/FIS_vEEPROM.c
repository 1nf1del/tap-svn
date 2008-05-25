#include "../libFireBird.h"
#include "../main/FBLib_main.h"

dword FIS_vEEPROM (void)
{
  dword                 *p;
  static dword          pEEPROM = 0;

  if (!pEEPROM)
  {
    if (!LibInitialized) InitTAPex ();
    if (!LibInitialized) return 0;

    //Scan for the "lw $??, ????($gp)" instruction in TAP_Channel_GetCurrent
    p = (dword *) TAP_Channel_GetCurrent;
    do
    {
      if ((*p & BASE_MASK) == LW_GP_CMD)
      {
        pEEPROM = FWgp + (short) (*p & 0x0000ffff);
        break;
      }
      p++;

      //ibbi 2007-01-07: at which address should be stopped anyway if there is no JR_RA_CMD?
      if ((dword) p > 0x82000000) return 0;

    } while (*p != JR_RA_CMD);

    if (pEEPROM) pEEPROM = *(dword *) pEEPROM - 2;
  }

  return pEEPROM;
}
