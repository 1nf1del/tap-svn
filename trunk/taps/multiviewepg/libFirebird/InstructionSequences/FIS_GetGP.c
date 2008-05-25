#include                "../libFireBird.h"

dword FIS_GetGP (dword *UncompressedFirmware)
{
  dword                 i, gp = 0;

  //Calculate the $gp register of the loaded FW
  for (i = 0; i < 256; i++)
  {
    if ((UncompressedFirmware [i] & 0xffff0000) == LUI_GP_CMD) gp = (UncompressedFirmware [i] & 0x0000ffff) << 16;
    if ((UncompressedFirmware [i] & 0xffff0000) == ADDIU_GP_CMD)
    {
      gp += (short)(UncompressedFirmware [i] & 0x0000ffff);
      break;
    }
  }
  return gp;
}
