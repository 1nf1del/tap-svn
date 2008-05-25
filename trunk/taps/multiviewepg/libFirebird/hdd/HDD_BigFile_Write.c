#include "FBLib_hdd.h"
#include "../libFireBird.h"

void HDD_BigFile_Write (void *buf, dword SectorNr, dword NrSectors, TYPE_File *fp)
{
  dword                 AbsSector;

  AbsSector = CalcAbsSectorFromFAT (fp, SectorNr);
#ifdef DEBUG_FIREBIRDLIB
  TAP_Print ("FireBirdLib: Rel/Abs Sector = %6.6d/%6.6d\n", SectorNr, AbsSector);
#endif
  HDD_WriteSectorDMA (AbsSector, NrSectors, buf);
}
