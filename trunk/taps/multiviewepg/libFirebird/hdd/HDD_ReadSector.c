#include "FBLib_hdd.h"
#include "../libFireBird.h"

int HDD_ReadSector (dword SectorNr, word *DataBuf)
{
  int                   Ret;
  dword                 RS;

  //Send a "ReadSector" command to the HDD
  RS = intLock ();
  Ret = SendHDDCommand (0, 1, SectorNr & 0xFF, (SectorNr >> 8) & 0xFF, (SectorNr >> 16) & 0xFF, 0x40, 0x20);
  if (Ret != 0)
  {
    intUnlock (RS);
#ifdef DEBUG_FIREBIRDLIB
    TAP_Print ("FireBirdLib: ReadHDD: Ret=%d\r\n", Ret);
#endif
    return Ret;
  }
  Ret = ReceiveSector ((word *) DataBuf);
  intUnlock (RS);
  SwapDWords ((dword *) DataBuf);
  return Ret;
}
